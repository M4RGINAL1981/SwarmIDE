#pragma once 
#include "SwarmIDE.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "graphics/Grid.hpp"
#include "graphics/Camera.hpp"

namespace SwarmIDE::UI {

    /**
     * @class TopBar
     * @brief Менеджер для отрисовки верхей полоски меню и отлавливания хот-кеев
     */
    class TopBar {
    public:
        /**
         * @brief Точка доступа к единственному экземпляру класса TopBar
         * @return TopBar& 
         */
        static TopBar& getInstance();

        /**
         * @brief Отрисовка верхней панели.
         */
        void Draw(GLFWwindow* window, Graphics::Camera* camera, Graphics::Grid* gridk);

        // Запрещаем копирование и перемещение менеджера
        TopBar(const TopBar&) = delete;
        TopBar& operator=(const TopBar&) = delete;
        TopBar(TopBar&&) = delete;
        TopBar& operator=(TopBar&&) = delete;
    private:
        TopBar() = default;
        ~TopBar() = default;

        int lastXPos = 150, lastYPos = 150; 
        int lastWidth = 1920, lastHeight = 1080;
    };
}