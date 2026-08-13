#pragma once 
#include "SwarmIDE.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace SwarmIDE::UI {

    /**
     * @class WindowEventHandler
     * @brief Класс для подключения всех колбэков
     */
    class WindowEventHandler {
    public:

        /** 
         * @brief 
         * @param[in] w          Текущее окно
         * @param[in] button     Текущая клавиша мыши
         * @param[in] action  
         * @param[in] mods
         */
        static void MouseButtonCallback(GLFWwindow* w, int button, int action, int mods);

        /**
         * @brief 
         * 
         * @param[in] w          Текущее окно
         * @param[in] xpos       Положение курсора по x
         * @param[in] ypos       Положение курсора по y
         */
        static void CursorPosCallback(GLFWwindow* w, double xpos, double ypos);

        /**
         * @brief 
         * 
         * @param[in] w          Текущее окно 
         * @param[in] key         
         * @param[in] scancode 
         * @param[in] action 
         * @param[in] mods 
         */
        static void KeyCallback(GLFWwindow* w, int key, int scancode, int action, int mods);

        /**
         * @brief 
         * 
         * @param[in] w          Текущее окно 
         * @param[in] xoffset    
         * @param[in] yoffset 
         */
        static void ScrollCallback(GLFWwindow* w, double xoffset, double yoffset);

        /**
         * @brief 
         * 
         * @param[in] w          Текущее окно 
         * @param[in] width 
         * @param[in] height 
         */
        static void FramebufferSizeCallback(GLFWwindow* w, int width, int height);
    };
}