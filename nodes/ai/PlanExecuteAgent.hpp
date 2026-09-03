#pragma once
#include "SwarmIDE.hpp"
#include "nodes/ai/BaseAgent.hpp"

namespace SwarmIDE::Nodes {

    /**
     * @class PlanExecuteAgent
     * @brief Агент планирования и выполнения
     * 
     * @see SwarmIDE::Nodes::LogicNode
     * @see SwarmIDE::Nodes::BaseAgent
     */
    class PlanExecuteAgent : public BaseAgent {
    public:
        /**
         * @brief Класс Агента планировани и выполнения 
         * @param name Имя агента
         */
        PlanExecuteAgent(const std::string& name);
        
        /**
         * @brief Отработка агента
         */
        void Execute() override;

        /**
         * @brief Выдать строковый тип ноды
         * @return std::string Строковый тип ноды
         */
        std::string getTypeName() override { return "PlanExecuteAgent"; };
        
        /**
         * @brief Устанвоить стандартные настройки ноды
         */
        virtual void ApplyDefaultSettings() override;
    };

}