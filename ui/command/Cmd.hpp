#pragma once
#include "SwarmIDE.hpp"

namespace SwarmIDE::UI::Command {

    /**
     * @class Cmd
     * @brief Контракт для любой обратимой операции в UI.
     * 
     * @todo Какие команды мне нужно будет создать в будущем: Групповое перемещение нод, Дублирование/ Копирование и вставка, 
     */
    class Cmd {
    public:

        /**
         * @brief Создать объект Cmd
         */
        Cmd() = default;

        /**
         * @brief Уничтожить объект Cmd
         */
        virtual ~Cmd() = default;

        /**
         * @brief Выполнить действие (например, изменить значение поля, создать Link).
         */
        virtual void Execute() = 0;

        /**
         * @brief Откатить действие назад.
         * 
         */
        virtual void Undo() = 0;
    };

}