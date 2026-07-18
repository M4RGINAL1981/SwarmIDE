#include "SwarmIDE.hpp"
#include "nodes/core/TextNode.hpp"
#include "core/models/ModelManager.hpp"
#include "core/engine/LoggingManager.hpp"

namespace SwarmIDE::Nodes {

    TextNode::TextNode(const std::string& nodeName) : LogicNode(nodeName) {
        addOutputPin("output_text", "string");
    }

    TextNode::TextNode(const std::string& nodeName, const std::string& textContent) : LogicNode(nodeName) {
        text = textContent;

        // Подключаем выходные пины
        addOutputPin("output_text", "string"); 
    }
  
    void TextNode::Execute() {
        // Устанавливаем ноде статус Executing (Выполнение)
        SetState(Executing);                         

        // Логирование
        Core::LoggingManager::Print("\n[EXECUTING] TextNode ID: ", GetID(), "\n"); 


        try {
            setOutputPinValue("output_text", Core::DataVariant(text));
            SetState(Success);
            
            Core::LoggingManager::Print("\n[EXECUTION COMPLETED]: State: FAILED; TextNode ID: ", GetID(), "\n"); 
        }
        catch (...) {
            SetState(Failed);

            Core::LoggingManager::Print("\n[EXECUTION COMPLETED]: State: FAILED; TextNode ID: ", GetID(), "\n"); 
        }
    }

    // РЕГИСТРАЦИЯ НОДЫ В ФАБРИКЕ
    REGISTER_NODE(TextNode, "TextNode");
}