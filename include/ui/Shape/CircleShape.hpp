#pragma once 
#include "ui/Shape/BaseShape.hpp"

namespace SwarmIDE::UI::Shape {
    class CircleShape : public BaseShape {
    public:
        CircleShape();
        CircleShape(glm::vec2 pos);
        ~CircleShape() { count = 0; }

        bool Draw(const std::string& title, Camera& camera, Fonts& fonts, glm::vec2 windowSize, std::function<void()> renderContent, std::optional<std::string> iconKey = std::nullopt) override;
    private:
        static int count;
    };
}
