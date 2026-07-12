/*#include "ui/ConnectionDrawer.hpp"

namespace SwarmIDE::UI {

    ConnectionDrawer& ConnectionDrawer::getInstance() {
        static ConnectionDrawer instance;
        return instance;
    }
    
    float ConnectionDrawer::tension = 200.0f;
    bool ConnectionDrawer::isConnecting = false;

    void ConnectionDrawer::DrawLink(Camera& camera, glm::vec2 windowSize) {
        ConnectionDrawer& instance = getInstance();
        std::vector<SwarmIDE::Core::Connection> connection = instance.connection_manager->getConnection();

        for(auto connect : connection){
            std::optional<SwarmIDE::UI::TransformedPin> start_opt = instance.object_manager->GetTransformedPinForObject(connect.sourceObjectID, connect.sourcePinID, 1.0f);
            std::optional<SwarmIDE::UI::TransformedPin> end_opt = instance.object_manager->GetTransformedPinForObject(connect.targetObjectID, connect.targetPinID, 1.0f);


            if(!start_opt.has_value() || !end_opt.has_value() ) continue;

            SwarmIDE::UI::TransformedPin start = *start_opt;
            SwarmIDE::UI::TransformedPin end = *end_opt;

            glm::vec2 p1 = start.globalPos + start.globalNormal * (tension);
            glm::vec2 p2 = end.globalPos + end.globalNormal * (tension);

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

        if(isConnecting) {
            std::optional<SwarmIDE::UI::TransformedPin> start_opt = instance.object_manager->GetTransformedPinForObject(instance.sourceObjectID, instance.sourcePinID, 1.0f);

            if (start_opt != std::nullopt) {
                SwarmIDE::UI::TransformedPin start = *start_opt;

                glm::vec2 screenStart = camera.worldToScreen(start.globalPos, windowSize);
                ImVec2 imP0(screenStart.x, screenStart.y);

                ImVec2 imP3 = ImGui::GetMousePos(); 

                glm::vec2 normalScreen = start.globalNormal * tension * camera.zoomFactor;
                
                ImVec2 imP1(imP0.x + normalScreen.x, imP0.y + normalScreen.y);
                ImVec2 imP2(imP3.x - normalScreen.x, imP3.y - normalScreen.y); 

                ImDrawList* drawList = ImGui::GetBackgroundDrawList();

                ImU32 lassoColor = ImGui::GetColorU32(ImVec4(1.0f, 0.6f, 0.0f, 0.6f));
                float thickness = 9.0f * camera.zoomFactor;

                drawList->AddBezierCubic(imP0, imP1, imP2, imP3, lassoColor, thickness);
            }
        }
    }

    void ConnectionDrawer::DrawPins(Camera& camera, glm::vec2 windowSize) {
        ConnectionDrawer& instance = getInstance();
        ImDrawList* drawList = ImGui::GetBackgroundDrawList();

        ImVec2 mousePosImGui = ImGui::GetMousePos();
        glm::vec2 mousePos(mousePosImGui.x, mousePosImGui.y);

        int hoveredObjectID = -1;
        SwarmIDE::UI::AnchorPin hoveredPin; 
        bool hoveringAnyPin = false;

        for (const auto& object : instance.object_manager->objects) {
            if (!object || !object->GetShape()) continue;

            std::vector<SwarmIDE::UI::TransformedPin> worldPins = object->GetShape()->GetWorldPins(1.0f);
            
            // ИСПРАВЛЕНИЕ ТУТ: Вызываем публичный геттер вместо прямого доступа к protected полю
            const auto& localPins = object->GetShape()->GetLocalPins(); 

            for (size_t i = 0; i < worldPins.size() && i < localPins.size(); ++i) {
                const auto& pin = worldPins[i];
                const auto& localPin = localPins[i]; 

                glm::vec2 screenPos = camera.worldToScreen(pin.globalPos, windowSize);
                ImVec2 center(screenPos.x, screenPos.y);                
                float radius = 10.0f * camera.zoomFactor; 

                ImU32 pinColor = ImGui::GetColorU32(ImVec4(0.0f, 1.0f, 0.8f, 1.0f));

                float dist = glm::distance(mousePos, screenPos);
                if(dist <= radius) {
                    pinColor = ImGui::GetColorU32(ImVec4(1.0f, 0.8f, 0.0f, 1.0f));
                    hoveredObjectID = object->GetID();
                    hoveredPin = localPin; 
                    hoveringAnyPin = true;
                }

                ImU32 borderColor = ImGui::GetColorU32(ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
                float borderThickness = 1.5f * camera.zoomFactor;

                drawList->AddCircleFilled(center, radius, pinColor);
                drawList->AddCircle(center, radius, borderColor, 0, borderThickness);
            }
        }

        if(ImGui::IsMouseClicked(ImGuiMouseButton_Left) && hoveringAnyPin && !isConnecting) {
            isConnecting = true;
            instance.sourceObjectID = hoveredObjectID;
            instance.sourcePinID = hoveredPin.id; 
        }
        else if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && isConnecting) {
            if(hoveringAnyPin) {

                // Тут делаем выбор: Можно ли соединять пины одного и того же объекта?
                // hoveredObjectID != instance.sourceObjectID || hoveredPin.id != instance.sourcePinID) - можно
                // hoveredObjectID != instance.sourceObjectID - нельзя 
                
                if(hoveredObjectID != instance.sourceObjectID) { 
                    
                    SwarmIDE::UI::AnchorPin sourcePin;
                    bool sourceFound = false;

                    for (const auto& obj : instance.object_manager->objects) {
                        if (obj && obj->GetID() == instance.sourceObjectID) {
                            for (const auto& lp : obj->GetShape()->GetLocalPins()) {
                                if (lp.id == instance.sourcePinID) {
                                    sourcePin = lp;
                                    sourceFound = true;
                                    break;
                                }
                            }
                            break;
                        }
                    }

                    if (sourceFound) {
                        instance.connection_manager->connect(instance.sourceObjectID, hoveredObjectID, sourcePin, hoveredPin);
                    }
                }
            }

            isConnecting = false;
        } else if(ImGui::IsMouseClicked(ImGuiMouseButton_Right) && hoveringAnyPin && !isConnecting){
            ConnectionDrawer& instance = getInstance();
            std::vector<SwarmIDE::Core::Connection> connection = instance.connection_manager->getConnection();

            for(auto& connect : connection) {
                if((connect.sourcePinID == hoveredPin.id && connect.sourceObjectID == hoveredObjectID) ||
                   (connect.targetPinID == hoveredPin.id && connect.targetObjectID == hoveredObjectID)) {
                    instance.connection_manager->disconnect(connect);
                }
            }
        }
    }
}
*/