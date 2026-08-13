#pragma once 
#include "SwarmIDE.hpp"
#include "core/graph/GraphManager.hpp"
#include "nodes/base/LogicNode.hpp"
#include "ui/windows/ThemeManager.hpp"
#include "ui/command/CommandBus.hpp"
#include <string>

namespace SwarmIDE::UI {

    /**
     * @class PropertyInspector
     * @brief 
     * @todo Нужно доделать в будущих версиях
     */
    class PropertyInspector {
    public:
        /**
         * @brief Глобальная точка доступа к единственному экземпляру ContextMenuManager.
         * @return ContextMenuManager& Ссылка на синглтон генератора идентификаторов.
         */
        static PropertyInspector& getInstance();

        /**
         * @brief Фокусирует инспектор на конкретной ноде, 
         * считывает её текущие данные из Core.
         * @param[in] nodeID ID ноды
         */
        void OpenForNode(UUIDv4::UUID nodeID);

        /**
         * @brief Рендерит боковую панель
         */
        void Draw();

        /**
         * @brief Внутренний полиморфный метод
         * @param[in] nodeID ID ноды
         */
        void RenderNodeSpecificControls(UUIDv4::UUID nodeID);

        // Запрещаем перемещение и копирование менеджера
        PropertyInspector(const PropertyInspector&) = delete;
        PropertyInspector& operator=(const PropertyInspector&) = delete;
        PropertyInspector(PropertyInspector&&) = delete;
        PropertyInspector& operator=(PropertyInspector&&) = delete;
    private:
        PropertyInspector() = default;
        ~PropertyInspector() = default;

        UUIDv4::UUID targetNodeID;
        bool isOpen;
        std::string inputBuffer;
    };

}