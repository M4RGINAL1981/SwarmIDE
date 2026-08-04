#include "SwarmIDE.hpp"
#include "nodes/base/LogicNode.hpp"
#include "core/graph/LogicPin.hpp"
#include "core/graph/DataVariant.hpp"

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
}