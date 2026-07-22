#pragma once
#include "SwarmIDE.hpp"
#include "nodes/base/LogicNode.hpp"

namespace SwarmIDE::Nodes {

    class MergeNode : public LogicNode {
    public:
        MergeNode(const std::string& nodeName);                                     // Конструктор
        virtual ~MergeNode() = default;                                             // Деструктор 

        /**
         * @brief Выгрузка ноды в json
         * @return json 
         */
        virtual json serializeNode() override;

        /**
         * @brief Загрузка ноды из json
         * @param[in] jsonNode 
         */
        virtual void deserializeNode(json jsonNode) override;
        
        virtual void Execute() override;                                            // Логика ноды
        virtual std::string getTypeName() override { return "MergeNode"; }          // Вернуть тип ноды
    };
}