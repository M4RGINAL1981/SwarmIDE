#pragma once 
#include "object/BaseObject.hpp"

namespace SwarmIDE::Object::Memory {

    class BaseMemory : public SwarmIDE::Object::BaseObject {
    public:
        BaseMemory(std::string name, glm::vec2 worldPos) : BaseObject(name, worldPos) {};
    protected:

    };
}