#pragma once
#include <string>
#include "SwarmIDE.hpp"
#include "ui/Theme.hpp"
#include "graphics/Camera.hpp"
#include "graphics/Grid.hpp"
#include "graphics/Colors.hpp"

namespace SwarmIDE::AI {
    class BaseAgent {
    public:
        BaseAgent(std::string name, glm::vec2 worldPos): name(name), worldPos(worldPos) {};
        virtual ~BaseAgent() = default;
        virtual void update() = 0;   // Логика
        virtual void drawUI(Camera& camera, glm::vec2 windowSize) = 0;   // Визуализация
        virtual void render() = 0;   // Запуск
        void GetFonts(SwarmIDE::UI::Fonts& fonts){ this->fonts = fonts; }
    protected:
        std::string name;
        glm::vec2 worldPos;
        SwarmIDE::UI::Fonts fonts;
    };
}