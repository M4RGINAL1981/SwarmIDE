#include "core/ModelManager.hpp"


namespace SwarmIDE::Core {

    ModelManager& ModelManager::getInstance() {
        static ModelManager instance;
        return instance;
    }

    ModelManager::ModelManager() {

        /*
        struct Model {                         // Структура модели

                std::string name;                  // Имя модели
                std::string path;                  // Строковой путь до модели
                std::string iconPath;              // Путь до иконки модели

                std::string id;                    // ID модели
        
                std::string downloadUrl;           // Url для скачивания модели
                bool isDownloaded = false;         // Переменная показывающая скачана ли модель или нет
                bool isDownloading = false;        // Скачивается ли сейчас модель или нет
                float progress = 0.0f;             // Процесс загрузки модели (от 0.0f до 1.0f)

            };
        */


        models = {
            {
                "DeepSeek R1", 
                "resources/models/DeepSeek-R1-Distill-Qwen-1.5B-Q5_K_M.gguf", 
                "resources/icons/deepseek.png",
                "deepseek-7b",
                "https://huggingface.co/TheBloke/DeepSeek-LLM-7B-Chat-GGUF/resolve/main/deepseek-llm-7b-chat.Q4_K_M.gguf"
            },
            {
                "Gemma 2B", 
                "resources/models/gemma-2b.gguf", 
                "resources/icons/gemma.png",
                "gemma-2b",
                "https://huggingface.co/google/gemma-2b-it-GGUF/resolve/main/gemma-2b-it.Q4_K_M.gguf"
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
    
    Model& ModelManager::GetModelForID(std::string id) {
        for(auto& model : models){
            if(model.id == id) return model;
        }
        throw ModelNotFoundException(id, "Model Not Found");
    }

    bool ModelManager::isDownloadedModelForID(std::string id) {
        Model& modelData = GetModelForID(id);
        modelData.isDownloaded = std::filesystem::exists(modelData.path);
        if(!modelData.isDownloaded) return false;
        return true;
    }

    void ModelManager::downloadModelForID(std::string id) {
        Model& modelData = GetModelForID(id);
        modelData.isDownloading = true;

        modelData.isDownloaded = std::filesystem::exists(modelData.path);
        if(modelData.isDownloaded) return;
        
        HRESULT hr = URLDownloadToFileA(NULL, modelData.downloadUrl.c_str(), modelData.path.c_str(), 0, NULL);

        if (SUCCEEDED(hr)) { 
            std::cout << "Загрузка завершена успешно!!!" << std::endl;
            modelData.isDownloaded = true;    
        }
        else {
            std::cout << "Загрузка завершена ошибкой:(" << std::endl;
            //pfd::message("Error", "Failed to download " + modelData.name, pfd::choice::ok);
        }

        modelData.isDownloading = false;
    }
    
}