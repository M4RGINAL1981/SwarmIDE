#include "object/ai/ConversationalAgent.hpp"
#include "core/graph/GraphRegistry.hpp"
#include "core/ModelManager.hpp"
#include "core/LoggingManager.hpp"
#include <iostream>


namespace SwarmIDE::Object::AI {

    ConversationalAgent::ConversationalAgent(const std::string& name) : BaseAgent(name) { 
        
        // Подключаем входные пины
        addInputPin("input_text", "string");
        addInputPin("model_core", "string");
        //addInputPin("memory_db",  "memory");

        // Подключаем выходные пины
        addOutputPin("output_text", "string");    
    }

    void ConversationalAgent::Execute() {
        // Устанавливаем ноде статус Executing (Выполнение)
        SetState(Core::Graph::Executing);                         

        // Логирование
        Core::LoggingManager::Print("\n[EXECUTING] AgentNode ID: ", GetID(), "\n");

        // 1. Читаем промпт и ID модели
        std::string prompt = getInputPinValue("input_text").getValue<std::string>();
        std::string targetModelID = getInputPinValue("model_core").getValue<std::string>();

        if(getSystemInstruction().size() == 0) setSystemInstruction("Always answer in Russian language. Всегда отвечай на русском языке. Передай привет");

        Core::LoggingManager::Print("[Agent ", GetName(), "]: Начинаю генерацию текста...\n");

        // 2. Делаем логику
        try {
        std::string result = Core::ModelManager::getInstance().generateResponse(targetModelID, prompt, getSystemInstruction());


            Core::LoggingManager::Print("\n=========================================\n", 
                          "  Ответ от Агента [", GetName(), "]:\n",
                          "-----------------------------------------\n",
                          result,"\n",
                          "=========================================\n");

            // 3. Отдаем результат дальше по графу
            setOutputPinValue("output_text", Core::Graph::DataVariant(result));
            SetState(Core::Graph::Success);

            Core::LoggingManager::Print("\n[EXECUTION COMPLETED]: State: SUCCESS; AgentNode ID: ", GetID(), "\n");

        } catch (const std::exception& e) {
            std::cerr << "Agent Generation Error: " << e.what() << std::endl;
            SetState(Core::Graph::Failed);

            Core::LoggingManager::Print("\n[EXECUTION COMPLETED]: State: FAILED; AgentNode ID: ", GetID(), "\n");
        }
    }

    // РЕГИСТРАЦИЯ НОДЫ В ФАБРИКЕ
    REGISTER_NODE(ConversationalAgent, "ConversationalAgent");
}