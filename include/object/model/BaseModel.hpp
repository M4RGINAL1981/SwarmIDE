#pragma once 
#include "object/BaseObject.hpp"

namespace SwarmIDE::Object::Model {

    class BaseModel : public SwarmIDE::Object::BaseObject {
    public:
        BaseModel(std::string name, glm::vec2 worldPos) : BaseObject(name, worldPos) {};
    protected:

    };

    /*
    */
}