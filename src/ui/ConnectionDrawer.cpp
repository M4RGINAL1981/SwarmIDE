#include "ui/ConnectionDrawer.hpp"

namespace SwarmIDE::UI {

    ConnectionDrawer& ConnectionDrawer::getInstance() {
        static ConnectionDrawer instance;
        return instance;
    }
    
    float ConnectionDrawer::tension = 200.0f;

    void ConnectionDrawer::DrawLink(Camera& camera, glm::vec2 windowSize) {
        ConnectionDrawer& instance = getInstance();
        std::vector<SwarmIDE::Core::Connection> connection = instance.connection_manager->getConnection();

        for(auto connect : connection){
            // Передаем 1.0f, чтобы получить чистые МИРОВЫЕ координаты точек без искажений зума на этом этапе
            std::optional<SwarmIDE::UI::TransformedPin> start_opt = instance.object_manager->GetTransformedPinForObject(connect.sourceObjectID, connect.sourcePinID, 1.0f);
            std::optional<SwarmIDE::UI::TransformedPin> end_opt = instance.object_manager->GetTransformedPinForObject(connect.targetObjectID, connect.targetPinID, 1.0f);

            if(start_opt == std::nullopt || end_opt == std::nullopt) continue;

            SwarmIDE::UI::TransformedPin start = *start_opt;
            SwarmIDE::UI::TransformedPin end = *end_opt;

            // Считаем контрольные точки Безье в мировых координатах
            glm::vec2 p1 = start.globalPos + start.globalNormal * (tension);
            glm::vec2 p2 = end.globalPos + end.globalNormal * (tension);

            // МАГИЯ ЗДЕСЬ: Переводим все 4 точки из пространства мира в пространство экрана через камеру!
            glm::vec2 screenP0 = camera.worldToScreen(start.globalPos, windowSize);
            glm::vec2 screenP1 = camera.worldToScreen(p1, windowSize);
            glm::vec2 screenP2 = camera.worldToScreen(p2, windowSize);
            glm::vec2 screenP3 = camera.worldToScreen(end.globalPos, windowSize);

            ImVec2 imP0(screenP0.x, screenP0.y);
            ImVec2 imP1(screenP1.x, screenP1.y);
            ImVec2 imP2(screenP2.x, screenP2.y);
            ImVec2 imP3(screenP3.x, screenP3.y);

            ImDrawList* drawList = ImGui::GetBackgroundDrawList();

            ImU32 color = ImGui::GetColorU32(ImVec4(0.0f, 0.8f, 1.0f, 0.8f));
            float thickness = 9.0f * camera.zoomFactor;

            drawList->AddBezierCubic(imP0, imP1, imP2, imP3, color, thickness);
        }
    }

    void ConnectionDrawer::DrawPins(Camera& camera, glm::vec2 windowSize) {
        ConnectionDrawer& instance = getInstance();
        ImDrawList* drawList = ImGui::GetBackgroundDrawList();

        for (const auto& object : instance.object_manager->objects) {
            if (!object || !object->GetShape()) continue;

            std::vector<SwarmIDE::UI::TransformedPin> worldPins = object->GetShape()->GetWorldPins(1.0f);

            for (const auto& pin : worldPins) {
                glm::vec2 screenPos = camera.worldToScreen(pin.globalPos, windowSize);

                ImVec2 center(screenPos.x, screenPos.y);
                
                float radius = 10.0f * camera.zoomFactor; 

                ImU32 pinColor = ImGui::GetColorU32(ImVec4(0.0f, 1.0f, 0.8f, 1.0f));
                ImU32 borderColor = ImGui::GetColorU32(ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
                float borderThickness = 1.5f * camera.zoomFactor;

                drawList->AddCircleFilled(center, radius, pinColor);
                drawList->AddCircle(center, radius, borderColor, 0, borderThickness);
            }
        }
    }
}