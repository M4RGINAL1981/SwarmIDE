/*#pragma once 
#include "ui/Shape/BaseShape.hpp"

namespace SwarmIDE::UI::Shape {
    class RectangleShape : public BaseShape {
    public:
        RectangleShape();
        RectangleShape(glm::vec2 pos);
        ~RectangleShape() { count = 0; }

        bool Draw(const std::string& title, Camera& camera, Fonts& fonts, glm::vec2 windowSize, std::function<void()> renderContent, std::optional<std::string> iconKey = std::nullopt) override;
    private:
        static int count;
    };
}
    */
