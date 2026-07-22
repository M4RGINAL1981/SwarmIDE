#pragma once
#include "SwarmIDE.hpp"
#include "core/graph/DataVariant.hpp"
#include <string>

namespace SwarmIDE::Core {

    /**
     * @enum Direction
     * @brief Направление передачи данных через логический пин.
     */
    enum class Direction {
        Input,                                    /**< @brief Входной порт: принимает данные от других узлов. */
        Output                                    /**< @brief Выходной порт: транслирует результаты вычислений наружу. */
    };

    /**
     * @enum DataType
     * @brief Допустимый тип данных, который пин может принимать или передавать.
     */
    enum class DataType {
        STRING,                                   /**< @brief Данные типа std::string. */
        INT,                                      /**< @brief Данные типа int. */
        FLOAT,                                    /**< @brief Данные типа float. */
        JSON,                                     /**< @brief Данные формата JSON. */
        VOID_PTR                                  /**< @brief Специфичный указатель на произвольные данные (void*). */
    };

    /**
     * @enum PinID
     * @brief Уникальные идентификаторы всех возможных логических пинов в системе.
     */
    enum class PinID {
        DataStream,                               /**< @brief ID пина для входа данных. */
        Model,                                    /**< @brief ID пина для подключения языковой модели. */
        Memory,                                   /**< @brief ID пина для подключения контекста или памяти. */
        Tools,                                    /**< @brief ID пина для подключения внешних инструментов (ToolCall). */
    };

    /**
     * @struct LogicPin
     * @brief Структурная единица логического порта (пина) узла.
     * @details Хранит конфигурацию пина, ограничения по связям, тип данных 
     * и текущее значение, протекающее через порт.
     * 
     * @see SwarmIDE::Nodes::LogicNode
     * @see SwarmIDE::Core::DataVariant
     */
    struct LogicPin {

        LogicPin(PinID i, Direction dir, DataType dT, int ind, int maxConn, DataVariant v) {
            id = i;
            direction = dir;
            dataType = dT;
            index = ind;
            maxConnections = maxConn;
            value = v;
        }

        PinID id;                                 /**< @brief Идентификатор пина. */
        Direction direction;                      /**< @brief Направление пина (Вход / Выход). */
        DataType dataType;                        /**< @brief Тип данных, который ожидает или передает пин. */
        int index = 0;                            /**< @brief Порядковый номер (для нод с несколькими одинаковыми пинами) */
        int maxConnections = 1;                   /**< @brief Максимальное количество одновременных связей (-1 для безлимита). */
        DataVariant value = Core::DataVariant();  /**< @brief Контейнер с текущим значением данных на пине. */
    private:
        friend ConnectionManager;

        int currentConnections = 0;               /**< @brief Текущее кол-во одновременных связей */
    };
}