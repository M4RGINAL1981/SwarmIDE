#pragma once
#include "object/memory/BaseMemory.hpp"

namespace SwarmIDE::Object::Memory {

    class PostgreSQL : public BaseMemory {
    public:
        PostgreSQL(std::string name, glm::vec2 worldPos);

        void update() override;
        void drawUI(Camera& camera, glm::vec2 windowSize) override;
    
    private:
    };
}