#pragma once
#include "object/ai/BaseAgent.hpp"

namespace SwarmIDE::Object::AI {

    class ToolsAgent : public BaseAgent {
    public:
        ToolsAgent(const std::string& name);
        
        void Execute() override;

        std::string getTypeName() override { return "ToolsAgent"; };
    };

}