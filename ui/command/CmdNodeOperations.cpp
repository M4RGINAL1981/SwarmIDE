#include "SwarmIDE.hpp"
#include "ui/command/CmdNodeOperations.hpp"
#include "core/graph/ConnectionManager.hpp"
#include "ui/command/CommandBus.hpp"
#include "ui/canvas/CanvasManager.hpp"
#include "nodes/core/TextNode.hpp"
#include "nodes/core/ModelNode.hpp"

namespace SwarmIDE::UI::Command {

    CmdCreateNode::CmdCreateNode(const std::string& typeName, const std::string& nodeName) : typeName(typeName), nodeName(nodeName) {}

    void CmdCreateNode::Execute() {
        Core::LoggingManager::Info("[CREATE]: Node is create.");

        // Создание логической ноды
        auto newNode = Core::GraphRegistry::getInstance().createNode(typeName);

        if (!newNode) {
            Core::LoggingManager::Fatal("Node is not created");
            throw std::runtime_error("Node is not created");
        }

        newNode->ApplyDefaultSettings();
        nodeID = newNode->GetID();

        // Создание визуальной ноды
        Core::GraphManager::getInstance().addNode(std::move(newNode));
        CanvasManager::getInstance().AddViewNode(nodeID, worldPos);

        // Если соединений нет - заканчиваем работу
        if(connectionForNode.empty()) return;

        for(auto& conn : connectionForNode) {
            // Создание логической связи
            Core::ConnectionManager::getInstance().connect(&conn);
            CanvasManager::getInstance().AddViewConnection(&conn);
        }
    }

    void CmdCreateNode::Undo() {
        Core::LoggingManager::Info("[REMOVE]: Node is remove. Node ID: ", nodeID);
        
        auto viewNode = CanvasManager::getInstance().findViewNodeByID(nodeID);

        // Сохаранение положение ноды на экране
        if(!viewNode) return;
        worldPos = viewNode->GetWorldPos();

        auto& connection_manager = Core::ConnectionManager::getInstance();   

        // Сохраняем связи с нодой
        connectionForNode = connection_manager.getAllConnectionsForNode(nodeID);
        // Удаляем все связи с нодой
        connection_manager.removeConnectionsForNode(nodeID);

        // Удаление визуальной ноды
        CanvasManager::getInstance().RemoveViewNode(nodeID);
        // Удаление логической ноды
        Core::GraphManager::getInstance().removeNode(nodeID);
    }

    CmdDeleteNode::CmdDeleteNode(const std::string& typeName, const std::string& nodeName) : typeName(typeName), nodeName(nodeName) {}

    void CmdDeleteNode::Execute() {
        Core::LoggingManager::Info("[REMOVE]: Node is remove. Node ID: ", nodeID);
        
        auto viewNode = CanvasManager::getInstance().findViewNodeByID(nodeID);

        // Сохаранение положение ноды на экране
        if(!viewNode) return;
        worldPos = viewNode->GetWorldPos();

        auto& connection_manager = Core::ConnectionManager::getInstance();   

        // Сохраняем связи с нодой
        connectionForNode = connection_manager.getAllConnectionsForNode(nodeID);
        // Удаляем все связи с нодой
        connection_manager.removeConnectionsForNode(nodeID);

        // Удаление визуальной ноды
        CanvasManager::getInstance().RemoveViewNode(nodeID);
        // Удаление логической ноды
        Core::GraphManager::getInstance().removeNode(nodeID);
    }

    void CmdDeleteNode::Undo() {
        Core::LoggingManager::Info("[CREATE]: Node is create.");

        // Создание логической ноды
        auto newNode = Core::GraphRegistry::getInstance().createNode(typeName);

        if (!newNode) {
            Core::LoggingManager::Fatal("Node is not created");
            throw std::runtime_error("Node is not created");
        }

        newNode->SetName(nodeName);

        /** @todo Нужно полностью переделать данную проверку на типы, 
         * Иначе, когда будет намного больше типов, тут будет ебаннутая полоса методов. 
         */
        if(typeName == "ModelNode") {
            auto mNode1 = dynamic_cast<Nodes::ModelNode*>(newNode.get());
            if (mNode1) mNode1->setTargetModelID("0");
        }
        
        if(typeName == "TextNode") {
            auto tNode1 = dynamic_cast<Nodes::TextNode*>(newNode.get());
            if(tNode1) tNode1->setText("You are Alpha-Agent. You conncetion with Gramma-Agent. Say hello, state your name, and tell the other agent that he is a Gamma-Agent.");
        }

        nodeID = newNode->GetID();

        // Создание визуальной ноды
        Core::GraphManager::getInstance().addNode(std::move(newNode));
        CanvasManager::getInstance().AddViewNode(nodeID, worldPos);

        // Если соединений нет - заканчиваем работу
        if(connectionForNode.empty()) return;

        for(auto& conn : connectionForNode) {
            // Создание логической связи
            Core::ConnectionManager::getInstance().connect(&conn);
            CanvasManager::getInstance().AddViewConnection(&conn);
        }
    }
}