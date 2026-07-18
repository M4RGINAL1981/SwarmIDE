#pragma once
#include "core/graph/nodes/LogicNode.hpp"
#include <string>

namespace SwarmIDE::Object::AI {
    
    class BaseAgent : public Core::Graph::LogicNode{
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