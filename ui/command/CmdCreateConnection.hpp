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
         * @param[in] conn Указатель на соединение
         */
        CmdCreateConnection(Core::Connection* conn) : conn(conn) {};

        /**
         * @brief Создаёт связь между нодами
         */
        virtual void Execute() override;

        /**
         * @brief Удаляет связь между нодами
         */
        virtual void Undo() override;

    private:

        Core::Connection* conn = nullptr;     /** @brief < Информация о соединении */
    };

}