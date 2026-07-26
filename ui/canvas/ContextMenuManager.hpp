#pragma once 
#include "SwarmIDE.hpp"
#include "ui/views/ViewNode.hpp"
#include "glm/glm.hpp"

namespace SwarmIDE::UI {

    /**
     * @class ContextMenuManager
     * @brief Контекстный менеджер
     * 
     */
    class ContextMenuManager {
    public:
        /**
         * @brief Глобальная точка доступа к единственному экземпляру ContextMenuManager.
         * @return ContextMenuManager& Ссылка на синглтон генератора идентификаторов.
         */
        static ContextMenuManager& getInstance();

        /**
         * @brief Открытие меню ноды
         * @param[in] mousePos 
         * @param[in] optionalNodeID 
         */
        void RequestMenu(glm::vec2 mousePos, UUIDv4::UUID optionalNodeID);

        /**
         * @brief Отрисовка меню ноды
         * @details Отрисовывает всплывающее ImGui-меню (ImGui::BeginPopupContextWindow).
         * Если кликнули по ноде, показывает пункты: «Open Inspector», «Delete Node», «Duplicate».
         */
        void Draw();

        // Блокируем копирование и перемещение менеджера
        ContextMenuManager(const ContextMenuManager&) = delete;
        ContextMenuManager& operator=(const ContextMenuManager&) = delete;
        ContextMenuManager(ContextMenuManager&&) = delete;
        ContextMenuManager& operator=(ContextMenuManager&&) = delete;
    private:
        ContextMenuManager() = default;
        ~ContextMenuManager() = default;


        UUIDv4::UUID nodeID;      /** @brief < ID ноды по которой кликнули */
    };

}