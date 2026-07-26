#pragma once 
#include "SwarmIDE.hpp"
#include "ui/views/Shape/BaseShape.hpp"

namespace SwarmIDE::UI::Shape {

    class RectangleShape : public BaseShape {
    public:
        RectangleShape() = default;
        ~RectangleShape() = default;


        virtual void DrawBackground(ImDrawList* drawList, glm::vec2 pos, glm::vec2 size, float zoom, Nodes::State state, bool isHovered) override;
    private:
    };
}