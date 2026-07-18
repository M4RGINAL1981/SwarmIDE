#pragma once
#include "SwarmIDE.hpp"
#include "nodes/base/LogicNode.hpp"
#include <string>

namespace SwarmIDE::Nodes {
    
    class BaseAgent : public LogicNode{
    public:
        BaseAgent(const std::string& name) : LogicNode(name) {};
        virtual ~BaseAgent() = default;

        // Установить и Получить инструкции агента
        std::string getSystemInstruction() { return systemInstruction; }
        void setSystemInstruction(std::string system_instruction) { systemInstruction = system_instruction; }

    private:
        std::string systemInstruction;
    };
}