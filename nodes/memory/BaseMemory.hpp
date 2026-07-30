#pragma once 
#include "SwarmIDE.hpp"
#include "nodes/base/LogicNode.hpp"

namespace SwarmIDE::Nodes {

    /**
     * @class BaseMemory
     * @brief Данный класс подключает память для агента
     */
    class BaseMemory : public LogicNode  {
    public:
        BaseMemory(std::string name) : LogicNode(name) {};
        virtual ~BaseMemory() = default;
    protected:

    };
}