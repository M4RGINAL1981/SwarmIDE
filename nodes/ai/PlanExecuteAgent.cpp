#include "SwarmIDE.hpp"
#include "nodes/ai/PlanExecuteAgent.hpp"
#include "core/graph/GraphRegistry.hpp"
#include "imgui.h"

namespace SwarmIDE::Nodes {

    PlanExecuteAgent::PlanExecuteAgent(const std::string& name) : BaseAgent(name) {  
        
        // Подключаем входные пины
        addInputPin("input_text", "string");
        addInputPin("model_core", "model");
        addInputPin("memory_db",  "memory");
        addInputPin("tool", "tool");

        // Подключаем выходные пины
        addOutputPin("output_text", "string");  
    }

    void PlanExecuteAgent::Execute() {
        SetState(Executing); // Устанавливаем ноде статус Executing (Выполнение)

        // 1. Читаем данные из входного пина
        Core::DataVariant inputData = getInputPinValue("input_text");

        std::string incomingText = "";
        if (inputData.isType<std::string>()) {
            incomingText = inputData.getValue<std::string>();
        }

        // 2. Делаем логику (здесь будет вызов LLM, но пока просто тест)
        std::string resultText = "Agent " + GetName() + " says: I received -> " + incomingText;
        std::cout << resultText << std::endl;

        // 3. Записываем результат в ВЫХОДНОЙ пин
        setOutputPinValue("output_text", Core::DataVariant(resultText));

        SetState(Success);

        /*
        1. Берет `DataVariant` из пина `"model_core"`. Извлекает оттуда указатель на модель (`BaseModel*` или `llama_model*`).
        
        2. Берет строку из пина `"input_text"` (запрос пользователя).
            
        3. Берет историю чата из пина `"memory_db"` (если подключена база данных).
            
        4. Соединяет всё это с приватной переменной `systemInstruction`.
            
        5. Вызывает инференс (генерацию текста).
            
        6. Полученный текстовый ответ записывает в свой выходной пин `"output_text"`.
            
        7. Меняет свой статус на `Success`.
        */
    }

    
    // РЕГИСТРАЦИЯ НОДЫ В ФАБРИКЕ
    REGISTER_NODE(PlanExecuteAgent, "PlanExecuteAgent");
}
