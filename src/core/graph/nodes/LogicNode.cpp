#include "core/graph/nodes/LogicNode.hpp"
#include "core/graph/LogicPin.hpp"
#include "core/graph/DataVariant.hpp"

namespace SwarmIDE::Core::Graph {

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
        inputs.push_back(std::make_unique<LogicPin>(LogicPin{id, Direction::Input, type, DataVariant()}));
    }

    // Добавить новый выходной пин для ноды
    void LogicNode::addOutputPin(std::string id, std::string type) {
        outputs.push_back(std::make_unique<LogicPin>(LogicPin{id, Direction::Output, type, DataVariant()}));
    }
    
    
    // Считывает значение логического пина ноды (Вход)
    DataVariant LogicNode::getInputPinValue(std::string id) {
        for(auto& Ipin : inputs) {
            if(Ipin->id == id) return Ipin->value;
        }
        return DataVariant();
    }

    // Выдаёт значение логического пина ноды (Выход)
    DataVariant LogicNode::getOutputPinValue(std::string id) {
        for(auto& Opin : outputs) {
            if(Opin->id == id) return Opin->value;
        }
        return DataVariant();
    }

    // Устанавливает значение логического пина ноды (Вход)
    void LogicNode::setInputPinValue(std::string id, DataVariant value) {
        for(auto& Ipin : inputs) {
            if(Ipin->id == id) {
                Ipin->value = value;
                return;
            }
        }
    }

    // Устанавливает значение логического пина ноды (Выход)           
    void LogicNode::setOutputPinValue(std::string id, DataVariant value) {
        for(auto& Opin : outputs) {
            if(Opin->id == id) {
                Opin->value = value;
                return;
            }
        }
    }

    // Выдаёт входной логически пин по ноды ID
    LogicPin* LogicNode::getLogicInputPinByID(std::string id) {
        for(auto& Ipin : inputs) {
            if(Ipin->id == id) return Ipin.get();
        }
        return nullptr;
    }

    // Выдаёт выходной логический пин ноды по ID
    LogicPin* LogicNode::getLogicOutputPinByID(std::string id) {
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