#include "SwarmIDE.hpp"
#include "CmdCreateConnection.hpp"
#include "ui/canvas/CanvasManager.hpp"

namespace SwarmIDE::UI::Command {


    CmdCreateConnection::CmdCreateConnection(UUIDv4::UUID fromNodeID, Core::PinID fromPinID, int fromIndex,
                            UUIDv4::UUID toNodeID, Core::PinID toPinID, int toIndex) {
        this->fromNodeID = fromNodeID;
        this->fromPinID = fromPinID;
        this->fromIndex = fromIndex;

        this->toNodeID = toNodeID;
        this->toPinID = toPinID;
        this->toIndex = toIndex;
    }


    void CmdCreateConnection::Execute() {
        Core::LoggingManager::Info("[CMD]: [CREATE]: Connection is create.");

        // Создание логической связи
        auto conn = Core::ConnectionManager::getInstance().connect(fromNodeID, fromPinID, fromIndex, toNodeID, toPinID, toIndex);
        
        // Если связь не создалась - выходим
        if(!conn) return;

        if(CanvasManager::getInstance().findViewConnection(conn) == nullptr) {
            // Создание визуальной связи
            CanvasManager::getInstance().AddViewConnection(conn);
        }
    }

    void CmdCreateConnection::Undo() {
        Core::LoggingManager::Info("[REMOVE]: Connection is remove.");
        std::cout << "0" << std::endl;
        // Находим соединение
        auto conn = Core::ConnectionManager::getInstance().findConnection(fromNodeID, fromPinID, fromIndex, toNodeID, toPinID, toIndex);
        std::cout << "1" << std::endl;

        std::cout << "2" << std::endl;
        // Удаление визуальной связи
        if(!conn) return;
        CanvasManager::getInstance().RemoveViewConnection(conn);

        std::cout << "3" << std::endl;
        // Удаление логической связи
        if(!conn) return;
        Core::ConnectionManager::getInstance().disconnect(conn);
        std::cout << "4" << std::endl;
    }
}