#pragma once
#include "SwarmIDE.hpp"
#include "nodes/base/LogicNode.hpp"

namespace SwarmIDE::Nodes {

    /**
     * @class MergeNode
     * @brief Соединяющая нода
     * @details Собирает данные из входных пинов и возвращает объединённый результат
     * 
     * @see SwarmIDE::Nodes::LogicNode
     * @see SwarmIDE::Nodes::BaseAgent
     */
    class MergeNode : public LogicNode {
    public:
        /**
         * @brief Конструктор соединяющий ноды
         * 
         * @param nodeName 
         */
        MergeNode(const std::string& nodeName);                                    

        /**
         * @brief Деструктор соединяющей ноды
         */
        virtual ~MergeNode() = default;         

        /**
         * @brief Логика ноды, забирает данные из входных пинов и возвращает объединённый результат
         */
        virtual void Execute() override;                                        

        /**
         * @brief Выгрузка ноды в json
         * @return json 
         */
        virtual json serializeNode() override { return LogicNode::serializeNode(); }

        /**
         * @brief Загрузка ноды из json
         * @param[in] jsonNode 
         */
        virtual void deserializeNode(json jsonNode) override { LogicNode::deserializeNode(jsonNode); }
        
        /**
         * @brief Вернуть строковый тип ноды
         * @return std::string 
         */
        virtual std::string getTypeName() override { return "MergeNode"; }       
        
        /**
         * @brief Применить стандартные настройки для ноды
         */
        virtual void ApplyDefaultSettings() override;
    };
}