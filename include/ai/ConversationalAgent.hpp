#pragma once
#include "BaseAgent.hpp"


namespace SwarmIDE::AI {
    
    class ConversationalAgent : public SwarmIDE::AI::BaseAgent {
    public:
        ConversationalAgent(std::string name, glm::vec2 worldPos) : BaseAgent(name + std::to_string(count), worldPos) { count++; std::cout << "ConversationalAgent" << std::endl; }

        void update() override;
        void drawUI(Camera& camera, glm::vec2 windowSize) override;
        void render() override;
    private:
        static int count;
    };
}