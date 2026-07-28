#include "SwarmIDE.hpp"
#include "ui/command/CmdMoveNode.hpp"
#include "core/graph/GraphManager.hpp"
#include "ui/canvas/CanvasManager.hpp"
#include "core/engine/LoggingManager.hpp"

namespace SwarmIDE::UI::Command {

    void CmdMoveNode::Execute() {
        auto node = CanvasManager::getInstance().findViewNodeByID(nodeID);
        if (node) node->SetWorldPos(newPos); // Новая позиция
    }

    void CmdMoveNode::Undo()  {
        auto node = CanvasManager::getInstance().findViewNodeByID(nodeID);
        if (node) node->SetWorldPos(oldPos); // Вернуть предыдушую позицию
    }
}