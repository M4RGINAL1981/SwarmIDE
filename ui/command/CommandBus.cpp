#include "SwarmIDE.hpp"
#include "ui/command/CommandBus.hpp"
#include "core/engine/LoggingManager.hpp"

namespace SwarmIDE::UI::Command {

    CommandBus& CommandBus::getInstance() {
        static CommandBus instance;
        return instance;
    }
    
    void CommandBus::PushAndExecute(std::unique_ptr<Cmd> cmd) {
        Core::LoggingManager::Fatal("[PUSH_AND_EXECUTE]: Count undo: ", undoStack.size());
        if(!cmd) return;

        // Выполняем команду
        cmd->Execute();

        // Добавляем в вектор списка команд
        undoStack.push_back(std::move(cmd));

        // Очищаем список отменённых команд
        redoStack.clear();

        // Следим за переполненостью вектора.
        if(undoStack.size() > MAX_HISTORY_SIZE) {
            undoStack.erase(undoStack.begin());
        } 
    }

    void CommandBus::Undo() {
        // Если отменять нечего - выходим
        if(undoStack.empty()) return;

        std::unique_ptr<Cmd> command = std::move(undoStack.back());
        undoStack.pop_back();

        command->Undo();

        redoStack.push_back(std::move(command));
    }

    void CommandBus::Redo() {
        // Если возвращать нечего - выходим
        if(redoStack.empty()) return;
        
        std::unique_ptr<Cmd> command = std::move(redoStack.back());
        redoStack.pop_back();

        command->Execute();

        undoStack.push_back(std::move(command));
    }

    void CommandBus::Clear() {
        undoStack.clear();
        redoStack.clear();
    }

}