#pragma once
#include "SwarmIDE.hpp"
#include "core/graph/GraphManager.hpp"
#include "core/graph/ConnectionManager.hpp"
#include "core/engine/SerializationManager.hpp"
#include "core/models/ModelManager.hpp"
#include "nodes/base/LogicNode.hpp"
#include "graphics/Camera.hpp"
#include "graphics/Grid.hpp"

namespace SwarmIDE::UI {
    
    /**
     * @brief Меню нодовой программы
     */
    class Menu {
    public:
        /**
         * @brief Глобальная точка доступа к единственному экземпляру Menu.
         * @return Menu& Ссылка на синглтон меню.
         */
        static Menu& getInstance(); 


        /**
         * @brief Отрисовка меню
         */
        void DrawMenu();                                                

        /**
         * @brief Отрисовка верхней полосы меню
         * @param window Окно, для отрисовки
         */
        void DrawTopBar(GLFWwindow* window);           

        //void DrawDownloadModel();                                                                   // Отрисовка загрузки моделей
        //void DrawProgressBar(float progress, ImVec2 size);                                          // Отрисовка полосы загрузки

        /**
         * @brief Установить новый размер окна
         * @param otherWinSize Новый размер окна
         */
        void GetWinSize(glm::vec2 otherWinSize) { winSize = otherWinSize; }           
        
        /**
         * @brief Уставновить сетку
         * @param grid 
         */
        void SetGrid(Graphics::Grid* grid) { this->grid = grid; }                                     // Установка сетки
        /**
         * @brief Установить камеру
         * @param camera 
         */
        void SetCamera(Graphics::Camera* camera) { this->camera = camera; }                           // Установки камеры

        // Запрещаем дублирование синглтона меню
        Menu (const Menu&) = delete;
        Menu& operator=(const Menu&) = delete; 
        Menu (Menu&&) = delete;
        Menu& operator=(Menu&&) = delete;
    private:
        Menu() = default;
        ~Menu() = default;

        Graphics::Grid* grid = nullptr;
        Graphics::Camera* camera = nullptr;

        glm::vec2 winSize;
    };
}
