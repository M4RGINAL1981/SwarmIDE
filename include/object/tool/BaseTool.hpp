#pragma once 
#include "object/BaseObject.hpp"

namespace SwarmIDE::Object::Tool {

    class BaseTool : public SwarmIDE::Object::BaseObject {
    public:
        BaseTool(std::string name, glm::vec2 worldPos) : BaseObject(name, worldPos) {};
    protected:

    };
}