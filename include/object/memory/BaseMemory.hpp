#pragma once 
#include "core/graph/LogicNode.hpp"

namespace SwarmIDE::Object::Memory {

    class BaseMemory : public Core::Graph::LogicNode  {
    public:
        BaseMemory(std::string name) : LogicNode(name) {};
        virtual ~BaseMemory() = default;


    protected:

    };
}