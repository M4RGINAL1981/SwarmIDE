#include "SwarmIDE.hpp"
#include "ui/views/ViewPin.hpp"
#include "ui/windows/ThemeManager.hpp"
#include "core/engine/LoggingManager.hpp"
#include "glm/geometric.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <cmath>

namespace SwarmIDE::UI {

    ViewPin::ViewPin(UUIDv4::UUID id, glm::vec2 localPos, glm::vec2 normal) {
        this->id = id;
        this->localPos = localPos;
        this->normal = normal;
        this->cachedScreenPos = glm::vec2(0.0f, 0.0f);
        this->is_hovered = false;
    }

    void ViewPin::Draw(ImDrawList* drawList, float zoomFactor) {
        try {
            if (drawList == nullptr) {
                return;
            }

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
                Core::LoggingManager::Warning("Shape Pin is not identified, skipping draw.");
                return;
            }
        }
        catch (const std::exception& ex) {
            Core::LoggingManager::Error("View pin draw failed: ", ex.what());
        }
        catch (...) {
            Core::LoggingManager::Error("View pin draw failed with unknown exception.");
        }
    }

    void ViewPin::UpdatePos(glm::vec2 NodeScreenPos, float zoom) {
        cachedScreenPos = NodeScreenPos + (localPos * zoom); 
    }

    bool ViewPin::IsPointInside(glm::vec2 mousePos, float zoomFactor) {
        auto& theme = ThemeManager::getInstance();
        auto radius = theme.GetStyleVar(StyleVarRole::PinRadius) * zoomFactor;

        if (!std::isfinite(cachedScreenPos.x) || !std::isfinite(cachedScreenPos.y) ||
            !std::isfinite(mousePos.x) || !std::isfinite(mousePos.y)) {
            return false;
        }

        if(glm::length(mousePos - cachedScreenPos) < radius) return true;
        return false;
    }
}
