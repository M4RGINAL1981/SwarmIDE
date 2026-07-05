#pragma once
#include "object/model/BaseModel.hpp"

namespace SwarmIDE::Object::Model {

    class DeepSeek : public BaseModel {
    public:
        DeepSeek(std::string name, glm::vec2 worldPos);

        void update() override;
        std::optional<bool> drawUI(Camera& camera, glm::vec2 windowSize) override;
        
        std::string GetTypeName() override { return "DeepSeek"; };
        
    private:
    };
}