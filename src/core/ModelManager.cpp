#include "core/ModelManager.hpp"

namespace SwarmIDE::Core {

    ModelManager& ModelManager::getInstance() {
        static ModelManager instance;
        return instance;
    }

    ModelManager::ModelManager() {
        models = {
            {
                "DeepSeek 7B", 
                "models/deepseek-7b.gguf", 
                "resources/icons/deepseek.png",
                "https://huggingface.co/TheBloke/DeepSeek-LLM-7B-Chat-GGUF/resolve/main/deepseek-llm-7b-chat.Q4_K_M.gguf",
                false,
                "deepseek-7b"
            },
            {
                "Gemma 2B", 
                "models/gemma-2b.gguf", 
                "resources/icons/gemma.png",
                "https://huggingface.co/google/gemma-2b-it-GGUF/resolve/main/gemma-2b-it.Q4_K_M.gguf",
                false,
                "gemma-2b"
            }
        };
    }

    std::vector<std::string> ModelManager::GetNamesModels() {
        std::vector<std::string> namesModels;
        for(auto& model : models) {
            namesModels.push_back(model.name);
        }
        return namesModels;
    }
    
    Model* ModelManager::GetModelForID(std::string id) {
        for(auto& model : models){
            if(model.id == id) return &model;
        }
        return nullptr;
    }

    bool ModelManager::downloadModelForID(std::string id) {
        Model* model = GetModelForID(std::string id);
        if(model == nullptr) {
            return false;
            throw ModelNotFoundException(id, "Model Not Found Exception");
        }

        

    }
    
}