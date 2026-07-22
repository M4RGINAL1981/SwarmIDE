#pragma once
#include "SwarmIDE.hpp"
#include "nodes/ai/BaseAgent.hpp"

namespace SwarmIDE::Nodes {

    class ToolsAgent : public BaseAgent {
    public:
        ToolsAgent(const std::string& name);
        
        void Execute() override;

        std::string getTypeName() override { return "ToolsAgent"; };

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
    };

}