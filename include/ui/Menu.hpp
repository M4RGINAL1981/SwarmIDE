#pragma once
#include "SwarmIDE.hpp"
#include "core/ObjectManager.hpp" 
#include "object/ai/ConversationalAgent.hpp"
#include "object/ai/PlanExecuteAgent.hpp"
#include "object/ai/ToolsAgent.hpp"
#include "graphics/Camera.hpp"

namespace SwarmIDE::UI {
    class Menu {
    public:
        
        static Menu& getInstance(); 

        void Push(std::unique_ptr<SwarmIDE::Object::AI::BaseAgent> agent);
        void SetManager(SwarmIDE::Core::ObjectManager* mgr) { manager = mgr; }
        void DrawMenu(Camera& camera);
        void GetWinSize(glm::vec2 otherWinSize) { winSize = otherWinSize; }
        void SetFonts(const SwarmIDE::UI::Fonts& fontsAtlas) { this->fonts = fontsAtlas; }

    private:
        Menu() = default;
        SwarmIDE::Core::ObjectManager* manager = nullptr;
        glm::vec2 winSize;
        SwarmIDE::UI::Fonts fonts;
    };
}