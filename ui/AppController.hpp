#pragma once
#include "SwarmIDE.hpp"
#include "ui/canvas/CanvasManager.hpp"
#include "ui/canvas/CanvasInputController.hpp"

namespace SwarmIDE::UI {

    /**
     * @class AppController
     * @brief Главный управляющий контроллер (оркестратор) жизненного цикла SwarmIDE.
     * @details Этот класс инициализирует графический контекст OpenGL, окно GLFW и интерфейс ImGui. 
     * Он собирает в себе ссылки на все ключевые менеджеры ядра и координирует их совместную работу, 
     * обеспечивая связь между вычислительной логикой (Core) и визуальным слоем (UI/Graphics).
     * 
     * @todo Нужно будет вынести все лямбда-фукнции в отдельный файл. Также конструктор сейчас занимается слишком большим кол-вом действий.
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
         * - Расчет трансформаций камеры camera и сетки grid.
         * - Отрисовка интерфейса ImGui и холста графа.
         * - Обновление состояний асинхронных вычислительных потоков.
         */
        void run();

    private:
        friend class WindowEventHandler;

        GLFWwindow* window;                                                 /** @brief Системный указатель на окно программы, управляемое библиотекой GLFW. */
        CanvasManager& canvas_manager;                                      /** @brief Ссылка на менеджер холста. */
        CanvasInputController& canvas_input_controller;                     /** @brief Ссылка на контроллер ввода холста. */
        
        glm::vec2 lastMousePos = glm::vec2(0.0f);                           /** @brief Координаты последнего зарегистрированного положения курсора мыши (используется для панорамирования). */
        bool isPanning = false;                                             /** @brief Флаг, указывающий, зажата ли кнопка мыши для перемещения (скролла) по холсту. */
    
        /**
         * @brief Битовые маски для определения стороны растягивания окна (включая углы).
         */
        struct ResizeFlags {
            static constexpr uint8_t NONE   = 0;
            static constexpr uint8_t LEFT   = 1 << 0;
            static constexpr uint8_t RIGHT  = 1 << 1;
            static constexpr uint8_t TOP    = 1 << 2;
            static constexpr uint8_t BOTTOM = 1 << 3;
        };

        /**
         * @struct DraggingWindow
         * @brief Структура для управления перемещением и изменением размеров окна программы.
         */
        struct DraggingWindow {
            bool isDraggingWindow = false;                                   /** @brief Тащим ли мы само окно за TopBar */
            bool isResizingWindow = false;                                   /** @brief Растягиваем ли мы окно за край */
            uint8_t resizeEdgeFlags = ResizeFlags::NONE;                     /** @brief Битовая маска сторон, за которые ухватился пользователь */
            double dragStartX = 0, dragStartY = 0;                           /** @brief Локальные координаты клика мыши в момент начала действия */
            int windowStartX = 0, windowStartY = 0;                          /** @brief Стартовая позиция окна на экране (может быть отрицательной на multi-monitor) */
            int windowStartWidth = 0, windowStartHeight = 0;                 /** @brief Стартовые размеры окна */

            int targetWidth = 0, targetHeight = 0;
            int targetX = 0, targetY = 0;
            bool needsResize = false;

            void Reset() { 
                isDraggingWindow = false;
                isResizingWindow = false;
                resizeEdgeFlags = ResizeFlags::NONE; 
            }
        } drag_window;
    };
}
