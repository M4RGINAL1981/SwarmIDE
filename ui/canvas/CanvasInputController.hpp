#pragma once
#include "SwarmIDE.hpp"
#include "core/graph/GraphManager.hpp"
#include "ui/command/CommandBus.hpp"
#include "graphics/Camera.hpp"
#include "graphics/Grid.hpp"
#include "core/engine/LoggingManager.hpp"
#include <optional>

namespace SwarmIDE::UI {

    /**
     * @brief Указывает текущиее состояние событий мыши
     */
    enum class MouseEvents {
        IDLE,                  /** @brief < Ничего не делаем / Просто водим мышкой */
        DRAGGING_NODE,         /** @brief < Тащим ноду */
        DRAGGING_WIRE          /** @brief < Тянем связь из пина */
    };

    /**
     * @struct ActiveLinkContext
     * @brief Структура, хранящая состояние сборки связи 
     * (тянет ли пользователь провод из пина прямо сейчас, откуда тянет, где мышь).
     */
    struct ActiveLinkContext {
        MouseEvents state = MouseEvents::IDLE;         /** @brief < Текущиее состояние */

        // Данные для перемещения ноды
        UUIDv4::UUID grabberNodeID;                    /** @brief < Какую ноду тащим */
        glm::vec2 initialNodePos;                      /** @brief < Где она была до начала перетаскивания (для отмены) */

        // Данные для создания связи
        UUIDv4::UUID startNodeID;                      /** @brief < Из какой ноды тянем связь */  
        UUIDv4::UUID startPinID;                       /** @brief < Из какого пина тянем связь */
        glm::vec2 currentMouseWorldPos;                /** @brief < Текущие координаты мыши в мире */

        void Reset() {                                 /** @brief < Методы для очистки контекста */
            state = MouseEvents::IDLE;
            initialNodePos = glm::vec2(0.0f, 0.0f);
            currentMouseWorldPos = glm::vec2(0.0f, 0.0f);
        }    
    };

    /**
     * @class CanvasInputController
     * @brief Класс, который обрабатывает ввод пользователя на холсте (Canvas) и управляет взаимодействием с визуальными нодами и связями.
     * 
     * @see SwarmIDE::UI::Command::CommandBus
     * @see SwarmIDE::UI::CanvasManager
     */
    class CanvasInputController {
    public:
        /**
         * @brief Глобальная точка доступа к единственному экземпляру CanvasInputController.
         */
        static CanvasInputController& getInstance();

        /**
         * @brief 
         */
        void ProcessInput();

        /**
         * @brief Устанавливает указатель на менеджер холста, чтобы CanvasInputController мог управлять визуальными нодами и связями.
         * @param[in] canvasManager Указатель на менеджер холста, чтобы CanvasInputController мог управлять визуальными нодами и связями.
         */
        void SetCanvasManager(CanvasManager* canvasManager) { this->canvas_manager = canvasManager; }
        
        /**
         * @brief Установить камеру
         * @param[in] camera
         */
        void SetCamera(Graphics::Camera* camera) { this->camera = camera; }
 
        // Блокируем копирование и перемещение менеджера
        CanvasInputController(const CanvasInputController&) = delete;
        CanvasInputController& operator=(const CanvasInputController&) = delete;
        CanvasInputController(CanvasInputController&&) = delete;
        CanvasInputController& operator=(CanvasInputController&&) = delete;
    private:
        CanvasInputController() : command_bus(Command::CommandBus::getInstance()) { Core::LoggingManager::Info("[CANVAS_INPUT_CONTROLLER]: Constructor"); }
        ~CanvasInputController() { Core::LoggingManager::Info("[CANVAS_INPUT_CONTROLLER]: Destructor"); }

        /**
         * @brief Обрабатывает нажатие левой кнопки мыши на холсте.
         * @details 
         * Если левая кнопка мыши зажата на ноде - событие перетаскивание ноды.
         * Если левая кнопка мыши зажата на пине - событие создание связи. 
         * @param[in] mousePos Текущее положение мыши на экране.
         */
        void HandleMouseLeftClick(glm::vec2 mousePos);

        /**
         * @brief Открывает контекстное меню ноды, если мышь наведена на ноду и нажата правая кнопка мыши.
         * @details Внутри происходит проверка, наведена ли мышь на ноду, и если да, то открывается контекстное меню с опциями для этой ноды.
         * @param[in] mousePos Текущее положение мыши на экране.
         */
        void HandleMouseRightClick(glm::vec2 mousePos);

        /**
         * @brief Отлавливаем хоткеи.
         * @details
         * if (ImGui::IsKeyPressed(ImGuiKey_Delete)) -> ищем выделенную ноду (Hovered/Selected) -> пушим CmdDeleteNode
         * if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Z)) -> вызываем CommandBus::getInstance().Undo()
         * if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Y)) -> вызываем CommandBus::getInstance().Redo()
         */
        void HandleKeyboardInputs();

        ActiveLinkContext activeLinkContext;                                    /** @brief < Контекст активной связи */
        CanvasManager* canvas_manager = nullptr;                                /** @brief < Менеджер холста*/
        Graphics::Camera* camera = nullptr;                                     /** @brief < Камера */
        Command::CommandBus& command_bus;                                       /** @brief < Командный менеджер */

        std::optional<UUIDv4::UUID> selectedNodeID = std::nullopt;              /** @brief < ID выбранной нodы (для удаления) */
        std::optional<UUIDv4::UUID> lastHoveredPin = std::nullopt;              /** @brief < ID Последний наведенный пин */
        std::optional<UUIDv4::UUID> NodehasLastHoveredPin = std::nullopt;       /** @brief < ID Нода, которой принадлежит последний наведенный пин */
        std::optional<UUIDv4::UUID> lastHoveredNode = std::nullopt;             /** @brief < ID Последняя наведенная нода */
    };
}