#pragma once 
#include "core/graph/LogicNode.hpp"
#include "core/ModelManager.hpp"
#include "llama.h"

namespace SwarmIDE::Object {

    class BaseModel : public SwarmIDE::Core::Graph::LogicNode {
    public:
        BaseModel(std::string name) : LogicNode(name) {};

           
        std::string getTypeName() override { return "Model"; };                      // Вернуть тип объекта
        std::string GetModelID() const { return model ? model->id : ""; }            // Вернуть ID модели
        
    private:
        llama_model* m_model = nullptr;          
        llama_context* m_ctx = nullptr;
        bool m_isModelLoaded = false;        

        SwarmIDE::Core::Model* model = nullptr;
    };
}