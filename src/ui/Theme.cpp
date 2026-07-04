#include "ui/Theme.hpp"
#include <iostream>

namespace SwarmIDE::UI {

    void Theme::ApplyGlobalStyle() {
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 10.0f;           // Закруглённые углы глобально
        style.WindowBorderSize = 0.0f;          // Размер бортиков
    }

    AgentWindowConfig Theme::GetAgentConfig() {
        return {
            ImVec4(0.12f, 0.12f, 0.12f, 1.0f),                       // Цвет фона (тёмно-серый)
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus, // Флаги
            30.0f                                                    // Закругление
        };
    }

    void Theme::BuildFontAtlas(std::vector<ImFont*>& fontVector, ImGuiIO& io, const std::string& filepath, const ImWchar* ranges) {
        std::vector<float> ScaleVector = {16.0f, 18.0f, 20.0f, 22.0f, 24.0f,
                                          26.0f, 28.0f, 30.0f, 32.0f, 34.0f,
                                          36.0f, 38.0f, 40.0f, 42.0f, 44.0f,
                                          46.0f, 48.0f, 50.0f};

        for(auto scale : ScaleVector) {
            fontVector.push_back(io.Fonts->AddFontFromFileTTF(filepath.c_str(), scale, nullptr, ranges));
        }
    }

}