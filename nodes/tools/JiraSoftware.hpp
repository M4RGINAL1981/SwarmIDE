#pragma once
#include "SwarmIDE.hpp"
#include "nodes/tools/BaseTool.hpp"

namespace SwarmIDE::Nodes {

    class JiraSoftware : public BaseTool {
    public:
        JiraSoftware(std::string name);
        
        void Execute() override;
        std::string getTypeName() override { return "JiraSoftware"; };
    private:


    };
}