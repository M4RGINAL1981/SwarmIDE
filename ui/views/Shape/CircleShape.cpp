#include "SwarmIDE.hpp"
#include "ui/views/Shape/CircleShape.hpp"
#include "ui/canvas/CanvasManager.hpp"

namespace SwarmIDE::UI::Shape {

void CircleShape::DrawBackground(ImDrawList* drawList, glm::vec2 pos, glm::vec2 size, float zoom, Nodes::State state, bool isHovered, bool isSelected) {
        // 1. Вычисляем координаты углов
        ImVec2 center = ImVec2(pos.x + (size.x / 2.0f), pos.y + (size.y / 2.0f));
        float radius = (size.x / 2.0f);

        // 2. Достаем менеджер тем
        auto& theme = ThemeManager::getInstance();
        
        // 3. Высчитываем масштабируемую геометрию
        float borderThickness = theme.GetStyleVar(StyleVarRole::NodeBorderThickness) * zoom;
        
        // 4. ОПРЕДЕЛЯЕМ ЦВЕТ ЗАЛИВКИ В ЗАВИСИМОСТИ ОТ СТАТУСА
        ImU32 bgColor;
        switch (state) {
            case Nodes::State::Waiting:   
                bgColor = theme.GetColorU32(ColorRole::NodeStateWaiting); break;
            case Nodes::State::Executing: 
                bgColor = theme.GetColorU32(ColorRole::NodeStateExecuting); break;
            case Nodes::State::Success:   
                bgColor = theme.GetColorU32(ColorRole::NodeStateSuccess); break;
            case Nodes::State::Failed:    
                bgColor = theme.GetColorU32(ColorRole::NodeStateFailed); break;
            case Nodes::State::Idle:
            default:                      
                bgColor = theme.GetColorU32(ColorRole::NodeStateIdle); break;
        }

        // 5. ОПРЕДЕЛЯЕМ ЦВЕТ РАМКИ В ЗАВИСИМОСТИ ОТ НАВЕДЕНИЯ МЫШИ
        ImU32 borderColor = (isHovered || isSelected) ? 
            theme.GetColorU32(ColorRole::NodeHover) : 
            theme.GetColorU32(ColorRole::NodeBorder);        

        // 6. Рисуем подложку
        drawList->AddCircleFilled(center, radius, bgColor);

        // 7. Рисуем красивую рамку поверх
        drawList->AddCircle(center, radius, borderColor, 0, borderThickness);
    }

    bool CircleShape::isPointInside(glm::vec2 mousePos, glm::vec2 nodePos, glm::vec2 nodeSize, float zoomFactor) {
        glm::vec2 scaledSize = nodeSize * zoomFactor;
        glm::vec2 center = nodePos + (scaledSize / 2.0f);
        float radius = scaledSize.x / 2.0f;
        return glm::length(mousePos - center) <= radius;
    }
}