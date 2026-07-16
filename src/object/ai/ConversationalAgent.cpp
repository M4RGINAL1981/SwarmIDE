#include "object/ai/ConversationalAgent.hpp"
#include "core/graph/GraphRegistry.hpp"
#include "core/ModelManager.hpp"
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
        SetState(Core::Graph::Executing);

        // 1. Читаем промпт и ID модели
        std::string prompt = getInputPinValue("input_text").getValue<std::string>();
        std::string targetModelID = getInputPinValue("model_core").getValue<std::string>();

        if(getSystemInstruction().size() == 0) setSystemInstruction("Always answer in Russian language. Всегда отвечай на русском языке. Передай привет");

        std::cout << "[Agent " << GetName() << "]: Начинаю генерацию текста..." << std::endl;

        // 2. Делаем логику
        try {
        std::string result = Core::ModelManager::getInstance().generateResponse(targetModelID, prompt, getSystemInstruction());

            // --- ВОТ ЭТОТ ВЫВОД МЫ ДОБАВЛЯЕМ ---
            std::cout << "\n=========================================" << std::endl;
            std::cout << "  Ответ от Агента [" << GetName() << "]:" << std::endl;
            std::cout << "-----------------------------------------" << std::endl;
            std::cout << result << std::endl;
            std::cout << "=========================================\n" << std::endl;
            // ------------------------------------

            // 3. Отдаем результат дальше по графу
            setOutputPinValue("output_text", Core::Graph::DataVariant(result));
            SetState(Core::Graph::Success);
        } catch (const std::exception& e) {
            std::cerr << "Agent Generation Error: " << e.what() << std::endl;
            SetState(Core::Graph::Failed);
        }
    }

    // РЕГИСТРАЦИЯ НОДЫ В ФАБРИКЕ
    REGISTER_NODE(ConversationalAgent, "ConversationalAgent");
}