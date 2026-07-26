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
        CircleShape();

        /**
         * @brief Деструктор фигуры круг
         */
        ~CircleShape() = default;

        virtual void DrawBackground(ImDrawList* drawList, glm::vec2 pos, glm::vec2 size, float zoom, Nodes::State state, bool isHovered) override;

    private:
        
    };
}