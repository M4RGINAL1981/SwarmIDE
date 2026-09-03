#pragma once
#include "SwarmIDE.hpp"
#include "nodes/base/LogicNode.hpp"
#include <string>

namespace SwarmIDE::Nodes {
    
    /**
     * @class BaseAgent
     * @brief Это базовый/виртуальный класс агента
     * 
     * @see SwarmIDE::Nodes::LogicNode
     */
    class BaseAgent : public LogicNode{
    public:
        /**
         * @brief Конструктор базового/виртуального агента
         * @param name Имя агента
         */
        BaseAgent(const std::string& name) : LogicNode(name) {};

        /**
         * @brief Деструктор базового/виртуального агента
         */
        virtual ~BaseAgent() = default;

        /**
         * @brief Получить системные настройки агента
         * @return std::string Системные настройки агента
         */
        std::string getSystemInstruction() { return systemInstruction; }

        /**
         * @brief Установить системные настройки агента
         * @param system_instruction Строка содержащая системные настройки агента
         */
        void setSystemInstruction(std::string system_instruction) { systemInstruction = system_instruction; }

        /**
         * @brief 
         * @return std::vector<Core::PropertyDescriptor> 
         */
        virtual std::vector<Core::PropertyDescriptor> properties() override;

        json serializeNode();          
        void deserializeNode(const json&);

    private:
        std::string systemInstruction; /** @brief < Системные настройки агента */
    };
}