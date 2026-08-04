#pragma once
#include "SwarmIDE.hpp"
#include "ui/command/Cmd.hpp"
#include "core/graph/ConnectionManager.hpp"
#include <optional>

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
         * @brief Конструктор команды создания связи
         * 
         * @param fromNodeID  ID ноды-источника
         * @param fromPinID   ID пина ноды-источника
         * @param toNodeID    ID ноды-цели
         * @param toPinID     ID пина ноды-цели
         */
        CmdCreateConnection(UUIDv4::UUID fromNodeID, UUIDv4::UUID fromPinID, UUIDv4::UUID toNodeID, UUIDv4::UUID toPinID);

        /**
         * @brief Создаёт связь между нодами
         */
        virtual void Execute() override;

        /**
         * @brief Удаляет связь между нодами
         */
        virtual void Undo() override;

    private:
        bool m_isFirstExecution = true;                                               /** @brief < Первый ли это запуск */

        UUIDv4::UUID connectionID;                                                    /** @brief < ID соединения */
        UUIDv4::UUID fromNodeID;                                                      /** @brief < ID ноды-источника */
        UUIDv4::UUID fromPinID;                                                       /** @brief < ID пина ноды-источника */
        UUIDv4::UUID toNodeID;                                                        /** @brief < ID ноды-цели */
        UUIDv4::UUID toPinID;                                                         /** @brief < ID пина ноды-цели */

        std::optional<Core::LogicConnection>  cachedLogicConnection = std::nullopt;   /** @brief < Кэш для логического-подключения */
        std::unique_ptr<UI::ViewConnection> cachedViewConnection = nullptr;           /** @brief < Кэш для визуального-подключения */
    };

}