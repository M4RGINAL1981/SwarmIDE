#include "SwarmIDE.hpp"
#include "nodes/base/LogicNode.hpp"
#include "core/graph/LogicPin.hpp"
#include "core/graph/DataVariant.hpp"

namespace SwarmIDE::Nodes {


    void LogicNode::addPin(Core::PinID id, Core::Direction dir, Core::DataType dataType, int index, int maxConn, Core::DataVariant value) {
        pins.push_back(std::make_unique<Core::LogicPin>(Core::LogicPin(id, dir, dataType, index, maxConn, value)));
    }

    Core::DataVariant LogicNode::getPinValue(Core::PinID id, Core::Direction dir, int index) {
        Core::LogicPin* pin = findPin(id, dir, index);
        if(pin != nullptr) return pin->value;
        return Core::DataVariant();
    }

    void LogicNode::setPinValue(Core::DataVariant value, Core::PinID id, Core::Direction dir, int index) {
        Core::LogicPin* pin = findPin(id, dir, index);
        if(pin != nullptr) pin->value = value;
    }

    Core::LogicPin* LogicNode::findPin(Core::PinID id, Core::Direction dir, int index) {
        for(auto& pin : pins) {
            if(pin->id == id && pin->direction == dir && pin->index == index) return pin.get();
        }
        return nullptr;
    }

        
    bool LogicNode::isFullInputPin() const {
        for(const auto& pin : pins) {
            if(pin->value.isEmpty() && pin->direction == Core::Direction::Input) return false;
        }
        return true;
    }

    bool LogicNode::isFullOutputPin() const {
        for(const auto& pin : pins) {
            if(pin->value.isEmpty() && pin->direction == Core::Direction::Output) return false;
        }
        return true;
    }

    void LogicNode::Reset() {
        state = State::Idle;
        for(auto& pin : pins) pin->value.clear();
    }
}