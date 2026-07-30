#include "SwarmIDE.hpp"
#include "nodes/ai/PlanExecuteAgent.hpp"
#include "core/graph/GraphRegistry.hpp"
#include "core/models/ModelManager.hpp"
#include "core/engine/LoggingManager.hpp"
#include <iostream>

namespace SwarmIDE::Nodes {

    PlanExecuteAgent::PlanExecuteAgent(const std::string& name) : BaseAgent(name) {  
        
        // Подключаем входные пины
        addPin(Core::PinID::DataStream, Core::Direction::Input, Core::DataType::STRING);
        addPin(Core::PinID::Model, Core::Direction::Input, Core::DataType::STRING);
        addPin(Core::PinID::Memory, Core::Direction::Input, Core::DataType::STRING);
        addPin(Core::PinID::Tools, Core::Direction::Input, Core::DataType::STRING);

        // Подключаем выходные пины
        addPin(Core::PinID::DataStream, Core::Direction::Output, Core::DataType::STRING);  
    }

    void PlanExecuteAgent::Execute() {
        // Устанавливаем ноде статус Executing (Выполнение)
        SetState(Executing);                         

        // Логирование
        Core::LoggingManager::ExecutingNode("ConversationalAgent", GetID());

        // 1. Читаем промпт и ID модели
        std::string prompt = getPinValue(Core::PinID::DataStream, Core::Direction::Input).getValue<std::string>();
        std::string targetModelID = getPinValue(Core::PinID::Model, Core::Direction::Input).getValue<std::string>();

        if(getSystemInstruction().size() == 0) setSystemInstruction("Always answer in Russian language. Всегда отвечай на русском языке. Передай привет");

        Core::LoggingManager::Info("[Agent ", GetName(), "]: Начинаю генерацию текста...\n");

        // 2. Делаем логику
        try {
        std::string result = Core::ModelManager::getInstance().generateResponse(targetModelID, prompt, getSystemInstruction());


            Core::LoggingManager::Info( "  Ответ от Агента [", GetName(), "]:\n", result,"\n");

            // 3. Отдаем результат дальше по графу
            setPinValue(Core::DataVariant(result), Core::PinID::DataStream, Core::Direction::Output);
            SetState(Success);

            Core::LoggingManager::SuccessNode("ConversationalAgent", GetID());

        } catch (const std::exception& e) {
            std::cerr << "Agent Generation Error: " << e.what() << std::endl;
            SetState(Failed);

            Core::LoggingManager::FailedNode("ConversationalAgent", GetID());
        }
    }

    json PlanExecuteAgent::serializeNode() {

        json nodeJson;

        nodeJson["id"] = GetID().str();
        nodeJson["name"] = GetName();
            
        nodeJson["type"] = getTypeName(); 
        nodeJson["state"] = GetState();
        nodeJson["SystemInstruction"] = getSystemInstruction();
        
        return nodeJson;
    }

    void PlanExecuteAgent::deserializeNode(json nodeJson) {
        
        SetID(UUIDv4::UUID(nodeJson["id"].get<std::string>()));
        SetName(nodeJson["name"]);
        SetState(nodeJson["state"]);
        setSystemInstruction(nodeJson["SystemInstruction"]);
    }

    
    void PlanExecuteAgent::ApplyDefaultSettings() {
        SetName("PlanExecuteAgent");
        setSystemInstruction("You are Alpha-Agent. Write down your instructions here...");
    }

    // РЕГИСТРАЦИЯ НОДЫ В ФАБРИКЕ
    REGISTER_NODE(PlanExecuteAgent, "PlanExecuteAgent");
}
