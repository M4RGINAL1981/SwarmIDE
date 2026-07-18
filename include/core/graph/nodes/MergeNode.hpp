#pragma once
#include "core/graph/nodes/LogicNode.hpp"

namespace SwarmIDE::Core::Graph {

    class MergeNode : public LogicNode {
    public:
        MergeNode(const std::string& nodeName);                                     // Конструктор
        virtual ~MergeNode() = default;                                             // Деструктор 
        
        virtual void Execute() override;                                            // Логика ноды
        virtual std::string getTypeName() override { return "MergeNode"; }          // Вернуть тип ноды
    };
}