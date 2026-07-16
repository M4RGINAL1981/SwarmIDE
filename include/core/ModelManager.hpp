#pragma once
#include "core/ModelAsset.hpp"
#include <vector>
#include <string>
#include <memory>
#include <functional>

// Предварительное объявление типов llama.cpp
struct llama_model;
struct llama_context;
struct llama_vocab;

namespace SwarmIDE::Core {

    namespace Graph { class ModelNode; }
              
    // Структура настроек генерации (передается из ноды)               
    struct GenerationConfig { 
        int n_predict = 512;                                                                         // Максимум генерируемых токенов за один раз
        float temperature = 0.7f;                                                                    // Креативность (0.0 - точный ответ, 1.0 - случайный)
        float top_p = 0.9f;                                                                          // Отсечение маловероятных слов (Nucleus sampling)
        
        uint32_t seed = 42;                                                                          // Зерно генерации (-1 для случайных ответов)
        int n_threads = 4;                                                                           // Потоки CPU (1 слишком медленно, 4 — хороший старт)
        std::vector<std::string> anti_prompts = {"User:", "Observation:"};                           // Стоп-слова (лучше с двоеточием)
        int n_ctx = 2048;                                                                            // Размер окна памяти (2048 токенов — стандарт)

        GenerationConfig& createDefaultGenerationConfig() { GenerationConfig gC; return gC; };
    };

    // Структура для загрузки модели в Память 
    struct ActiveModel {
        struct llama_model* model = nullptr;                                                         // Загруженные веса модели
        struct llama_context* ctx = nullptr;                                                         // Рабочая область (память вычислений)
        const struct llama_vocab* vocab = nullptr;                                                   // Словарь (перевод текста в числа и обратно)
        
        int ctx_size = 0;                                                                            // Полный размер доступного контекста
        int tokens_past = 0;                                                                         // Счетчик использованных токенов (для истории)
        
        GenerationConfig config;                                                                     // Конфиг начальных настроек

        bool isValid() const;                                                                        // Безопасна ли модель для использования
    };

    class ModelManager {
    public:
        static ModelManager& getInstance();

        // --- API для UI (Управление ассетами и загрузкой) ---
        const std::vector<std::unique_ptr<ModelAsset>>& getAllAssets() const;                        // Получить все ассеты
        ModelAsset* getAssetByID(const std::string& id);                                             // Получить ассет по id
        
        // Запуск асинхронной загрузки (внутри создает std::thread, не блокирует UI!)
        void startAsyncDownload(const std::string& id, std::function<void(bool)> callback = nullptr);

        bool checkModelAssetInFolder(const std::string& id);                                         // Проверка загружена ли модель в папку или нет
        bool checkAllModelAssetInFolder();                                                           // Провека загружены ли все модели в папку или нет

        bool loadModelToMemory(const std::string& id);                                               // Загрузка модели в память по ID
        void unloadAllModel();                                                                       // Выгрузка всех моделей из памяти
        void unloadModelByID(const std::string& id);                                                 // Выгрузка модели из памяти по ID

        // Синхронная/Асинхронная генерация текста
        std::string generateResponse(const std::string& modelId, 
                                     const std::string& prompt, 
                                     const std::string& systemInstruction);

        ModelManager(const ModelManager&) = delete;
        ModelManager& operator=(const ModelManager&) = delete;
        ModelManager(ModelManager&&) = delete;
        ModelManager& operator=(ModelManager&&) = delete;

    private:
        ModelManager();
        ~ModelManager();

        std::string modelsPath = "resources/models/models.json";                            // Путь до всех модлей

        std::vector<std::unique_ptr<ModelAsset>> registry;                                           // Список всех зарегистрированных моделей
        std::unordered_map<std::string, ActiveModel> activeModels;                                   // Активные в данный момент модели
    };
}