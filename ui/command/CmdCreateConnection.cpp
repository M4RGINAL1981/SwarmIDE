#include "SwarmIDE.hpp"
#include "CmdCreateConnection.hpp"
#include "ui/canvas/CanvasManager.hpp"

namespace SwarmIDE::UI::Command {

    CmdCreateConnection::CmdCreateConnection(UUIDv4::UUID fromNodeID, UUIDv4::UUID fromPinID, UUIDv4::UUID toNodeID, UUIDv4::UUID toPinID) {
        this->fromNodeID = fromNodeID;
        this->fromPinID = fromPinID;
        this->toNodeID = toNodeID;
        this->toPinID = toPinID;
    }

    void CmdCreateConnection::Execute() {
        try {
            Core::LoggingManager::Info("[CMD_CONNECTION_CREATE]: [Execute]: Connection is create.");

            if (m_isFirstExecution) {
                auto* startNode = Core::GraphManager::getInstance().findNodeByID(fromNodeID);
                auto* endNode = Core::GraphManager::getInstance().findNodeByID(toNodeID);
                if (!startNode || !endNode) {
                    Core::LoggingManager::Warning("Connection creation aborted: one or both nodes were not found.");
                    return;
                }

                auto* startPin = startNode->findPin(fromPinID);
                auto* endPin = endNode->findPin(toPinID);
                if (!startPin || !endPin) {
                    Core::LoggingManager::Warning("Connection creation aborted: one or both pins were not found.");
                    return;
                }

                if (startPin->direction == endPin->direction || startPin->dataType != endPin->dataType || startPin->type != endPin->type) {
                    Core::LoggingManager::Warning("Connection creation aborted: pin direction or type mismatch.");
                    return;
                }

                auto connectionID = Core::ConnectionManager::getInstance().connect(fromNodeID, fromPinID, toNodeID, toPinID);
                if (!connectionID.has_value()) return;

                this->connectionID = connectionID.value();

                auto& canvas = CanvasManager::getInstance();
                if (canvas.findViewConnectionByID(this->connectionID) == nullptr) {
                    canvas.AddViewConnection(this->connectionID);
                }
                m_isFirstExecution = false;
                return;
            }

            if (cachedLogicConnection.has_value()) {
                auto recreatedConnectionID = Core::ConnectionManager::getInstance().connect(cachedLogicConnection.value());
                if (recreatedConnectionID.has_value()) {
                    this->connectionID = recreatedConnectionID.value();
                    CanvasManager::getInstance().AddViewConnection(this->connectionID);
                }
            }
        }
        catch (const std::exception& ex) {
            Core::LoggingManager::Error("Connection creation failed with exception: ", ex.what());
        }
        catch (...) {
            Core::LoggingManager::Error("Connection creation failed with unknown exception.");
        }
    }

    void CmdCreateConnection::Undo() {
        Core::LoggingManager::Info("[CMD_CONNECTION_CREATE]: [Undo]: Connection is delete.");

        auto viewConnection = CanvasManager::getInstance().findViewConnectionByID(this->connectionID);
        if(viewConnection) {
            cachedViewConnection = std::make_unique<UI::ViewConnection>(*viewConnection);
            CanvasManager::getInstance().RemoveViewConnection(this->connectionID);
            cachedLogicConnection = Core::ConnectionManager::getInstance().extractLogicConnection(this->connectionID);
        }
    }
}