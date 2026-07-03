#pragma once
#include "object/ai/BaseAgent.hpp"


namespace SwarmIDE::Object::AI {

    class ToolsAgent : public SwarmIDE::Object::AI::BaseAgent {
    public:
        ToolsAgent(std::string name, glm::vec2 worldPos);
        
        void update() override;
        std::optional<bool> drawUI(Camera& camera, glm::vec2 windowSize) override;
        void render() override;

    };

}