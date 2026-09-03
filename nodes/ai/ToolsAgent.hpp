#pragma once
#include "SwarmIDE.hpp"
#include "nodes/ai/BaseAgent.hpp"

namespace SwarmIDE::Nodes {

    /**
     * @class ToolsAgent
     * @brief Агент использующий инструменты
     * 
     * @see SwarmIDE::Nodes::LogicNode
     * @see SwarmIDE::Nodes::BaseAgent
     */
    class ToolsAgent : public BaseAgent {
    public:
        /**
         * @brief Конструктор агента с интрументами
         * @param name Имя агента
         */
        ToolsAgent(const std::string& name);
        
        /**
         * @brief Отработка агента с инструментами
         */
        void Execute() override;

        /**
         * @brief Вернуть строковый тип ноды
         * @return std::string Строковый тип ноды
         */
        std::string getTypeName() override { return "ToolsAgent"; };
    
        /**
         * @brief Установить стандартные настройки ноды
         */
        virtual void ApplyDefaultSettings() override;
    };

}