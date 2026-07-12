#pragma once
#include "object/memory/BaseMemory.hpp"

namespace SwarmIDE::Object::Memory {

    class PostgreSQL : public BaseMemory {
    public:
        PostgreSQL(std::string name);

        void Execute() override;
        std::string getTypeName() override { return "PostgreSQL"; };
    private:
    };
}