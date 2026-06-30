#pragma once 
#include "ui/Shape/BaseShape.hpp"

namespace SwarmIDE::UI::Shape {
    class SquareShape : public BaseShape {
    public:
        SquareShape();
        SquareShape(glm::vec2 pos);
        ~SquareShape() { count = 0; }

        void Draw(const std::string& title, Camera& camera, Fonts& fonts, glm::vec2 windowSize, std::function<void()> renderContent) override;
    private:
        static int count;
    };
}