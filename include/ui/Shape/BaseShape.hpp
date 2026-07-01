#pragma once 
#include "SwarmIDE.hpp"
#include "ui/Topology.hpp"
#include "graphics/Camera.hpp"
#include "ui/Theme.hpp"
#include <vector>
#include <string>
#include <functional>
#include <glm/glm.hpp>

namespace SwarmIDE::UI::Shape {

    class BaseShape {
    public:
        BaseShape() = default;
        BaseShape(glm::vec2 pos) : position(pos) {};
        virtual ~BaseShape() = default;

        void SetPosition(glm::vec2 pos) { position = pos; }
        glm::vec2 GetPosition() const { return position; }

        void SetSize(glm::vec2 s) { size = s; }
        glm::vec2 GetSize() const { return size; }

        void SetRotation(float rot) { rotation = rot; }
        float GetRotation() { return rotation; }

        const std::vector<SwarmIDE::UI::AnchorPin>& GetLocalPins() const { return localPins; }

        std::vector<AnchorPin> GetLocalPins () { return localPins; }

        // Главный метод: пересчет локальных пинов в глобальные экранные координаты
        std::vector<TransformedPin> GetWorldPins(float zoomFactor) const;
        TransformedPin GetWorldPin(AnchorPin pin, float zoomFactor) const;

        AnchorPin* GetAnchorPin(std::string pinID) const;

        // Чистый виртуальный метод отрисовки формы окна
        // renderContent — это лямбда, внутри которой Агент будет рисовать свои кнопки/текст
        virtual void Draw(const std::string& title, Camera& camera, Fonts& fonts, glm::vec2 windowSize, std::function<void()> renderContent) = 0;

    protected:
        glm::vec2 position {0.0f, 0.0f};  // Позиция окна на холсте
        glm::vec2 size{400.0f, 400.0f};   // Размер окна 
        float rotation{0.0f};             // Угол поворота в радианах

        std::vector<AnchorPin> localPins; // Локальные пины фигуры
    };
}