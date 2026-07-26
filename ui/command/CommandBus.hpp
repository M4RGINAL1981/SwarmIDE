#pragma once
#include "SwarmIDE.hpp"
#include "ui/command/Cmd.hpp"
#include <vector>

namespace SwarmIDE::UI::Command {

    /**
     * @class CommandBus
     * @brief Менедржер-синглтон для компонет Command
     * 
     * @see SwarmIDE::UI::Command
     */
    class CommandBus {
    public:

         /**
         * @brief Глобальная точка доступа к единственному экземпляру CommandBus.
         * @return CommandBus& Ссылка на синглтон менеджера соединений.
         */
        static CommandBus& getInstance();

        /**
         * @brief Выполняет команду и пушит её в undoStack
         * @details Очищает redoStack
         * 
         * @param[in] cmd Комманда для добавления и выполнения 
         */
        void PushAndExecute(Cmd* cmd);

        /**
         * @brief 
         * @details Достаёт команду из redoStack,
         * вызывает Undo(), перекидывает в redoStack
         */
        void Undo();

        /**
         * @brief 
         * @details Достаёт команду из redoStack, 
         * вызывает Execute(), возвращает в undoStack
         * 
         */
        void Redo();

        /**
         * @brief Проверяет нажатия клавиш каждую итерацию кадра
         */
        void ProcessKeybindings();

        /**
         * @brief Полностью очищает все списки команд
         * @details Очищает undoStack и redoStack
         */
        void Clear();

        // Запрещаем дублирование синглтона менеджера связей
        CommandBus(const CommandBus&) = delete;
        CommandBus& operator=(const CommandBus&) = delete;
        CommandBus(CommandBus&&) = delete;
        CommandBus& operator=(CommandBus&&) = delete;
    private:

        CommandBus() = default;
        ~CommandBus() = default;

        std::vector<std::unique_ptr<Cmd>> undoStack;  /** @brief < Список комманд */
        std::vector<std::unique_ptr<Cmd>> redoStack;  /** @brief < Список для отменённых команд */

        const int MAX_HISTORY_SIZE = 100;             /** @brief < Лимит истории для команд */
    };

}