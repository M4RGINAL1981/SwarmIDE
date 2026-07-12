#pragma once
#include "object/ai/BaseAgent.hpp"

namespace SwarmIDE::Object::AI {

    class PlanExecuteAgent : public BaseAgent {
    public:
        PlanExecuteAgent(const std::string& name);
        
        void Execute() override;

        std::string getTypeName() override { return "ToolsAgent"; };
    };

}