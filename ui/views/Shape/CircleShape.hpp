#pragma once 
#include "SwarmIDE.hpp"
#include "ui/views/Shape/BaseShape.hpp"

namespace SwarmIDE::UI::Shape {

    /**
     * @class CircleShape
     * @brief Фигура круг
     */
    class CircleShape : public BaseShape {
    public:
        /**
         * @brief Конструктор фигуры круг
         */
        CircleShape() = default;

        /**
         * @brief Деструктор фигуры круг
         */
        ~CircleShape() = default;

        /**
         * @brief Отрисовка самой геометрии (подложки ноды)
         * 
         * @param[in] drawList   Список отрисовки главного окна холста
         * @param[in] pos        Координаты фигуры
         * @param[in] size       Размер фигуры
         * @param[in] zoom       Текущий зум на холсте
         * @param[in] state      Текущий статус работы логической ноды (Idle, Executing и т.д.)
         * @param[in] isHovered  Наведена ли мышь на ноду
         * @param[in] isSelected Выбрана ли нода 
         */
        virtual void DrawBackground(ImDrawList* drawList, glm::vec2 pos, glm::vec2 size, float zoom, Nodes::State state, bool isHovered, bool isSelected) override;

        /**
         * @brief Проверят наведена ли мышь на фигуру или нет
         * 
         * @param mousePos   Положение мыши на экране в данный момент времени
         * @param nodePos    Положение ноды на экране в данный момент времени
         * @param nodeSize   Размер ноды
         * @param zoomFactor Текущий зум
         * 
         * @return true Мышь находиться на ноде
         * @return false Мышь не находиться на ноде
         */
        virtual bool isPointInside(glm::vec2 mousePos, glm::vec2 nodePos, glm::vec2 nodeSize, float zoomFactor) override;
    };
}