#pragma once
#include "object/BaseObject.hpp"


namespace SwarmIDE::Object::AI {
    
    class BaseAgent : public BaseObject {
    public:
        BaseAgent(std::string name, glm::vec2 worldPos): BaseObject(name, worldPos) {};
        virtual ~BaseAgent() = default;
        virtual void render() = 0;   // Запуск
    };
}