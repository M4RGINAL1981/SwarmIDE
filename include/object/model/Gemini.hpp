#pragma once
#include "object/model/BaseModel.hpp"

namespace SwarmIDE::Object::Model {

    class Gemini : public BaseModel {
    public:
        Gemini(std::string name, glm::vec2 worldPos);

        void update() override;
        std::optional<bool> drawUI(Camera& camera, glm::vec2 windowSize) override;
        
        std::string GetTypeName() override { return "Gemini"; };
        
    private:
    };
}