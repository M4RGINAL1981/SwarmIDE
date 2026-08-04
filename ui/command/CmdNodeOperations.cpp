#include "SwarmIDE.hpp"
#include "ui/command/CmdNodeOperations.hpp"
#include "core/graph/ConnectionManager.hpp"
#include "ui/command/CommandBus.hpp"
#include "ui/canvas/CanvasManager.hpp"

namespace {

    void createNode(UUIDv4::UUID& nodeID, const std::string& typeName, glm::vec2 worldPos,
                    std::unique_ptr<SwarmIDE::Nodes::LogicNode>& cachedNode, 
                    std::vector<SwarmIDE::Core::LogicConnection>& connectionForNode, bool& m_isFirstExecution) {

        SwarmIDE::Core::LoggingManager::Info("[CREATE]: Node execute.");

        if (m_isFirstExecution) {
            // ПЕРВЫЙ ЗАПУСК: Создаем с нуля
            cachedNode = SwarmIDE::Core::GraphRegistry::getInstance().createNode(typeName);
            if (!cachedNode) throw std::runtime_error("Node is not created");
            
            cachedNode->ApplyDefaultSettings();
            nodeID = cachedNode->GetID();
            m_isFirstExecution = false;
        }
        
        // Возвращаем нodу на холст (отдаем владение графу)
        SwarmIDE::Core::GraphManager::getInstance().addNode(std::move(cachedNode));
        SwarmIDE::UI::CanvasManager::getInstance().AddViewNode(nodeID, worldPos);

        // Восстанавливаем связи
        for(auto& logicConnection : connectionForNode) {
            SwarmIDE::Core::ConnectionManager::getInstance().connect(logicConnection);
            SwarmIDE::UI::CanvasManager::getInstance().AddViewConnection(logicConnection.getID()); 
        }
    }

    void deleteNode(UUIDv4::UUID nodeID, glm::vec2& worldPos, 
                    std::unique_ptr<SwarmIDE::Nodes::LogicNode>& cachedNode, 
                    std::vector<SwarmIDE::Core::LogicConnection>& connectionForNode) {

        SwarmIDE::Core::LoggingManager::Info("[REMOVE]: Node undo. Node ID: ", nodeID);
        
        auto viewNode = SwarmIDE::UI::CanvasManager::getInstance().findViewNodeByID(nodeID);
        if(viewNode) worldPos = viewNode->GetWorldPos();

        auto& connection_manager = SwarmIDE::Core::ConnectionManager::getInstance();   
        connectionForNode = connection_manager.getAllConnectionsForNode(nodeID);

        SwarmIDE::UI::CanvasManager::getInstance().RemoveViewNode(nodeID);
        
        cachedNode = SwarmIDE::Core::GraphManager::getInstance().extractNode(nodeID);
    }
}

namespace SwarmIDE::UI::Command {

    CmdCreateNode::CmdCreateNode(const std::string& typeName, const std::string& nodeName) : typeName(typeName), nodeName(nodeName) {}
    void CmdCreateNode::Execute() { createNode(nodeID, typeName, worldPos, cachedNode, connectionForNode, m_isFirstExecution); }
    void CmdCreateNode::Undo() { deleteNode(nodeID, worldPos, cachedNode, connectionForNode); }

    CmdDeleteNode::CmdDeleteNode(const std::string& typeName, const std::string& nodeName) : typeName(typeName), nodeName(nodeName) {}
    void CmdDeleteNode::Execute() { deleteNode(nodeID, worldPos, cachedNode, connectionForNode); }
    void CmdDeleteNode::Undo() { createNode(nodeID, typeName, worldPos, cachedNode, connectionForNode, m_isFirstExecution); }
}