#include "SwarmIDE.hpp"
#include "ui/views/Shape/DShape.hpp"
#include "ui/windows/ThemeManager.hpp"
#ifndef IM_PI
#define IM_PI 3.14159265358979323846f
#endif

namespace SwarmIDE::UI::Shape {

    void DShape::DrawBackground(ImDrawList* drawList, glm::vec2 pos, glm::vec2 size, float zoom, Nodes::State state, bool isHovered) {
        ImVec2 pMin = ImVec2(pos.x, pos.y);
        ImVec2 pMax = ImVec2(pos.x + size.x, pos.y + size.y);

        auto& theme = ThemeManager::getInstance();
        float rounding = theme.GetStyleVar(StyleVarRole::NodeRounding) * zoom;
        float borderThickness = theme.GetStyleVar(StyleVarRole::NodeBorderThickness) * zoom;
        
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

        ImU32 borderColor = isHovered ? 
            theme.GetColorU32(ColorRole::NodeHover) : 
            theme.GetColorU32(ColorRole::NodeBorder);

        // Идеальный D-Shape через векторный путь ImGui
        float radius = size.y / 2.0f;
        ImVec2 centerRight(pMax.x - radius, pMin.y + radius);

        // 1. Рисуем заливку подложки
        drawList->PathArcTo(ImVec2(pMin.x + rounding, pMin.y + rounding), rounding, IM_PI, IM_PI * 1.5f); // Левый верхний
        drawList->PathArcTo(centerRight, radius, IM_PI * 1.5f, IM_PI * 2.0f);                            // Правый верхний полукруг
        drawList->PathArcTo(centerRight, radius, 0.0f, IM_PI * 0.5f);                                     // Правый нижний полукруг
        drawList->PathArcTo(ImVec2(pMin.x + rounding, pMax.y - rounding), rounding, IM_PI * 0.5f, IM_PI); // Левый нижний
        drawList->PathFillConvex(bgColor);

        // 2. Рисуем внешний контур (цельный, без внутренних швов)
        drawList->PathArcTo(ImVec2(pMin.x + rounding, pMin.y + rounding), rounding, IM_PI, IM_PI * 1.5f);
        drawList->PathArcTo(centerRight, radius, IM_PI * 1.5f, IM_PI * 2.0f);
        drawList->PathArcTo(centerRight, radius, 0.0f, IM_PI * 0.5f);
        drawList->PathArcTo(ImVec2(pMin.x + rounding, pMax.y - rounding), rounding, IM_PI * 0.5f, IM_PI);
        drawList->PathStroke(borderColor, ImDrawFlags_Closed, borderThickness);
    }

    bool DShape::isPointInside(glm::vec2 mousePos, glm::vec2 nodePos, glm::vec2 nodeSize, float zoomFactor) {
        glm::vec2 pMin = nodePos;
        glm::vec2 pMax = nodePos + (nodeSize * zoomFactor);
        
        if (mousePos.x >= pMin.x && mousePos.x <= pMax.x &&
            mousePos.y >= pMin.y && mousePos.y <= pMax.y) {
            return true;
        }
        return false;
    }
}