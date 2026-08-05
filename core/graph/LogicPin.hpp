#pragma once
#include "SwarmIDE.hpp"
#include "core/graph/DataVariant.hpp"
#include "core/engine/GenerateIDManager.hpp"
#include <string>

namespace SwarmIDE::Core {

    /**
     * @enum Direction
     * @brief Направление передачи данных через логический пин.
     */
    enum class Direction {
        Input,                                    /** @brief < Входной порт: принимает данные от других узлов. */
        Output                                    /** @brief < Выходной порт: транслирует результаты вычислений наружу. */
    };

    /**
     * @enum DataType
     * @brief Допустимый тип данных, который пин может принимать или передавать.
     */
    enum class DataType {
        STRING,                                   /** @brief < Данные типа std::string. */
        INT,                                      /** @brief < Данные типа int. */
        FLOAT,                                    /** @brief < Данные типа float. */
        JSON,                                     /** @brief < Данные формата JSON. */
        VOID_PTR                                  /** @brief < Специфичный указатель на произвольные данные (void*). */
    };

    /**
     * @enum PinType
     * @brief Уникальные идентификаторы всех возможных логических пинов в системе.
     */
    enum class PinType {
        DataStream,                               /** @brief < Тип пина для входа данных. */
        Model,                                    /** @brief < Тип пина для подключения языковой модели. */
        Memory,                                   /** @brief < Тип пина для подключения контекста или памяти. */
        Tools,                                    /** @brief < Тип пина для подключения внешних инструментов (ToolCall). */
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

        /**
         * @brief Конструктор логического пина
         * 
         * @param type         Тип пина
         * @param dir          Направление пина
         * @param dT           Тип данных пина
         * @param nID          Индекс пина (Если в ноде, есть несколько пинов с одинаковым PinID)
         * @param maxConn      Максимальное кол-во соединений
         * @param v            Данные, которые храняться в пине
         */
        LogicPin(PinType type, Direction dir, DataType dT, UUIDv4::UUID nID, int maxConn, DataVariant v) {
            this->type = type;
            direction = dir;
            dataType = dT;
            nodeID = nID;
            maxConnections = maxConn;
            value = v;

            ID = GenerateIDManager::getInstance().GetNewID();
        }

        UUIDv4::UUID GetID() { return ID; }
        void SetID(UUIDv4::UUID ID) { this->ID = ID; }
        int GetCurrentConnections() { return currentConnections; }

        PinType type;                             /** @brief < Идентификатор пина. */
        Direction direction;                      /** @brief < Направление пина (Вход / Выход). */
        DataType dataType;                        /** @brief < Тип данных, который ожидает или передает пин. */
        UUIDv4::UUID nodeID;                      /** @brief < ID ноды */
        int maxConnections = 1;                   /** @brief < Максимальное количество одновременных связей (-1 для безлимита). */
        DataVariant value = Core::DataVariant();  /** @brief < Контейнер с текущим значением данных на пине. */
    private:
        friend class ConnectionManager;

        UUIDv4::UUID ID;                          /** @brief < ID пина */
        int currentConnections = 0;               /** @brief < Текущее кол-во одновременных связей */
    };
}