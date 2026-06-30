#include "ui/Shape/RectangleShape.hpp"
#include <iostream>
#include <vector>
#include <string>
#include "imgui.h"


namespace SwarmIDE::UI::Shape {
    
    RectangleShape::RectangleShape() {

        this->size = glm::vec2(800.0f, 400.0f);

        this->localPins = {
            {"0", SwarmIDE::UI::PinType::Universal, glm::vec2(0.25f, 0.0f), glm::vec2(-1.0f, -1.0f)}, 
            {"1", SwarmIDE::UI::PinType::Universal, glm::vec2( 0.5f, 0.0f), glm::vec2( 0.0f, -1.0f)}, 
            {"2", SwarmIDE::UI::PinType::Universal, glm::vec2(0.75f, 0.0f), glm::vec2( 1.0f, -1.0f)}, 
            {"3", SwarmIDE::UI::PinType::Universal, glm::vec2( 1.0f, 0.5f), glm::vec2( 1.0f,  0.0f)}, 
            {"4", SwarmIDE::UI::PinType::Universal, glm::vec2(0.75f, 1.0f), glm::vec2( 1.0f,  1.0f)}, 
            {"5", SwarmIDE::UI::PinType::Universal, glm::vec2( 0.5f, 1.0f), glm::vec2( 0.0f,  1.0f)}, 
            {"6", SwarmIDE::UI::PinType::Universal, glm::vec2(0.25f, 1.0f), glm::vec2(-1.0f,  1.0f)}, 
            {"7", SwarmIDE::UI::PinType::Universal, glm::vec2( 0.0f, 0.5f), glm::vec2(-1.0f,  0.0f)}  
        };

        count++;
    }

    RectangleShape::RectangleShape(glm::vec2 pos) : BaseShape(pos) {
        this->size = glm::vec2(800.0f, 400.0f);

        this->localPins = {
            {"0", SwarmIDE::UI::PinType::Universal, glm::vec2(0.25f, 0.0f), glm::vec2(-1.0f, -1.0f)}, 
            {"1", SwarmIDE::UI::PinType::Universal, glm::vec2( 0.5f, 0.0f), glm::vec2( 0.0f, -1.0f)}, 
            {"2", SwarmIDE::UI::PinType::Universal, glm::vec2(0.75f, 0.0f), glm::vec2( 1.0f, -1.0f)}, 
            {"3", SwarmIDE::UI::PinType::Universal, glm::vec2( 1.0f, 0.5f), glm::vec2( 1.0f,  0.0f)}, 
            {"4", SwarmIDE::UI::PinType::Universal, glm::vec2(0.75f, 1.0f), glm::vec2( 1.0f,  1.0f)}, 
            {"5", SwarmIDE::UI::PinType::Universal, glm::vec2( 0.5f, 1.0f), glm::vec2( 0.0f,  1.0f)}, 
            {"6", SwarmIDE::UI::PinType::Universal, glm::vec2(0.25f, 1.0f), glm::vec2(-1.0f,  1.0f)}, 
            {"7", SwarmIDE::UI::PinType::Universal, glm::vec2( 0.0f, 0.5f), glm::vec2(-1.0f,  0.0f)}
        };

        count++;
    }

    void RectangleShape::Draw(const std::string& title, Camera& camera, Fonts& fonts, glm::vec2 windowSize, std::function<void()> renderContent) {
        auto config = SwarmIDE::UI::Theme::GetAgentConfig();          // Берём параметры из Theme

        glm::vec2 screenPos = camera.worldToScreen(this->position, windowSize); 

        if(!ImGui::IsMouseDragging(0)){
            ImGui::SetNextWindowPos(ImVec2(screenPos.x, screenPos.y), ImGuiCond_Always);
        }
    
        ImGui::SetNextWindowSize(ImVec2(size.x * camera.zoomFactor, size.y * camera.zoomFactor));

        ImGui::PushStyleColor(ImGuiCol_WindowBg, config.bgColor);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, config.rounding * camera.zoomFactor);

        if(ImGui::Begin((title).c_str(), nullptr, config.flags)) {

            int index = camera.GetCurrentZoomIndex();
            float baseScale =  2.0f * index + 14.0f;

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

        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
    }

    int RectangleShape::count = 0;
}