#pragma once
#include "SwarmIDE.hpp"
#include "nodes/memory/BaseMemory.hpp"

namespace SwarmIDE::Nodes {

    class PostgreSQL : public BaseMemory {
    public:
        PostgreSQL(std::string name);

        void Execute() override;
        std::string getTypeName() override { return "PostgreSQL"; };
    private:
    };
}