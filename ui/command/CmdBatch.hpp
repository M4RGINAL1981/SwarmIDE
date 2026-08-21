#pragma once 
#include "SwarmIDE.hpp"
#include "ui/command/Cmd.hpp"

#include <vector>   
#include <memory>


namespace SwarmIDE::UI::Command {

    /**
     * @class CmdBatch
     * @brief Макро-команда для групового выполения операций (груповое перемещение, удаление и т.д.)
     */
    class CmdBatch : public Cmd {
    public:
        CmdBatch() = default;
        ~CmdBatch() override = default;

        /**
         * @brief Добавить команду в батч. 
         */
        void AddCommand(std::unique_ptr<Cmd> cmd) { if(cmd) commands.push_back(std::move(cmd)); }
        
        /** 
         * @brief Проверка, есть ли комманды в батче.
         */
        bool IsEmpty() const { return commands.empty(); }

        /**
         * @brief Количество команд в батче.
         * @return size_t Кол-во команд. 
         */
        size_t Size() const { return commands.size(); }

        /**
         * @brief Выполнить все команды в батче.
         */
        virtual void Execute() override { for (auto& cmd : commands) { cmd->Execute(); } }

        /**
         * @brief Отменить все команды в батче (В обратном порядке).
         */
        virtual void Undo() override { for (auto it = commands.rbegin(); it != commands.rend(); ++it) { (*it)->Undo(); } }

    private:
        std::vector<std::unique_ptr<Cmd>> commands;       
    };

} // namespace SwarmIDE::UI::Command