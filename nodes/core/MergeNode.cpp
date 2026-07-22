#include "SwarmIDE.hpp"
#include "nodes/core/MergeNode.hpp"
#include "core/models/ModelManager.hpp"
#include "core/engine/LoggingManager.hpp"

namespace SwarmIDE::Nodes {

    MergeNode::MergeNode(const std::string& nodeName) : LogicNode(nodeName) {
        // Установка входных пинов
        addPin(Core::PinID::DataStream, Core::Direction::Input, Core::DataType::STRING, 0);
        addPin(Core::PinID::DataStream, Core::Direction::Input, Core::DataType::STRING, 1);

        // Установка выходных пинов
        addPin(Core::PinID::DataStream, Core::Direction::Output, Core::DataType::STRING, 0);
    }

    void MergeNode::Execute() {
        // Устанавливаем ноде статус Executing (Выполнение)
        SetState(Executing);                         

        // Логирование
        Core::LoggingManager::ExecutingNode("MergeNode", GetID()); 

        try {
            std::string answer1 = getPinValue(Core::PinID::DataStream, Core::Direction::Input, 0).getValue<std::string>();
            std::string answer2 = getPinValue(Core::PinID::DataStream, Core::Direction::Input, 1).getValue<std::string>();
            setPinValue(Core::DataVariant(answer1 + answer2), Core::PinID::DataStream, Core::Direction::Output);

            SetState(Success);
            Core::LoggingManager::SuccessNode("MergeNode", GetID()); 
        }
        catch (...) {
            SetState(Failed);
            Core::LoggingManager::FailedNode("MergeNode", GetID()); 
        }
    }

    json MergeNode::serializeNode() {

        json nodeJson;

        nodeJson["id"] = GetID().str();
        nodeJson["name"] = GetName();
            
        nodeJson["type"] = getTypeName(); 
        nodeJson["state"] = GetState();
        
        return nodeJson;
    }

    void MergeNode::deserializeNode(json nodeJson) {
        
        SetID(UUIDv4::UUID(nodeJson["id"].get<std::string>()));
        SetName(nodeJson["name"]);
        SetState(nodeJson["state"]);
    }

    // РЕГИСТРАЦИЯ НОДЫ В ФАБРИКЕ
    REGISTER_NODE(MergeNode, "MergeNode");
}