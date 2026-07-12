#pragma once
#include "core/graph/DataVariant.hpp"
#include <string>

namespace SwarmIDE::Core::Graph {

    enum Direction {
    /*
        Константа указывающая направление пина
    */
        Input,
        Output,
    };

    struct LogicPin {
    /*
        Хранит логическое состояние порта
    */
        std::string id;                    // Уникальное имя пина
        Direction direction;               // Направление пина (Вход/Выход)
        std::string typeName;              // Идентификатор типа данных, которые пин согласен принять
        DataVariant value;                 // Контейнер, в котором лежат данные 
    };
}