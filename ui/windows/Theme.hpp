#pragma once
#include "SwarmIDE.hpp"
#include "imgui.h"
#include <iostream>
#include <vector>
#include <string>

namespace SwarmIDE::UI {

    struct AgentWindowConfig {
        ImVec4 bgColor;
        ImGuiWindowFlags flags;
        float rounding;
    };

    struct Fonts {
        std::vector<ImFont*> regularFont;
        std::vector<ImFont*> boldFont;
        std::vector<ImFont*> italicFont;
    };

    class Theme { // Класс-хелпер
    public:
        static void ApplyGlobalStyle(); // Для глобальных настроек (шрифты, закругления)
        static AgentWindowConfig GetAgentConfig(); // Для получения параметров конкретного окна
        static void BuildFontAtlas(std::vector<ImFont*>& fontVector, ImGuiIO& io, const std::string& filepath, const ImWchar* ranges);
    };

}