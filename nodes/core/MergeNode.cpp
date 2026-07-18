#include "SwarmIDE.hpp"
#include "nodes/core/MergeNode.hpp"
#include "core/models/ModelManager.hpp"
#include "core/engine/LoggingManager.hpp"

namespace SwarmIDE::Nodes {

    MergeNode::MergeNode(const std::string& nodeName) : LogicNode(nodeName) {
        // Установка входных пинов
        addInputPin("input_text1", "string");
        addInputPin("input_text2", "string");

        // Установка выходных пинов
        addOutputPin("output_text", "string");
    }

    void MergeNode::Execute() {
        // Устанавливаем ноде статус Executing (Выполнение)
        SetState(Executing);                         

        // Логирование
        Core::LoggingManager::Print("\n[EXECUTING] MergeNode ID: ", GetID(), "\n"); 

        try {
            std::string answer1 = getInputPinValue("input_text1").getValue<std::string>();
            std::string answer2 = getInputPinValue("input_text2").getValue<std::string>();
            setOutputPinValue("output_text", Core::DataVariant(answer1 + answer2));

            SetState(Success);
            Core::LoggingManager::Print("\n[EXECUTION COMPLETED]: State: SUCCESS; MergeNode ID: ", GetID(), "\n"); 
        }
        catch (...) {
            SetState(Failed);
            Core::LoggingManager::Print("\n[EXECUTION COMPLETED]: State: Failed; MergeNode ID: ", GetID(), "\n"); 
        }
    }

    // РЕГИСТРАЦИЯ НОДЫ В ФАБРИКЕ
    REGISTER_NODE(MergeNode, "MergeNode");
}