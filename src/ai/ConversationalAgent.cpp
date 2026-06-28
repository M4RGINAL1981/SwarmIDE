#include "ai/ConversationalAgent.hpp"
#include "imgui.h" 

namespace SwarmIDE::AI {

    void ConversationalAgent::update() {
        //std::cout << "ConversationalAgent " << count << std::endl;
    }

    void ConversationalAgent::drawUI(Camera& camera, glm::vec2 windowSize) {
        auto config = SwarmIDE::UI::Theme::GetAgentConfig();          // Берём параметры из Theme

        glm::vec2 screenPos = camera.worldToScreen(this->worldPos, windowSize); 

        if(!ImGui::IsMouseDragging(0)){
            ImGui::SetNextWindowPos(ImVec2(screenPos.x, screenPos.y), ImGuiCond_Always);
        }
    
        ImGui::SetNextWindowSize(ImVec2(config.size.x * camera.zoomFactor, config.size.y * camera.zoomFactor));

        ImGui::PushStyleColor(ImGuiCol_WindowBg, config.bgColor);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, config.rounding * camera.zoomFactor);

        if(ImGui::Begin(name.c_str(), nullptr, config.flags)) {

            int index = camera.GetCurrentZoomIndex();
            float baseScale =  2.0f * index + 14.0f;

            float targetPixelSize = 18.0f * camera.zoomFactor; // 18px — базовый размер при зуме 1.0
            float currentFontBaseSize = 2.0f * index + 16.0f;  // Размер шрифта в текущем дефолтном индексе атласа
            float fontScale = targetPixelSize / currentFontBaseSize;

            ImGui::PushFont(fonts.regularFont[index]);
            ImGui::SetWindowFontScale(fontScale); // Вот эта херня дотянет размер текста до пиксель-перфект соответствия окну

            std::string titleStr = "===" + name + "===";
            float windowWidth = ImGui::GetWindowSize().x;
            float textWidth = ImGui::CalcTextSize(titleStr.c_str()).x;

            float posX = (windowWidth - textWidth) * 0.5f;
            if(posX > 0.0f) ImGui::SetCursorPosX(posX);

            ImGui::TextColored(ImVec4(0.0f, 0.8f, 1.0f, 1.0f), "%s", titleStr.c_str());
            ImGui::Separator();

            ImGui::Text("Агент: %s", name.c_str()); // Что пишется внутри самого агента
            
            ImGui::PopFont();
        }

        if(ImGui::IsWindowFocused() && ImGui::IsMouseDragging(0)){  // Это условие, нужно, чтобы я мог лично, перевести окно туда-куда я хочу
            ImVec2 mousePos = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
            this->worldPos = camera.screenToWorld(glm::vec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y), windowSize);
        }

        ImGui::End();

        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
    }

    void ConversationalAgent::render() {

    }

    int ConversationalAgent::count = 0;
}