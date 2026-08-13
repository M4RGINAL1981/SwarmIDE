#include "SwarmIDE.hpp"
#include "ui/command/CmdNodeOperations.hpp"
#include "core/graph/ConnectionManager.hpp"
#include "ui/command/CommandBus.hpp"
#include "ui/canvas/CanvasManager.hpp"
#include "graphics/Camera.hpp"

namespace {

    void createNode(UUIDv4::UUID& nodeID, const std::string& typeName, glm::vec2 worldPos,
                    std::unique_ptr<SwarmIDE::Nodes::LogicNode>& cachedNode, 
                    std::vector<SwarmIDE::Core::LogicConnection>& connectionForNode, bool& m_isFirstExecution) {

        
    }

    void deleteNode(UUIDv4::UUID nodeID, glm::vec2& worldPos, 
                    std::unique_ptr<SwarmIDE::Nodes::LogicNode>& cachedNode, 
                    std::vector<SwarmIDE::Core::LogicConnection>& connectionForNode) {

        
    }
}

namespace SwarmIDE::UI::Command {

    CmdCreateNode::CmdCreateNode(const std::string& typeName, const std::string& nodeName, const glm::vec2& worldPosNode) : typeName(typeName), nodeName(nodeName), worldPos(worldPosNode) {}

    void CmdCreateNode::Execute() {
        Core::LoggingManager::Info("[CREATE]: Node create.");

        if (m_isFirstExecution) {
            // ПЕРВЫЙ ЗАПУСК: Создаем с нуля
            cachedNode = Core::GraphRegistry::getInstance().createNode(typeName);
            if (!cachedNode) throw std::runtime_error("Node is not created");
            
            cachedNode->ApplyDefaultSettings();
            nodeID = cachedNode->GetID();
            m_isFirstExecution = false;
        }
        
        // Возвращаем нodу на холст (отдаем владение графу)
        Core::GraphManager::getInstance().addNode(std::move(cachedNode));
        CanvasManager::getInstance().AddViewNode(nodeID, worldPos);

    }
    void CmdCreateNode::Undo() { 
        Core::LoggingManager::Info("[DELETE]: Node is delete. Node ID: ", nodeID.str());
        
        auto viewNode = CanvasManager::getInstance().findViewNodeByID(nodeID);
        if(viewNode) worldPos = viewNode->GetWorldPos();
        CanvasManager::getInstance().RemoveViewNode(nodeID);  
        
        cachedNode = Core::GraphManager::getInstance().extractNode(nodeID);
    }

    CmdDeleteNode::CmdDeleteNode(std::optional<UUIDv4::UUID> nodeID) : nodeID(nodeID) {}

    void CmdDeleteNode::Execute() { 
        if(nodeID == std::nullopt) {
            //Core::LoggingManager::Error("Node is not deleted.");
            return;
        }
        Core::LoggingManager::Info("[CREATE]: Node is delete. Node ID: ", nodeID.value());
        
        auto& connection_manager = Core::ConnectionManager::getInstance();   
        connectionForNode = connection_manager.getAllConnectionsForNode(nodeID.value());

        auto viewNode = CanvasManager::getInstance().findViewNodeByID(nodeID.value());
        if(viewNode) worldPos = viewNode->GetWorldPos();
        CanvasManager::getInstance().RemoveViewNode(nodeID.value());
        
        cachedNode = Core::GraphManager::getInstance().extractNode(nodeID.value());
    }

    void CmdDeleteNode::Undo() { 
        if(nodeID == std::nullopt) {
            //Core::LoggingManager::Error("Node is not deleted.");
            return;
        }
        Core::LoggingManager::Info("[CREATE]: Node create.");
        
        // Возвращаем нodу на холст (отдаем владение графу)
        Core::GraphManager::getInstance().addNode(std::move(cachedNode));
        CanvasManager::getInstance().AddViewNode(nodeID.value(), worldPos);

        // Восстанавливаем связи
        for(auto& logicConnection : connectionForNode) {
            Core::ConnectionManager::getInstance().connect(logicConnection);
            CanvasManager::getInstance().AddViewConnection(logicConnection.getID()); 
        }
    }
}