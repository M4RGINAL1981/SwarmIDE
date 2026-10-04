#pragma once
#include "SwarmIDE.hpp"
#include "nodes/base/LogicNode.hpp"
#include "ui/views/Shape/BaseShape.hpp"
#include "ui/views/ViewPin.hpp"
#include "ui/windows/ThemeManager.hpp"
#include "graphics/TextureManager.hpp"
#include "glm/glm.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <unordered_map>

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
         * @param spawnPos Координаты ноды для создания
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
         * @brief Поиск визуального пина по его ID
         * 
         * @param[in] pinID ID пина для поиска
         * @return ViewPin* Указатель на визуальный пин, если найден, иначе nullptr   
         */
        ViewPin* findViewPin(UUIDv4::UUID pinID);

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
         * @param[in] wP новые координаты визуальной ноды
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

        /**
         * @brief Сместить ноду на дельту
         * @param[in] delta дельта смещения ноды 
         */
        void MoveBy(glm::vec2 delta) { worldPos += delta; }

        /**
         * @brief Установить статус: Наведена ли мышь на ноду или нет?
         * @param[in] state Статус
         */
        void SetHovered(bool state) { isHovered = state; }

        /**
         * @brief Установить статус: Выбрана ли нода сейчас или нет?
         * @param[in] state Статус
         */
        void SetSelected(bool state) { isSelected = state; }

        /**
         * @brief Вернуть все визуальные пины нодыы
         * @return std::unordered_map<UUIDv4::UUID, std::unique_ptr<ViewPin>>& Контейнер с визуальными пинами
         */
        std::unordered_map<UUIDv4::UUID, std::unique_ptr<ViewPin>>& GetViewPins() { return viewPins; }

        /**
         * @brief Обрабатывает перемещение ноды мышкой (Drag)
         * @warning Сейчас не испльзуется, возмножно будет в будущем. Сейчас перемещение ноды занимается CanvasManager
         */
        void HandleInteraction();

        /**
         * @brief Загрузка визуальной ноды в json
         * @return json 
         */
        json serializeViewNode() const;

        /**
         * @brief Загрузка визуальной ноды из json
         * @param[in] jsonNode
         */
        void deserializeViewNode(json jsonNode);

    private:

        friend class CanvasManager;
        friend class CanvasInputController;

        UUIDv4::UUID nodeID;                                                      /** @brief < ID ноды. */
        Nodes::State state = Nodes::State::Idle;                                  /** @brief < Статус ноды. */
        std::unique_ptr<SwarmIDE::UI::Shape::BaseShape> shape;                    /** @brief < Указатель на фигуру/трафарет */
        std::unordered_map<UUIDv4::UUID, std::unique_ptr<ViewPin>> viewPins;      /** @brief < Контейнер для быстрого поиска пинов по ID. */
        glm::vec2 worldPos;                                                       /** @brief < Положение ноды на поле. */
        glm::vec2 nodeSize;                                                       /** @brief < Размер ноды. */
        bool isSelected;                                                          /** @brief < Выбрана ли нода в данный момент. */
        bool isHovered;                                                           /** @brief < Наведена ли мышь на хитбокс ноды. */
    };
}