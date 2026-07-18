#include "core/graph/nodes/MergeNode.hpp"
#include "core/ModelManager.hpp"
#include "core/LoggingManager.hpp"

namespace SwarmIDE::Core::Graph { 

    MergeNode::MergeNode(const std::string& nodeName) : LogicNode(nodeName) {
        // Установка входных пинов
        addInputPin("input_text1", "string");
        addInputPin("input_text2", "string");

        // Установка выходных пинов
        addOutputPin("output_text", "string");
    }

    void MergeNode::Execute() {
        // Устанавливаем ноде статус Executing (Выполнение)
        SetState(Core::Graph::Executing);                         

        // Логирование
        LoggingManager::Print("\n[EXECUTING] MergeNode ID: ", GetID(), "\n"); 

        try {
            std::string answer1 = getInputPinValue("input_text1").getValue<std::string>();
            std::string answer2 = getInputPinValue("input_text2").getValue<std::string>();
            setOutputPinValue("output_text", DataVariant(answer1 + answer2));

            SetState(Core::Graph::Success);
            LoggingManager::Print("\n[EXECUTION COMPLETED]: State: SUCCESS; MergeNode ID: ", GetID(), "\n"); 
        }
        catch (...) {
            SetState(Core::Graph::Failed);
            LoggingManager::Print("\n[EXECUTION COMPLETED]: State: Failed; MergeNode ID: ", GetID(), "\n"); 
        }
    }

    // РЕГИСТРАЦИЯ НОДЫ В ФАБРИКЕ
    REGISTER_NODE(MergeNode, "MergeNode");
}