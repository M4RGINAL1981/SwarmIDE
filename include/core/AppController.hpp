#pragma once
#include "core/graph/GraphManager.hpp"
#include "core/graph/GraphRegistry.hpp"
#include "core/ConnectionManager.hpp"
#include "core/SerializationManager.hpp"
#include "core/ModelManager.hpp"
#include "graphics/Camera.hpp"
#include "graphics/Grid.hpp"
#include "SwarmIDE.hpp"

namespace SwarmIDE::Core {

    class AppController {
    public:
        AppController();                                 // Инициализация библиотек, установка связи между менеджерами и модулями
        
        void run();                                      // Запуск игрового цикла и отрисовка приложения

    private:
        GLFWwindow* window;                              // Окно программы 
        Camera camera;                                   // Камера
        Grid grid;                                       // Сетка

        // Менеджеры
        Graph::GraphManager& graph_manager;                     // Менеджер нод
        ConnectionManager& connect_manager;              // Менеджер соединений
        SerializationManager& serialization_manager;     // Менеджер сохранения и загруки
        ModelManager& model_manager;                     // Менеджер загрузки моделей для агентов
        GenerateIDManager& generate_id_manager;          // Менеджер по генерации уникальных id
        Graph::GraphRegistry& graph_registry;                   // Регистрация реализаций нод в графе
        
        SwarmIDE::UI::Fonts fonts;                       // Шрифты

        glm::vec2 lastMousePos = glm::vec2(0.0f);        // Последнее положение мыши в пространстве
        bool isPanning = false;                          
    };
}