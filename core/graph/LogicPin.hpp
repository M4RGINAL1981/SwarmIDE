#pragma once
#include "SwarmIDE.hpp"
#include "core/graph/DataVariant.hpp"
#include <string>

namespace SwarmIDE::Core {

    /**
     * @enum Direction
     * @brief Направление передачи данных через логический пин.
     */
    enum Direction {
        Input,  /**< Входной порт: принимает данные от других узлов для вычислений. */
        Output  /**< Выходной порт: транслирует результаты вычислений узла наружу. */
    };

    /**
     * @struct LogicPin
     * @brief Структурная единица логического порта (пина) узла.
     * @details Хранит состояние соединения, тип данных и само значение, протекающее 
     * через этот порт. Служит связующим мостом между вычислительным ядром (Core) 
     * 
     * @see SwarmIDE::Nodes::LogicNode
     * @see SwarmIDE::Core::DataVariant
     */
    struct LogicPin {
        std::string id;        /**< Уникальное имя пина в рамках одной ноды (например, "input_text", "model_core"). */
        Direction direction;   /**< Направление пина (Вход / Выход). */
        std::string typeName;  /**< Строковый идентификатор типа данных, который пин готов принять или отдать (например, "string", "int", "float"). */
        DataVariant value;     /**< Универсальный контейнер-вариант, хранящий фактическое текущее значение пина. */
    };
}