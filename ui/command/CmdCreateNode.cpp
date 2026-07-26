#include "SwarmIDE.hpp"
#include "ui/command/CmdCreateNode.hpp"
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
        CanvasManager::getInstance().AddViewNode(nodeID, glm::vec2(0.0f, 0.0f));
    }

    void CmdCreateNode::Undo() {
        Core::LoggingManager::Info("[REMOVE]: Node is remove. Node ID: ", nodeID);

        // Удаление визуальной ноды
        CanvasManager::getInstance().RemoveViewNode(nodeID);

        // Удаление логической ноды
        Core::GraphManager::getInstance().removeNode(nodeID);
    }

}