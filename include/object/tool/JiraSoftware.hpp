#pragma once
#include "object/tool/BaseTool.hpp"

namespace SwarmIDE::Object::Tool {

    class JiraSoftware : public BaseTool {
    public:
        JiraSoftware(std::string name, glm::vec2 worldPos);

        void update() override;
        std::optional<bool> drawUI(Camera& camera, glm::vec2 windowSize) override;
    
    private:
    };
}