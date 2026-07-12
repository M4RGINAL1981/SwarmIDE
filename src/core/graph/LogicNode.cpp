#include "core/graph/LogicNode.hpp"
#include "core/graph/LogicPin.hpp"
#include "core/graph/DataVariant.hpp"

namespace SwarmIDE::Core::Graph {

    /*
    struct LogicPin {
        Хранит логическое состояние порта
    
        std::string id;                    // Уникальное имя пина
        Direction direction;               // Направление пина (Вход/Выход)
        std::string typeName;              // Идентификатор типа данных, которые пин согласен принять
        DataVariant value;                 // Контейнер, в котором лежат данные 
    };
 
    */

    void LogicNode::addInputPin(std::string id, std::string type) {
        inputs.push_back(std::make_unique<LogicPin>(LogicPin{id, Direction::Input, type, DataVariant()}));
    }

    void LogicNode::addOutputPin(std::string id, std::string type) {
        outputs.push_back(std::make_unique<LogicPin>(LogicPin{id, Direction::Output, type, DataVariant()}));
    }
    
    
    // Считывает значение логического пина (Вход)
    DataVariant LogicNode::getInputPinValue(std::string id) {
        for(auto& Ipin : inputs) {
            if(Ipin->id == id) return Ipin->value;
        }
        return DataVariant();
    }

    DataVariant LogicNode::getOutputPinValue(std::string id) {
        for(auto& Opin : outputs) {
            if(Opin->id == id) return Opin->value;
        }
    }

    // Устанавливает значение логического пина (Вход)
    void LogicNode::setInputPinValue(std::string id, DataVariant value) {
        for(auto& Ipin : inputs) {
            if(Ipin->id == id) {
                Ipin->value = value;
                return;
            }
        }
    }

    // Устанавливает значение логического пина (Выход)           
    void LogicNode::setOutputPinValue(std::string id, DataVariant value) {
        for(auto& Opin : outputs) {
            if(Opin->id == id) {
                Opin->value = value;
                return;
            }
        }
    }

    LogicPin* LogicNode::getLogicInputPinByID(std::string id) {
        for(auto& Ipin : inputs) {
            if(Ipin->id == id) return Ipin.get();
        }
        return nullptr;
    }

    LogicPin* LogicNode::getLogicOutputPinByID(std::string id) {
        for(auto& Opin : outputs) {
            if(Opin->id == id) return Opin.get();
        }
        return nullptr;
    }
        
}