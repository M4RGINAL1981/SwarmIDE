#include "SwarmIDE.hpp"
#include "ui/views/ViewConnection.hpp"
#include <cmath>

namespace SwarmIDE::UI {
    
    void ViewConnection::Draw(ImDrawList* drawList, float zoom) {
        try {
            if (drawList == nullptr || sourcePin == nullptr || targetPin == nullptr) return;

            auto& theme = ThemeManager::getInstance();

            float strength = theme.GetStyleVar(StyleVarRole::ConnectionStrength);
            ImU32 color = theme.GetColorU32(ColorRole::ConnectionLine);
            float thickness = theme.GetStyleVar(StyleVarRole::ConnectionThickness) * zoom;

            glm::vec2 P0 = sourcePin->GetScreenPos();
            glm::vec2 P1 = P0 + (sourcePin->GetNormal() * strength * zoom);
            glm::vec2 P2 = targetPin->GetScreenPos();
            glm::vec2 P3 = P2 + (targetPin->GetNormal() * strength * zoom);

            if (!std::isfinite(P0.x) || !std::isfinite(P0.y) || !std::isfinite(P2.x) || !std::isfinite(P2.y)) return;

            ImVec2 ImP0 = ImVec2(P0.x, P0.y);
            ImVec2 ImP1 = ImVec2(P1.x, P1.y);
            ImVec2 ImP2 = ImVec2(P2.x, P2.y);
            ImVec2 ImP3 = ImVec2(P3.x, P3.y);

            drawList->AddBezierCubic(ImP0, ImP1, ImP3, ImP2, color, thickness);
        }
        catch (const std::exception& ex) { Core::LoggingManager::Error("View connection draw failed: ", ex.what()); }
        catch (...) { Core::LoggingManager::Error("View connection draw failed with unknown exception."); }
    }
}