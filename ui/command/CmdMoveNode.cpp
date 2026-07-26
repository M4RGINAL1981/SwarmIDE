#include "SwarmIDE.hpp"
#include "ui/command/CmdMoveNode.hpp"
#include "core/graph/GraphManager.hpp"
#include "ui/canvas/CanvasManager.hpp"
#include "core/engine/LoggingManager.hpp"

namespace SwarmIDE::UI::Command {

    CmdMoveNode::CmdMoveNode(const UUIDv4::UUID& nodeID, const glm::vec2& pos) : nodeID(nodeID), pos(pos) {}

    void CmdMoveNode::Execute() {
        auto viewNode = CanvasManager::getInstance().findViewNodeByID(nodeID);

        if(!viewNode) {
            Core::LoggingManager::Fatal("Node is not found. Node ID: ", nodeID);
            throw std::runtime_error("Node is not found");
        }

        // Cохраняем координаты до перемещения
        pos = viewNode->GetWorldPos();
    }

    void CmdMoveNode::Undo() {
        auto viewNode = CanvasManager::getInstance().findViewNodeByID(nodeID);

        if(!viewNode) {
            Core::LoggingManager::Fatal("Node is not found. Node ID: ", nodeID);
            throw std::runtime_error("Node is not found");
        }

        // Перемещаем ноду обратно
        viewNode->SetWorldPos(pos);
    }
}