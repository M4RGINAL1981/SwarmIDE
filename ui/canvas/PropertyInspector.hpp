#pragma once 
#include "SwarmIDE.hpp"
#include "core/graph/GraphManager.hpp"
#include "nodes/base/LogicNode.hpp"
#include "ui/windows/ThemeManager.hpp"
#include "ui/command/CommandBus.hpp"
#include "uuid_v4/uuid_v4.h"

#include <string>
#include <functional>
#include <unordered_map>
#include <optional>

namespace SwarmIDE::UI {

    /**
     * @class PropertyInspector
     * @brief Инспектор свойств (UI-окно) для редактирования внутренних данных нод.
     */
    class PropertyInspector {
    public:
        static PropertyInspector& getInstance() {
            static PropertyInspector instance;
            return instance;
        }

        /**
         * @brief Функция отрисовки/применения свойств ноды.
         * @param[in,out] node Указатель на логическую ноду.
         * @param[in,out] applyChanges Флаг, указывающий режим: false — отрисовка UI, true — сохранение данных.
         */
        using NodeUIDrawer = std::function<void(Nodes::LogicNode*, bool&)>;

        /**
         * @brief Открыть инспектор для указанной ноды.
         */
        void OpenForNode(UUIDv4::UUID nodeID);

        /**
         * @brief Отрисовка модального окна инспектора.
         */
        void Draw();

        // Запрещаем перемещение и копирование
        PropertyInspector(const PropertyInspector&) = delete;
        PropertyInspector& operator=(const PropertyInspector&) = delete;
        PropertyInspector(PropertyInspector&&) = delete;
        PropertyInspector& operator=(PropertyInspector&&) = delete;
    private:
        PropertyInspector();
        ~PropertyInspector() = default;

        void RegisterDrawers();

        std::unordered_map<std::string, NodeUIDrawer> drawers;
        std::optional<UUIDv4::UUID> activeNodeID = std::nullopt;
        bool openTrigger = false;
        
        char textBuffer[2048] = ""; 
    };

} // SwarmIDE::UI