#include "SwarmIDE.hpp"
#include "nodes/base/LogicNode.hpp"
#include "core/graph/LogicPin.hpp"
#include "core/graph/DataVariant.hpp"

#include <unordered_map>

namespace SwarmIDE::Nodes {


    void LogicNode::addPin(const std::string& pinName, Core::PinType type, Core::Direction dir, Core::DataType dataType, int maxConn, Core::DataVariant value) {
        Core::LogicPin* pin = new Core::LogicPin(type, dir, dataType, id, maxConn, value);
        pins[pin->GetID()] = std::unique_ptr<Core::LogicPin>(pin);
        pinNameToID[pinName] = pin->GetID();
    }

    Core::DataVariant LogicNode::getPinValue(UUIDv4::UUID pinID) {
        auto pin = findPin(pinID);
        if(pin != nullptr) return pin->value;
        return Core::DataVariant();
    }

    Core::DataVariant LogicNode::getPinValue(const std::string& pinName) {
        auto pin = findPin(pinNameToID[pinName]);
        if(pin != nullptr) return pin->value;
        return Core::DataVariant();
    }

    void LogicNode::setPinValue(UUIDv4::UUID pinID, Core::DataVariant value) {
        auto pin = findPin(pinID);
        if(pin != nullptr) pin->value = value;
    }

    void LogicNode::setPinValue(const std::string& pinName, Core::DataVariant value) {
        auto pin = findPin(pinNameToID[pinName]);
        if(pin != nullptr) pin->value = value;
    }

    Core::LogicPin* LogicNode::findPin(UUIDv4::UUID pinID) {
        if(pins.contains(pinID)) return pins[pinID].get();
        return nullptr;
    }
        
    bool LogicNode::isFullInputPin() const {
        for(const auto& [pinID, pin] : pins) {
            if(pin->value.isEmpty() && pin->direction == Core::Direction::Input) return false;
        }
        return true;
    }

    bool LogicNode::isFullOutputPin() const {
        for(const auto& [pinID, pin] : pins) {
            if(pin->value.isEmpty() && pin->direction == Core::Direction::Output) return false;
        }
        return true;
    }

    void LogicNode::Reset() {
        state = State::Idle;
        for(auto& [pinID, pin] : pins) pin->value.clear();
    }

    json LogicNode::serializeNode() {
        json jsonLogicNode;

        jsonLogicNode["id"] = id.str();
        jsonLogicNode["type"] = getTypeName();
        jsonLogicNode["name"] = name;
        jsonLogicNode["state"] = state;

        jsonLogicNode["pins"] = json::object();
        for(const auto& [pinName, pinID] : pinNameToID) {
            jsonLogicNode["pins"][pinName] = pinID.str();
        }

        return jsonLogicNode;
    }

    void LogicNode::deserializeNode(json jsonLogicNode) {
        id = UUIDv4::UUID(jsonLogicNode["id"].get<std::string>());
        name = jsonLogicNode["name"];
        state = jsonLogicNode["state"];

        std::unordered_map<UUIDv4::UUID, std::unique_ptr<Core::LogicPin>> newPinsMap;

        for(auto& [pinName, oldIDStr] : jsonLogicNode["pins"].items()) {
            UUIDv4::UUID oldPinID(oldIDStr.get<std::string>());

            // Ищем свежесозданный пин по его имени
            if (pinNameToID.contains(pinName)) {
                UUIDv4::UUID currentWrongID = pinNameToID[pinName];

                if (pins.contains(currentWrongID)) {
                    // Забираем ownership у старого пина
                    auto pinPtr = std::move(pins[currentWrongID]);

                    // ВАЖНО: Добавь метод SetID в LogicPin, если его там нет!
                    pinPtr->SetID(oldPinID); 

                    // Вставляем пин по правильному (старому) ключу
                    newPinsMap[oldPinID] = std::move(pinPtr);
                    
                    // Обновляем мапу имен
                    pinNameToID[pinName] = oldPinID;
                }
            }
        }
        
        // Заменяем сломанную мапу на починенную
        pins = std::move(newPinsMap);
    }
}