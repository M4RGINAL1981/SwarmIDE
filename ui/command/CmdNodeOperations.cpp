#include "SwarmIDE.hpp"
#include "ui/command/CmdNodeOperations.hpp"
#include "core/graph/ConnectionManager.hpp"
#include "ui/command/CommandBus.hpp"
#include "ui/canvas/CanvasManager.hpp"
#include "graphics/Camera.hpp"

namespace SwarmIDE::UI::Command {

    /* ########################################
                CMD_CREATE_NODE
       ######################################## */

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

    /* ########################################
                CMD_DELETE_NODE
       ######################################## */

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


    /* ########################################
                 CMD_DUPLICATE_NODE
       ######################################## */

    CmdDuplicateNode::CmdDuplicateNode(std::optional<UUIDv4::UUID> nodeID) {
        if(nodeID == std::nullopt) return;
        this->nodeID = nodeID;

        auto logicNode = Core::GraphManager::getInstance().findNodeByID(nodeID.value());
        auto viewNode = UI::CanvasManager::getInstance().findViewNodeByID(nodeID.value());
        
        worldPos = viewNode->GetWorldPos() + glm::vec2(1.0f, 1.0f);

        nodeName = logicNode->GetName();
        nodeTypeName = logicNode->getTypeName();
    }

    void CmdDuplicateNode::Execute() {
        if(nodeID == std::nullopt) return;
        Core::LoggingManager::Info("[CREATE]: Node is duplicate. Original Node ID: ", nodeID.value().str());

        if (m_isFirstExecution) {
            // Создаем новую ноду того же типа
            cachedNode = Core::GraphRegistry::getInstance().createNode(nodeTypeName);
            if (!cachedNode) return;
            
            cachedNode->ApplyDefaultSettings();
            
            // Здесь (если есть метод) нужно скопировать параметры из оригинальной ноды.
            // Например: cachedNode->CopyStateFrom(Core::GraphManager::getInstance().findNodeByID(nodeID.value()));

            newNodeID = cachedNode->GetID();
            m_isFirstExecution = false;
        }
        
        // Отдаем владение графу
        Core::GraphManager::getInstance().addNode(std::move(cachedNode));
        CanvasManager::getInstance().AddViewNode(newNodeID, worldPos);
    }

    void CmdDuplicateNode::Undo() {
        if(nodeID == std::nullopt) return;
        Core::LoggingManager::Info("[DELETE]: Duplicated node is deleted. New Node ID: ", newNodeID.str());

        // Извлекаем ноду обратно в кэш команды
        CanvasManager::getInstance().RemoveViewNode(newNodeID);
        cachedNode = Core::GraphManager::getInstance().extractNode(newNodeID);
    }


    /* ########################################
                 CMD_MOVE_NODE
       ######################################## */

    CmdMoveNode::CmdMoveNode(UUIDv4::UUID nodeID, const glm::vec2& delta) : nodeID(nodeID), delta(delta) {}

    void CmdMoveNode::Execute() {
        auto viewNode = CanvasManager::getInstance().findViewNodeByID(nodeID);
        if (viewNode) {
            viewNode->MoveBy(delta);
            Core::LoggingManager::Info("[MOVE]: Node moved to new position."); // Можно раскомментировать для дебага
        }
    }

    void CmdMoveNode::Undo() {
        auto viewNode = CanvasManager::getInstance().findViewNodeByID(nodeID);
        if (viewNode) {
            viewNode->MoveBy(-delta);
            Core::LoggingManager::Info("[UNDO MOVE]: Node returned to old position.");
        }
    }

} // namespace SwarmIDE::UI::Command
