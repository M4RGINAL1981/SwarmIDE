#pragma once
#include "ui/Shape/BaseShape.hpp"
#include "ui/Topology.hpp"
#include <memory>
#include <optional>

namespace SwarmIDE::Object {
    
    class BaseObject{
    public:

        BaseObject(std::string name, glm::vec2 worldPos) : name(name), worldPos(worldPos) { count++; id = count; };
        virtual ~BaseObject() = 0; 
    
        virtual void update() = 0;                                       // Логика (что делает объект)
        virtual std::optional<bool> drawUI(Camera& camera, glm::vec2 windowSize) = 0;   // Визуализация объекта

        std::unique_ptr<SwarmIDE::UI::Shape::BaseShape>& GetShape() { return shape; }
        void GetFonts(SwarmIDE::UI::Fonts fonts) { this->fonts = fonts;}

        int GetID() { return id; }
        std::string GetIconKey() { if(iconKey != std::nullopt) { return *iconKey; }}
    
    protected:
        std::string name;                                                // Имя объекта
        int id;                                                          // Id объекта
        glm::vec2 worldPos;                                              // Коодринаты Объекта
        SwarmIDE::UI::Fonts fonts;                                       // Шрифт объекта
        std::unique_ptr<SwarmIDE::UI::Shape::BaseShape> shape;           // Форма объекта (круг, квадрат и т.д.)
        std::optional<std::string> iconKey;                              // Иконка объекта (опциоанальное поле)

        static int count;                                                // Кол-во объектов, которые было создано
    };
}