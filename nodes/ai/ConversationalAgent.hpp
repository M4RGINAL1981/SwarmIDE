#pragma once
#include "SwarmIDE.hpp"
#include "nodes/ai/BaseAgent.hpp"

namespace SwarmIDE::Nodes {
    
    /**
     * @class ConversationalAgetn
     * @brief Это класс разговорного агента - он может разговарить
     * 
     * @see SwarmIDE::Nodes::LogicNode
     * @see SwarmIDE::Nodes::BaseAgent
     */
    class ConversationalAgent : public BaseAgent {
    public:
        /**
         * @brief Конструктор разговорного агента 
         * @param name Имя разговорного агента
         */
        ConversationalAgent(const std::string& name);

        /**
         * @brief Отработка разговорного агента
         */
        void Execute() override;

        /**
         * @brief Получить строковый тип ноды
         * @return std::string Строковый тип ноды
         */
        std::string getTypeName() override { return "ConversationalAgent"; }

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

        /**
         * @brief Установить стандартные настройки ноды
         */
        virtual void ApplyDefaultSettings() override;
    };
}