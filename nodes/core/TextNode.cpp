#include "SwarmIDE.hpp"
#include "nodes/core/TextNode.hpp"
#include "core/models/ModelManager.hpp"
#include "core/engine/LoggingManager.hpp"

namespace SwarmIDE::Nodes {

    TextNode::TextNode(const std::string& nodeName) : LogicNode(nodeName) {
        // Подключаем выходные пины
        addPin("Output", Core::PinType::DataStream, Core::Direction::Output, Core::DataType::STRING);
    }

    TextNode::TextNode(const std::string& nodeName, const std::string& textContent) : LogicNode(nodeName) {
        text = textContent;

        // Подключаем выходные пины
        addPin("Output", Core::PinType::DataStream, Core::Direction::Output, Core::DataType::STRING);
    }
  
    void TextNode::Execute() {
        // Устанавливаем ноде статус Executing (Выполнение)
        SetState(Executing);                         

        // Логирование
        Core::LoggingManager::ExecutingNode("TextNode", GetID()); 


        try {
            setPinValue("Output", Core::DataVariant(text));
            SetState(Success);
            
            Core::LoggingManager::SuccessNode("TextNode", GetID()); 
        }
        catch (...) {
            SetState(Failed);

            Core::LoggingManager::FailedNode("TextNode", GetID()); 
        }
    }

    json TextNode::serializeNode() {
        json nodeJson = LogicNode::serializeNode();
        nodeJson["text"] = getText();
            
        return nodeJson;
    }

    void TextNode::deserializeNode(json nodeJson) {
        LogicNode::deserializeNode(nodeJson);
        setText(nodeJson["text"]);
    }

    void TextNode::ApplyDefaultSettings() {
        SetName("TextNode");
        setText("This is your prompt");
    }

    // РЕГИСТРАЦИЯ НОДЫ В ФАБРИКЕ
    REGISTER_NODE(TextNode, "TextNode");
}