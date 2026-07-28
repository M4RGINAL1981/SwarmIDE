#include "SwarmIDE.hpp"
#include "ui/views/ViewPin.hpp"
#include "ui/windows/ThemeManager.hpp"
#include "core/engine/LoggingManager.hpp"
#include "glm/geometric.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace SwarmIDE::UI {

    ViewPin::ViewPin(Core::PinID pinID, Core::Direction dir, int index, glm::vec2 localPos, glm::vec2 normal) {
        this->pinID = pinID;
        this->dir = dir;
        this->index = index;
        this->localPos = localPos;
        this->normal = normal;
    }

    bool ViewPin::ComplianceСheck(Core::PinID pinID, Core::Direction dir, int index) { 
        if(this->pinID == pinID && this->dir == dir && this->index == index) return true;
        return false;
    }

    void ViewPin::Draw(ImDrawList* drawList, float zoomFactor) {

        auto& theme = ThemeManager::getInstance();

        auto colorDefault = theme.GetColorU32(ColorRole::PinDefault);
        auto colorHover = theme.GetColorU32(ColorRole::PinHover);

        auto radius = theme.GetStyleVar(StyleVarRole::PinRadius) * zoomFactor;
        auto shapePin = theme.GetPinShape();

        if(shapePin == PinShapeType::Circle) {
            ImVec2 center = ImVec2(cachedScreenPos.x, cachedScreenPos.y);

            if(is_hovered) drawList->AddCircleFilled(center, radius, colorHover);
            else drawList->AddCircleFilled(center, radius, colorDefault);
            
        }
        else if (shapePin == PinShapeType::Square) {
            ImVec2 pMin = ImVec2(cachedScreenPos.x - radius, cachedScreenPos.y - radius);
            ImVec2 pMax = ImVec2(cachedScreenPos.x + radius, cachedScreenPos.y + radius);

            if(is_hovered) drawList->AddRectFilled(pMin, pMax, colorHover);
            else drawList->AddRectFilled(pMin, pMax, colorDefault);    
            
        }
        else {
            Core::LoggingManager::Fatal("Shape Pin is not identifacate");
            throw std::runtime_error("Shape Pin is not identificate");
        }
    }

    void ViewPin::UpdatePos(glm::vec2 NodeScreenPos, float zoom) {
        cachedScreenPos = NodeScreenPos + (localPos * zoom); 
    }

    bool ViewPin::IsPointInside(glm::vec2 mousePos, float zoomFactor) {
        auto& theme = ThemeManager::getInstance();
        auto radius = theme.GetStyleVar(StyleVarRole::PinRadius) * zoomFactor;

        if(glm::length(mousePos - cachedScreenPos) < radius) return true;
        return false;
    }
}
