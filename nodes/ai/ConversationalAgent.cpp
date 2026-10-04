#include "SwarmIDE.hpp"
#include "nodes/ai/ConversationalAgent.hpp"
#include "core/graph/GraphRegistry.hpp"
#include "core/models/ModelManager.hpp"
#include "core/engine/LoggingManager.hpp"
#include <iostream>

namespace SwarmIDE::Nodes {

    ConversationalAgent::ConversationalAgent(const std::string& name) : BaseAgent(name) { 
        
        // Подключаем входные пины
        addPin("Data", Core::PinType::DataStream, Core::Direction::Input, Core::DataType::STRING);
        addPin("Model", Core::PinType::Model, Core::Direction::Input, Core::DataType::STRING);

        // Подключаем выходные пины
        addPin("Output", Core::PinType::DataStream, Core::Direction::Output, Core::DataType::STRING);    
    }

    void ConversationalAgent::Execute() {
        // Устанавливаем ноде статус Executing (Выполнение)
        SetState(Executing);                         

        // Логирование
        Core::LoggingManager::ExecutingNode("ConversationalAgent", GetID());

        // 1. Читаем промпт и ID модели
        std::string prompt = getPinValue("Data").getValue<std::string>();
        std::string targetModelID = getPinValue("Model").getValue<std::string>();

        if(getSystemInstruction().size() == 0) setSystemInstruction("Always answer in Russian language. Всегда отвечай на русском языке. Передай привет");

        Core::LoggingManager::Info("[Agent ", GetName(), "]: Начинаю генерацию текста...\n");

        // 2. Делаем логику
        try {
        std::string result = Core::ModelManager::getInstance().generateResponse(targetModelID, prompt, getSystemInstruction());


            Core::LoggingManager::Info( "  Ответ от Агента [", GetName(), "]:\n", result,"\n");

            // 3. Отдаем результат дальше по графу
            setPinValue("Output", Core::DataVariant(result));
            SetState(Success);

            Core::LoggingManager::SuccessNode("ConversationalAgent", GetID());

        } catch (const std::exception& e) {
            std::cerr << "Agent Generation Error: " << e.what() << std::endl;
            SetState(Failed);

            Core::LoggingManager::FailedNode("ConversationalAgent", GetID());
        }
    }

    void ConversationalAgent::ApplyDefaultSettings() {
        SetName("ConversationalAgent");
        setSystemInstruction("You are Alpha-Agent. Write down your instructions here...");
    }

    // РЕГИСТРАЦИЯ НОДЫ В ФАБРИКЕ
    REGISTER_NODE(ConversationalAgent, "ConversationalAgent", "Conversational Agent", "AGENTS");
}