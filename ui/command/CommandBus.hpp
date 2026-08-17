#pragma once
#include "SwarmIDE.hpp"
#include "ui/command/Cmd.hpp"
#include "ui/command/CmdCreateConnection.hpp"
#include "ui/command/CmdDeleteAllConnectionForPin.hpp"
#include "ui/command/CmdMoveNode.hpp"
#include "ui/command/CmdNodeOperations.hpp"
#include "core/engine/LoggingManager.hpp"
#include <vector>
#include <memory>

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
         * @return CommandBus& Ссылка на синглтон менеджера комманд.
         */
        static CommandBus& getInstance();

        /**
         * @brief Выполняет команду и пушит её в undoStack
         * @details Очищает redoStack
         * 
         * @param[in] cmd Комманда для добавления и выполнения 
         */
        void PushAndExecute(std::unique_ptr<Cmd> cmd);

        /**
         * @brief 
         * @details Достаёт команду из redoStack,
         * вызывает Undo(), перекидывает в redoStack
         * 
         * @note Вызывается Ctrl + Z
         */
        void Undo();

        /**
         * @brief 
         * @details Достаёт команду из redoStack, 
         * вызывает Execute(), возвращает в undoStack
         * 
         * @note Вызвается Ctrl + Y
         */
        void Redo();
        
        /**
         * @brief Полностью очищает все списки команд
         * @details Очищает undoStack и redoStack
         * @note Пока не ясно где данный метод будет использоваться
         */
        void Clear();

        // Запрещаем дублирование синглтона менеджера Комманд
        CommandBus(const CommandBus&) = delete;
        CommandBus& operator=(const CommandBus&) = delete;
        CommandBus(CommandBus&&) = delete;
        CommandBus& operator=(CommandBus&&) = delete;
    private:

        CommandBus() { Core::LoggingManager::Info("[CANVAS_MANAGER]: Constructor"); }
        ~CommandBus() { Core::LoggingManager::Info("[CANVAS_MANAGER]: Destructor. undoStack: ", undoStack.size());  }

        std::vector<std::unique_ptr<Cmd>> undoStack;    /** @brief < Список комманд */
        std::vector<std::unique_ptr<Cmd>> redoStack;    /** @brief < Список для отменённых команд */

        const int MAX_HISTORY_SIZE = 20;                /** @brief < Лимит истории для команд */
    };

}