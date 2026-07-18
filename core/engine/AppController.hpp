#pragma once
#include "SwarmIDE.hpp"
#include "core/graph/GraphManager.hpp"
#include "core/graph/GraphRegistry.hpp"
#include "core/graph/ConnectionManager.hpp"
#include "core/engine/SerializationManager.hpp"
#include "core/models/ModelManager.hpp"
#include "graphics/Camera.hpp"
#include "graphics/Grid.hpp"

namespace SwarmIDE::Core {

    /**
     * @class AppController
     * @brief Главный управляющий контроллер (оркестратор) жизненного цикла SwarmIDE.
     * @details Этот класс инициализирует графический контекст OpenGL, окно GLFW и интерфейс ImGui. 
     * Он собирает в себе ссылки на все ключевые менеджеры ядра и координирует их совместную работу, 
     * обеспечивая связь между вычислительной логикой (Core) и визуальным слоем (UI/Graphics).
     */
    class AppController {
    public:
        /**
         * @brief Конструктор контроллера приложения.
         * @details Выполняет базовую инициализацию: запускает GLFW, настраивает параметры окна, 
         * инициализирует расширения OpenGL (через GLAD) и связывает менеджеры между собой 
         * (например, прокидывает указатель в `ConnectionManager::SetGraphManager()`).
         */
        AppController();
        
        /**
         * @brief Запуск главного цикла приложения (Main Render Loop).
         * @details Удерживает поток выполнения до закрытия окна. Внутри цикла происходит:
         * - Опрос системных событий ввода (мышь, клавиатура).
         * - Расчет трансформаций камеры #camera и сетки #grid.
         * - Отрисовка интерфейса ImGui и холста графа.
         * - Обновление состояний асинхронных вычислительных потоков.
         */
        void run();

    private:
        GLFWwindow* window;                              /**< Системный указатель на окно программы, управляемое библиотекой GLFW. */
        Graphics::Camera camera;                         /**< Объект 2D-камеры для перемещения и масштабирования холста. */
        Graphics::Grid grid;                             /**< Графическая бесконечная сетка, рисуемая на фоне холста в OpenGL. */

        // Ссылки на менеджеры ядра (Core Managers)
        GraphManager& graph_manager;                     /**< Ссылка на менеджер жизненного цикла и обхода узлов. */
        ConnectionManager& connect_manager;              /**< Ссылка на менеджер логических связей (ребер графа). */
        SerializationManager& serialization_manager;     /**< Ссылка на менеджер сохранения и загрузки проектов в JSON. */
        ModelManager& model_manager;                     /**< Ссылка на менеджер локальных LLM-моделей (llama.cpp) для ИИ-агентов. */
        GenerateIDManager& generate_id_manager;          /**< Ссылка на потокобезопасный генератор UUIDv4. */
        GraphRegistry& graph_registry;                   /**< Ссылка на фабрику объектов для динамического создания нод. */
        
        //SwarmIDE::UI::Fonts fonts;

        glm::vec2 lastMousePos = glm::vec2(0.0f);        /**< Координаты последнего зарегистрированного положения курсора мыши (используется для панорамирования). */
        bool isPanning = false;                          /**< Флаг, указывающий, зажата ли кнопка мыши для перемещения (скролла) по холсту. */
    };
}
