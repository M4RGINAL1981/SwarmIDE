#pragma once
#include "SwarmIDE.hpp"
#include "nodes/base/LogicNode.hpp"
#include "ui/views/Shape/BaseShape.hpp"
#include "ui/views/ViewPin.hpp"
#include "ui/windows/ThemeManager.hpp"
#include "glm/glm.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace SwarmIDE::UI {

    /**
     * @class ViewNode
     * @brief Визуальная нода
     * @details Используемый паттерн проектирования наблюдатель.
     * 
     * @see SwarmIDE::Node::LogicNode
     * @see SwarmIDE::UI::ViewPin
     * @see SwarmIDE::UI::ThemeManager
     */
    class ViewNode {
    public:

        /**
         * @brief Конструктор визуальной ноды
         * 
         * @param[in] nodeID ID логической ноды
         */
        ViewNode(UUIDv4::UUID nodeID);

        /**
         * @brief Конструктор визуальной ноды
         * 
         * @param nodeID ID логической ноды
         * @param worldPos Координаты ноды для создания
         */
        ViewNode(UUIDv4::UUID nodeID, glm::vec2 spawnPos) : ViewNode(nodeID) { worldPos = spawnPos; }

        /**
         * @brief Деструктор визуальной ноды
         * 
         */
        ~ViewNode() = default;

        /**
         * @brief Отрисовка подложки, заголовка, иконки и вызов DrawPins()
         * 
         * @param[in] drawList   Спец лист
         * @param[in] screenPos  Координаты визуальной ноды на экране
         * @param[in] zoom       Текущий зум
         */
        void Draw(ImDrawList* drawList, glm::vec2 screenPos, float zoom);

        /**
         * @brief 
         * 
         * @param[in] pinID  Идентификтор пина
         * @param[in] dir    Направление пина (Вход / Выход)
         * @param[in] index  Индекс пина, если есть пины с одинаковыми PinID
         * @return const ViewPin& 
         */
        ViewPin* findViewPin(Core::PinID pinID, Core::Direction dir, int index);

        /**
         * @brief Опрашивает состояние ноды
         * @details 
         * State::Idle      -> Cерый 
         * State::Waiting   -> Желтый
         * State::Executing -> Синий
         * State::Success   -> Зеленый
         * State::Failed    -> Красный
         * 
         * @return const Nodes::State& - текущий статус ноды
         */
        const Nodes::State& GetStateColor();

        /**
         * @brief Получить мировые координаты визуальной ноды
         * 
         * @return glm::vec2& 
         */
        glm::vec2& GetWorldPos() { return worldPos; }

        /**
         * @brief Установить новые мировые координаты визуальной ноды
         * 
         * @param[in] glm::vec2 wP новые координаты визуальной ноды
         */
        void SetWorldPos(glm::vec2 wP) { worldPos = wP; }

        /**
         * @brief Возвращает размер визуальной ноды
         * @return glm::vec2& 
         */
        glm::vec2& GetNodeSize() { return nodeSize; }
        /**
         * @brief Устанавливает размер визуальной ноды
         * @param nS 
         */
        void SetNodeSize(glm::vec2 nS) { nodeSize = nS; }

        void MoveBy(glm::vec2 delta) { worldPos += delta; }
        void SetHovered(bool state) { isHovered = state; }
        void SetSelected(bool state) { isSelected = state; }

        /**
         * @brief Вернуть все визуальные пины нодыы
         * 
         * @return std::vector<std::unique_ptr<ViewPin>> 
         */
        std::vector<std::unique_ptr<ViewPin>>& GetViewPins() { return viewPins; }

        /**
         * @brief Обрабатывает перемещение ноды мышкой (Drag)
         */
        void HandleInteraction();

    private:

        friend CanvasManager;

        UUIDv4::UUID nodeID;                                             /** @brief < ID ноды. */
        Nodes::State state = Nodes::State::Idle;                         /** @brief < Статус ноды. */
        std::unique_ptr<SwarmIDE::UI::Shape::BaseShape> shape;           /** @brief < Указатель на фигуру/трафарет */
        std::vector<std::unique_ptr<ViewPin>> viewPins;                  /** @brief < Визуальные пины ноды */
        glm::vec2 worldPos;                                              /** @brief < Положение ноды на поле. */
        glm::vec2 nodeSize;                                              /** @brief < Размер ноды. */
        bool isSelected;                                                 /** @brief < Выбрана ли нода в данный момент. */
        bool isHovered;                                                  /** @brief < Наведена ли мышь на хитбокс ноды. */
    };
}