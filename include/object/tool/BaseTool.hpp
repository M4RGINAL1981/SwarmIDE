#pragma once 
#include "core/graph/nodes/LogicNode.hpp"

namespace SwarmIDE::Object::Tool {

    class BaseTool : public Core::Graph::LogicNode  {
    public:
        BaseTool(std::string name) : LogicNode(name) {};
        virtual ~BaseTool() = default;


    protected:

    };
}