#pragma once
#include "object/ai/BaseAgent.hpp"


namespace SwarmIDE::Object::AI {
    
    class ConversationalAgent :  public SwarmIDE::Object::AI::BaseAgent {
    public:
        ConversationalAgent(std::string name, glm::vec2 worldPos);

        void update() override;
        void drawUI(Camera& camera, glm::vec2 windowSize) override;
        void render() override;
    };
}