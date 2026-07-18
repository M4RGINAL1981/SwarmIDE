#include "SwarmIDE.hpp"
#include "nodes/base/LogicNode.hpp"
#include "core/graph/LogicPin.hpp"
#include "core/graph/DataVariant.hpp"

namespace SwarmIDE::Nodes {

    /* Пример структуры Логического пина
    struct LogicPin {
        Хранит логическое состояние порта
    
        std::string id;                    // Уникальное имя пина
        Direction direction;               // Направление пина (Вход/Выход)
        std::string typeName;              // Идентификатор типа данных, которые пин согласен принять
        DataVariant value;                 // Контейнер, в котором лежат данные 
    };
    */

    // Добавить новый входной пин для ноды
    void LogicNode::addInputPin(std::string id, std::string type) {
        inputs.push_back(std::make_unique<Core::LogicPin>(Core::LogicPin{id, Core::Direction::Input, type, Core::DataVariant()}));
    }

    // Добавить новый выходной пин для ноды
    void LogicNode::addOutputPin(std::string id, std::string type) {
        outputs.push_back(std::make_unique<Core::LogicPin>(Core::LogicPin{id, Core::Direction::Output, type, Core::DataVariant()}));
    }
    
    
    // Считывает значение логического пина ноды (Вход)
    Core::DataVariant LogicNode::getInputPinValue(std::string id) {
        for(auto& Ipin : inputs) {
            if(Ipin->id == id) return Ipin->value;
        }
        return Core::DataVariant();
    }

    // Выдаёт значение логического пина ноды (Выход)
    Core::DataVariant LogicNode::getOutputPinValue(std::string id) {
        for(auto& Opin : outputs) {
            if(Opin->id == id) return Opin->value;
        }
        return Core::DataVariant();
    }

    // Устанавливает значение логического пина ноды (Вход)
    void LogicNode::setInputPinValue(std::string id, Core::DataVariant value) {
        for(auto& Ipin : inputs) {
            if(Ipin->id == id) {
                Ipin->value = value;
                return;
            }
        }
    }

    // Устанавливает значение логического пина ноды (Выход)           
    void LogicNode::setOutputPinValue(std::string id, Core::DataVariant value) {
        for(auto& Opin : outputs) {
            if(Opin->id == id) {
                Opin->value = value;
                return;
            }
        }
    }

    // Выдаёт входной логически пин по ноды ID
    Core::LogicPin* LogicNode::getLogicInputPinByID(std::string id) {
        for(auto& Ipin : inputs) {
            if(Ipin->id == id) return Ipin.get();
        }
        return nullptr;
    }

    // Выдаёт выходной логический пин ноды по ID
    Core::LogicPin* LogicNode::getLogicOutputPinByID(std::string id) {
        for(auto& Opin : outputs) {
            if(Opin->id == id) return Opin.get();
        }
        return nullptr;
    }
        
    bool LogicNode::isFullInputPin() const {
        for(const auto& Ipin : inputs) {
            if(Ipin->value.isEmpty()) return false;
        }
        return true;
    }

    bool LogicNode::isFullOutputPin() const {
        for(const auto& Opin : outputs) {
            if(Opin->value.isEmpty()) return false;
        }
        return true;
    }

    void LogicNode::Reset() {
        state = State::Idle;
        for(auto& Ipin : inputs) Ipin->value.clear();
        for(auto& Opin : outputs) Opin->value.clear();
    }
}