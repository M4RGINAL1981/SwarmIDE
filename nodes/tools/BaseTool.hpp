#pragma once 
#include "SwarmIDE.hpp"
#include "nodes/base/LogicNode.hpp"

namespace SwarmIDE::Nodes {

    class BaseTool : public LogicNode  {
    public:
        BaseTool(std::string name) : LogicNode(name) {};
        virtual ~BaseTool() = default;


    protected:

    };
}