#pragma once
#include "SwarmIDE.hpp"
#include "core/AgentManager.hpp" 
#include "ai/ConversationalAgent.hpp"
#include "ai/PlanExecuteAgent.hpp"
#include "ai/ToolsAgent.hpp"
#include "graphics/Camera.hpp"

namespace SwarmIDE::UI {
    class Menu {
    public:
        
        static Menu& getInstance(); 

        void Push(std::unique_ptr<SwarmIDE::AI::BaseAgent> agent);
        void SetManager(SwarmIDE::Core::AgentManager* mgr) { manager = mgr; }
        void DrawMenu(Camera& camera);
        void GetWinSize(glm::vec2 otherWinSize) { winSize = otherWinSize; }
        void SetFonts(const SwarmIDE::UI::Fonts& fontsAtlas) { this->fonts = fontsAtlas; }

    private:
        Menu() = default;
        SwarmIDE::Core::AgentManager* manager = nullptr;
        glm::vec2 winSize;
        SwarmIDE::UI::Fonts fonts;
    };
}