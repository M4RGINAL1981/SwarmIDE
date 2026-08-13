#pragma once 
#include "SwarmIDE.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace SwarmIDE::UI::Command {

    /**
     * @class ShortcutManager
     * @brief Класс для отлавливания горячих клавиш
     */
    class ShortcutManager {
    public:

        /**
         * @brief Отлавливание горячих клавиш
         * @param[in] window Текущее окно в котором нужно отлавливать горячие клавиши 
         */
        static void Process(GLFWwindow* window);
    };

}