#include "SwarmIDE.hpp"
#include "ui/views/Shape/DShape.hpp"

namespace SwarmIDE::UI::Shape {

    void DShape::DrawBackground(ImDrawList* drawList, glm::vec2 pos, glm::vec2 size, float zoom, Nodes::State state, bool isHovered) {
        // 1. Вычисляем координаты углов
        ImVec2 pMin = ImVec2(pos.x, pos.y);
        ImVec2 pMax = ImVec2(pos.x + size.x, pos.y + size.y);

        // 2. Достаем менеджер тем
        auto& theme = ThemeManager::getInstance();
        
        // 3. Высчитываем масштабируемую геометрию
        float rounding = theme.GetStyleVar(StyleVarRole::NodeRounding) * zoom;
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
        ImU32 borderColor = isHovered ? 
            theme.GetColorU32(ColorRole::NodeHover) : 
            theme.GetColorU32(ColorRole::NodeBorder);

        // 6. Рисуем подложку
        drawList->AddRectFilled(pMin, ImVec2(pMax.x / 2.0f, pMax.y), bgColor, rounding, ImDrawFlags_RoundCornersLeft);
        drawList->AddRectFilled(ImVec2(pMax.x /2.0f, pMax.y), pMax, bgColor, size.x / 2.0f, ImDrawFlags_RoundCornersRight);

        // 7. Рисуем красивую рамку поверх
        drawList->AddRect(pMin, ImVec2(pMax.x / 2.0f, pMax.y), borderColor, rounding, ImDrawFlags_RoundCornersLeft, borderThickness);
        drawList->AddRect(ImVec2(pMax.x / 2.0f, pMax.y), pMax, borderColor, size.x / 2.0f, ImDrawFlags_RoundCornersRight, borderThickness);
    }
}