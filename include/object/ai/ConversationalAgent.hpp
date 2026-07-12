#pragma once
#include "object/ai/BaseAgent.hpp"

namespace SwarmIDE::Object::AI {
    
    class ConversationalAgent : public BaseAgent {
    public:
        ConversationalAgent(const std::string& name);

        void Execute() override;

        std::string getTypeName() override { return "ConversationalAgent"; }
    };
}