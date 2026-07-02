#include "ui/Shape/CircleShape.hpp"
#include "core/TextureManager.hpp"
#include <iostream>
#include <vector>
#include <string>
#include "imgui.h"
                             

namespace SwarmIDE::UI::Shape {
    
    CircleShape::CircleShape() {

        this->size = glm::vec2(200.0f, 200.0f);

        this->localPins = {
            {"Out", SwarmIDE::UI::PinType::Universal, glm::vec2(0.5f, 0.0f), glm::vec2(0.0f, -1.0f)} 
        };

        count++;
    }

    CircleShape::CircleShape(glm::vec2 pos) : BaseShape(pos) {
        this->size = glm::vec2(200.0f, 200.0f);

        this->localPins = {
            {"Out", SwarmIDE::UI::PinType::Universal, glm::vec2(0.5f, 0.0f), glm::vec2(0.0f, -1.0f)}
        };

        count++;
    }

    int CircleShape::count = 0;

    void CircleShape::Draw(const std::string& title, Camera& camera, Fonts& fonts, glm::vec2 windowSize, std::function<void()> renderContent, std::optional<std::string> iconKey) {
        float zoom = camera.zoomFactor;
        
        auto config = SwarmIDE::UI::Theme::GetAgentConfig();                      // Берём параметры из Theme      

        glm::vec2 screenPos = camera.worldToScreen(this->position, windowSize);   // Находим координаты окна на экране

        if(!ImGui::IsMouseDragging(0)){  // Передвигаем наше окно 
            ImGui::SetNextWindowPos(ImVec2(screenPos.x, screenPos.y), ImGuiCond_Always);
        }
    
        ImGui::SetNextWindowSize(ImVec2(size.x * camera.zoomFactor, size.y * camera.zoomFactor));

        //ImGui::PushStyleColor(ImGuiCol_WindowBg, config.bgColor);
        //ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, config.rounding * camera.zoomFactor);

        if(ImGui::Begin((title).c_str(), nullptr, config.flags | ImGuiWindowFlags_NoBackground)) {

            int index = camera.GetCurrentZoomIndex();
            float baseScale =  2.0f * index + 14.0f;

            ImDrawList* drawList = ImGui::GetWindowDrawList();

            ImVec2 scaledSize = ImVec2(size.x * camera.zoomFactor, size.y * camera.zoomFactor);
            ImVec2 winPos = ImGui::GetWindowPos();
            ImVec2 screenPosCenter = ImVec2(winPos.x + scaledSize.x / 2.0f, winPos.y + scaledSize.y / 2.0f);

            ImU32 pinColor = ImGui::GetColorU32(ImVec4(0.0f, 1.0f, 0.8f, 1.0f));
            ImU32 borderColor = ImGui::GetColorU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f));

            float borderThickness = 1.5f * camera.zoomFactor;

            drawList->AddCircleFilled(screenPosCenter, scaledSize.x / 2.0, pinColor);
            drawList->AddCircle(screenPosCenter, scaledSize.x / 2.0, borderColor, 0, borderThickness);

            if(iconKey != std::nullopt) {
                auto textura = SwarmIDE::Core::TextureManager::getInstance().getTexture(*iconKey);

                if (textura.has_value()) {
                    float iconHalfSize = (size.x * 0.25f) * camera.zoomFactor; 
                    
                    ImVec2 pMin = ImVec2(screenPosCenter.x - iconHalfSize, screenPosCenter.y - iconHalfSize);
                    ImVec2 pMax = ImVec2(screenPosCenter.x + iconHalfSize, screenPosCenter.y + iconHalfSize);
                    
                    ImTextureID texID = (ImTextureID)(uintptr_t)textura->id;
                    
                    drawList->AddImage(texID, pMin, pMax);
                }
            }

            float targetPixelSize = 18.0f * camera.zoomFactor; // 18px — базовый размер при зуме 1.0
            float currentFontBaseSize = 2.0f * index + 16.0f;  // Размер шрифта в текущем дефолтном индексе атласа
            float fontScale = targetPixelSize / currentFontBaseSize;

            ImGui::PushFont(fonts.regularFont[index]);
            ImGui::SetWindowFontScale(fontScale); // Вот эта херня дотянет размер текста до пиксель-перфект соответствия окну

            std::string titleStr = "===" + title + "===";
            float windowWidth = ImGui::GetWindowSize().x;
            float textWidth = ImGui::CalcTextSize(titleStr.c_str()).x;

            float posX = (windowWidth - textWidth) * 0.5f;
            if(posX > 0.0f) ImGui::SetCursorPosX(posX);

            ImGui::TextColored(ImVec4(0.0f, 0.8f, 1.0f, 1.0f), "%s", titleStr.c_str());
            ImGui::Separator();

            renderContent();

            ImGui::PopFont();
        }

        if(ImGui::IsWindowFocused() && ImGui::IsMouseDragging(0)){  // Это условие, нужно, чтобы я мог лично, перевести окно туда-куда я хочу
            ImVec2 mousePos = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
            this->position = camera.screenToWorld(glm::vec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y), windowSize);
        }

        ImGui::End();

        //ImGui::PopStyleVar();
        //ImGui::PopStyleColor();
    }

}