#pragma once 
#include "object/BaseObject.hpp"
#include "core/ModelManager.hpp"
#include "llama.h"

namespace SwarmIDE::Object {

    class BaseModel : public SwarmIDE::Object::BaseObject {
    public:
        BaseModel(std::string name, glm::vec2 worldPos, SwarmIDE::Core::Model* model = nullptr);

        void update() override;
        std::optional<bool> drawUI(Camera& camera, glm::vec2 windowSize) override;   
        std::string GetTypeName() override { return "Model"; };                      // Вернуть тип объекта
        std::string GetModelID() const { return model ? model->id : ""; }            // Вернуть ID модели
        
    private:
        llama_model* m_model = nullptr;          
        llama_context* m_ctx = nullptr;
        bool m_isModelLoaded = false;        

        SwarmIDE::Core::Model* model = nullptr;
    };
}