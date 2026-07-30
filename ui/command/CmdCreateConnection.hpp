#pragma once
#include "SwarmIDE.hpp"
#include "ui/command/Cmd.hpp"
#include "core/graph/ConnectionManager.hpp"

namespace SwarmIDE::UI::Command {

    /**
     * @class CmdCreateConnection
     * @brief Команда для создания связи
     * 
     * @see SwarmIDE::Core::ConnectionManager
     * @see SwarmIDE::Core::Connection
     */
    class CmdCreateConnection : public Cmd {
    public:

        /**
         * @brief Конструктор команды, для создания нового соединения
         * @details Иницилизирует структуру conn.
         * 
         * @param[in] fromNodeID   ID ноды-источника
         * @param[in] fromPinID    ID пина ноды-источника
         * @param[in] fromIndex    Индекс пина ноды-источника
         * @param[in] toNodeID     ID ноды-цели
         * @param[in] toPinID      ID пина ноды-цели
         * @param[in] toIndex      Индекс пина ноды-цели
         */
        CmdCreateConnection(UUIDv4::UUID fromNodeID, Core::PinID fromPinID, int fromIndex,
                            UUIDv4::UUID toNodeID, Core::PinID toPinID, int toIndex);

        /**
         * @brief Создаёт связь между нодами
         */
        virtual void Execute() override;

        /**
         * @brief Удаляет связь между нодами
         */
        virtual void Undo() override;

    private:

        UUIDv4::UUID fromNodeID;       /** @brief < ID ноды-источника */
        Core::PinID fromPinID;         /** @brief < ID пина ноды-источника */
        int fromIndex;                 /** @brief < Индекс пина ноды-источника */

        UUIDv4::UUID toNodeID;         /** @brief < ID ноды-цели */
        Core::PinID toPinID;           /** @brief < ID пина ноды-цели */
        int toIndex;                   /** @brief < Индекс пина ноды-цели */
    };

}