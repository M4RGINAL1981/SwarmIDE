#pragma once 
#include "SwarmIDE.hpp"
#include "ui/views/ViewNode.hpp"
#include "glm/glm.hpp"
#include "ui/command/CommandBus.hpp"
#include "uuid_v4/uuid_v4.h"

#include <optional>

namespace SwarmIDE::UI {

    /**
     * @class ContextMenuManager
     * @brief Менеджер управления контекстным меню холста и нод.
     */
    class ContextMenuManager {
    public:
        /**
         * @brief Глобальная точка доступа к единственному экземпляру ContextMenuManager.
         */
        static ContextMenuManager& getInstance();

        /**
         * @brief Запрос на открытие контекстного меню.
         * @param[in] mousePos Позиция мыши в момент клика.
         * @param[in] optionalNodeID ID ноды (если кликнули по ноде) или std::nullopt (если по холсту).
         */
        void RequestMenu(glm::vec2 mousePos, std::optional<UUIDv4::UUID> optionalNodeID = std::nullopt);

        /**
         * @brief Отрисовка контекстного меню.
         */
        void Draw();

        /**
         * @brief Установить командный менеджер.
         */
        void SetCommandBus(Command::CommandBus* command_bus = nullptr) { this->command_bus = command_bus; }

        // Блокируем копирование и перемещение
        ContextMenuManager(const ContextMenuManager&) = delete;
        ContextMenuManager& operator=(const ContextMenuManager&) = delete;
        ContextMenuManager(ContextMenuManager&&) = delete;
        ContextMenuManager& operator=(ContextMenuManager&&) = delete;
    private:
        ContextMenuManager() = default;
        ~ContextMenuManager() = default;

        Command::CommandBus* command_bus = nullptr;             /** @brief Указатель на командный менеджер */
        std::optional<UUIDv4::UUID> nodeID = std::nullopt;      /** @brief ID ноды, по которой кликнули */
    };

} // SwarmIDE::UI