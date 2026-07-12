#pragma once
#include "object/tool/BaseTool.hpp"

namespace SwarmIDE::Object::Tool {

    class JiraSoftware : public BaseTool {
    public:
        JiraSoftware(std::string name);
        
        void Execute() override;
        std::string getTypeName() override { return "JiraSoftware"; };
    private:


    };
}