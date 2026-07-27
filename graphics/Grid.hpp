#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "graphics/Camera.hpp"
#include "ui/windows/ThemeManager.hpp"
#include "core/engine/LoggingManager.hpp"
#include "core/engine/LoggingManager.hpp"

namespace SwarmIDE::Graphics {

    /**
     * @class Grid
     * @brief Сетка на фоне программы
     */
    class Grid {
    public:

        /**
         * @brief Конструктор сетки
         * 
         * @param[in] color Цвет сетки
         * @param[in] step Шаг сетки
         * @param[in] thickness толщина сетки
         */
        Grid(glm::vec3 color, float step, float thickness) : color(color), step(step), lineThickness(thickness) {};
        
        /**
         * @brief Конструктор сетки
         */
        Grid();

        /**
         * @brief Деструктор сетки
         */
        ~Grid() { Core::LoggingManager::Info("[GRID]: Destructor"); }

        /**
         * @brief Иницилизация сетки
         */
        void init();

        /**
         * @brief Отрисовка сетки
         * @param camera Камера, чтобы знать шаг сетки и другие параметры
         */
        void draw(Camera& camera);
                
        /**
         * @brief Установить размер окна
         * @param width Ширина окна
         * @param height Высота окна
         */
        void SetWindowSize(int width, int height)  { windowWidth = width; windowHeight = height; }

        /**
         * @brief Меняет переменную видимости на противоположную
         */
        void toggleVisibility() { isVisible = !isVisible; }
        
        /**
         * @brief // Устанавливает переменную видимости в заданное значение
         * @param[in] visible 
         */
        void setVisibility(bool visible) { isVisible = visible; }  

    private:
    
        unsigned int shaderProgram = 0;

        // Переменные состояния сестки
        float step = 100.0f;            /** @brief < Шаг сетки */
        float lineThickness = 5.0;      /** @brief < Толшина сетки */
        glm::vec3 color;                /** @brief < Цвет сетки */

        int windowWidth = 3840;         /** @brief < Ширина окна */
        int windowHeight = 2160;        /** @brief < Высота окна */
        bool isVisible = true;          /** @brief < Видимость сетки */

        // ID объектов OpenGL
        unsigned int VAO = 0;
        unsigned int VBO = 0;

        // Вершины одной линии
        std::vector<glm::vec2> lineVertices;
    };
}
