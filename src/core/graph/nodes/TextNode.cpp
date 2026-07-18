#include "core/graph/nodes/TextNode.hpp"
#include "core/ModelManager.hpp"
#include "core/LoggingManager.hpp"

namespace SwarmIDE::Core::Graph { 

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
        SetState(Core::Graph::Executing);                         

        // Логирование
        LoggingManager::Print("\n[EXECUTING] TextNode ID: ", GetID(), "\n"); 


        try {
            setOutputPinValue("output_text", DataVariant(text));
            SetState(Core::Graph::Success);
            
            LoggingManager::Print("\n[EXECUTION COMPLETED]: State: FAILED; AgentNode ID: ", GetID(), "\n"); 
        }
        catch (...) {
            SetState(Core::Graph::Failed);

            LoggingManager::Print("\n[EXECUTION COMPLETED]: State: FAILED; TextNode ID: ", GetID(), "\n"); 
        }
    }

    // РЕГИСТРАЦИЯ НОДЫ В ФАБРИКЕ
    REGISTER_NODE(TextNode, "TextNode");
}