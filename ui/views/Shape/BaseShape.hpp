#pragma once 
#include "SwarmIDE.hpp"
#include "core/graph/LogicPin.hpp"
#include "nodes/base/LogicNode.hpp" 
#include "ui/views/ViewPin.hpp"
#include "ui/canvas/CanvasManager.hpp"
#include "ui/windows/ThemeManager.hpp"
#include "imgui/imgui.h"
#include "glm/glm.hpp"
#include <vector>

namespace SwarmIDE::UI::Shape {

    /**
     * @class BaseShape
     * @brief Трафарет для ViewNode
     * 
     * @see SwarmIDE::UI::ViewNode
     */
    class BaseShape {
    public:

        /**
         * @brief Дестркутор трафарета/фигуры
         */
        virtual ~BaseShape() = default;

        /**
         * @brief Отрисовка самой геометрии (подложки ноды)
         * 
         * @param[in] drawList  Список отрисовки главного окна холста
         * @param[in] pos       Координаты фигуры
         * @param[in] size      Размер фигуры
         * @param[in] zoom      Текущий зум на холсте
         * @param[in] state     Текущий статус работы логической ноды (Idle, Executing и т.д.)
         * @param[in] isHovered Наведена ли мышь на ноду
         */
        virtual void DrawBackground(ImDrawList* drawList, glm::vec2 pos, glm::vec2 size, float zoom, Nodes::State state, bool isHovered) = 0;
    
        /**
         * @brief Проверят наведена ли мышь на фигуру или нет
         * 
         * @param mousePos Положение мыши на поле в данный момент времени
         * @return true Мышь находиться на ноде
         * @return false Мышь не находиться на ноде
         */
        virtual bool isPointInside(glm::vec2 mousePos, glm::vec2 nodePos, glm::vec2 nodeSize, float zoomFactor) = 0;
    };
}
    