#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <core/ModelManager.hpp>
#include "core/graph/nodes/ModelNode.hpp"
#include <fstream>
#include <filesystem>               // Для проверки - Есть ли модель по указаному пути
#include <iostream>
#include <json.hpp>                 // Для загрузки образов модели
#include <cerrno>
#include <cstring>
#include <thread>
#include <llama.h>
#include <cpr/cpr.h>

using json = nlohmann::json;

// Пустая функция обратного вызова, которая просто игнорирует все сообщения от llama.cpp
void null_llama_log_callback(ggml_log_level level, const char* text, void* user_data) {}

namespace SwarmIDE::Core {
    /*
    // Структура Ассета модели
    struct ModelAsset {
        std::string id;                                                     // ID модели
        std::string name;                                                   // Имя модели
        std::string filepath;                                               // Путь до самой модели
        std::string fileIcon;                                               // Путь до иконки
        std::string downloadUrl;                                            // URL для скачивания

        // Потокобезопасные флаги для UI
        std::atomic<bool> isDownloaded{false};                              // Скачана ли модель
        std::atomic<bool> isDownloading{false};                             // Качается ли модель прямо сейчас
        std::atomic<float> progress{0.0f};                                  // Прогресс от 0.0f до 1.0f
    };     
    
    // Структура настроек генерации (передается из ноды)               
    struct GenerationConfig { 
        int n_predict = 512;                                                // Максимум генерируемых токенов за один раз
        float temperature = 0.7f;                                           // Креативность (0.0 - точный ответ, 1.0 - случайный)
        float top_p = 0.9f;                                                 // Отсечение маловероятных слов (Nucleus sampling)
        
        uint32_t seed = 42;                                                 // Зерно генерации (-1 для случайных ответов)
        int n_threads = 4;                                                  // Потоки CPU (1 слишком медленно, 4 — хороший старт)
        std::vector<std::string> anti_prompts = {"User:", "Observation:"};  // Стоп-слова (лучше с двоеточием)
        int n_ctx = 2048;                                                   // Размер окна памяти (2048 токенов — стандарт)
    };

    // Структура для загрузки модели в Память 
    struct ActiveModel {
        struct llama_model* model = nullptr;                                // Загруженные веса модели
        struct llama_context* ctx = nullptr;                                // Рабочая область (память вычислений)
        const struct llama_vocab* vocab = nullptr;                          // Словарь (перевод текста в числа и обратно)
        
        int ctx_size = 0;                                                   // Полный размер доступного контекста
        int tokens_past = 0;                                                // Счетчик использованных токенов (для истории)

        bool isValid() const;                                               // Безопасна ли модель для использования
    };
    */

    bool ActiveModel::isValid() const {
        return model != nullptr && ctx != nullptr && vocab != nullptr;
    }

    ModelManager& ModelManager::getInstance() {
        static ModelManager instance;
        return instance;
    }

    ModelManager::ModelManager() { 
    /*
        Регистрация ассетов моделей в регистор
    */
                        
        std::cout << "fulling registry model" << std::endl;

        llama_log_set(null_llama_log_callback, nullptr);    // Отключение логов

        llama_backend_init();                               // Иницилизация llama

        std::ifstream modelsFile(modelsPath);
        if (!modelsFile.is_open()) {
            std::cerr << "LoadModel Error: " << std::strerror(errno) << " (Path: " << modelsPath << ")" << std::endl;
            return;
        }

        json root;
        modelsFile >> root;
        
        if(root.contains("models")) {
            for (const auto& model : root["models"]) {

                std::string id          = model["id"];
                std::string name        = model["name"];
                std::string filepath    = model["filepath"];
                std::string fileIcon    = model["fileIcon"];
                std::string downloadUrl = model["downloadUrl"];

                std::cout << "Model: " << name << " is add in asset" << std::endl; // Вывод имени модели

                auto newModelAssetPtr = std::make_unique<ModelAsset>();
                newModelAssetPtr->id          = id;
                newModelAssetPtr->name        = name;
                newModelAssetPtr->filepath    = filepath;
                newModelAssetPtr->fileIcon    = fileIcon;
                newModelAssetPtr->downloadUrl = downloadUrl;

                // Флаги атомиков по умолчанию проинициализируются сами внутри структуры

                registry.push_back(std::move(newModelAssetPtr));
            }
        }
    }

    ModelManager::~ModelManager() {
    /*
        Выгрузка моделей из памяти
        Очистка регистра моделей
        Освобождение глобальных ресурсов библиотеки 
    */
        unloadAllModel();      // Очишаем память от моделей
        registry.clear();      // Очишаем регистр от моделей

        llama_backend_free();  // Освобождает глобальные ресурсы библиотеки llama
    }
    
   
    
    const std::vector<std::unique_ptr<ModelAsset>>& ModelManager::getAllAssets() const { return registry; }

    ModelAsset* ModelManager::getAssetByID(const std::string& id) {
    /*
        Получить ассет модели по ID

        Принимает ID accета. При не находе - возвращает nullptr
    */
        for(auto& asset : registry) {
            if(asset->id == id)  return asset.get();  
        }
        return nullptr;
    }

    void ModelManager::startAsyncDownload(const std::string& id, std::function<void(bool)> callback) {
    /*
        Запуск асинхронной загрузки модели по её URL
    */
        auto downloadTask = [this, id, callback]() {
            bool success = false;

            // Проверка: Иницилизирован ли сейчас ассет или нет?
            auto asset = getAssetByID(id);        
            if(!asset) {
                if (callback) callback(false); 
                return;
            }

            checkModelAssetInFolder(id);

            // Проверка: Скачана ли сейчас модель или нет?
            if(asset->isDownloaded == true || asset->isDownloading == true) {         
                if(callback) callback(false);
                return;
            }

            // Переводим ассет в режим загрузки и сбрасываем прогресс
            asset->isDownloading = true;
            asset->progress = 0.0f;

            // Выводим лог о том, что модель скачивается
            std::cout << "Model: " << asset->name << " is downloading" << std::endl;

            // Открываем файл для записи в бинарном режиме
            std::ofstream file(asset->filepath, std::ios::binary);
            if (!file.is_open()) {
                asset->isDownloading = false;
                if(callback) callback(false);
                return;
            }

            // 1. Лямбда для записи (теперь типы строго соответствуют cpr::WriteCallback)
            auto write_func = [&file](std::string_view data, intptr_t userdata) -> bool {
                file.write(data.data(), data.size());
                return file.good();
            };

            // 2. Лямбда для прогресса (тут типы cpr::cpr_off_t)
            auto progress_func = [asset](cpr::cpr_off_t downloadTotal, cpr::cpr_off_t downloadNow, 
                                        cpr::cpr_off_t uploadTotal, cpr::cpr_off_t uploadNow, 
                                        intptr_t userdata) -> bool {
                if (downloadTotal > 0) {
                    asset->progress = static_cast<float>(downloadNow) / static_cast<float>(downloadTotal);
                }
                return true;
            };

            // 3. Сетевой запрос
            cpr::Response response = cpr::Get(
                cpr::Url{asset->downloadUrl},
                cpr::WriteCallback{write_func},
                cpr::ProgressCallback{progress_func}
            );

            file.close();

            // Проверка: Скачалась ли модель или нет?
            if (response.status_code == 200 && response.error.code == cpr::ErrorCode::OK && std::filesystem::exists(asset->filepath)) {
                success = true;
            }

            if(success) {
                asset->isDownloaded = true;
                asset->progress = 1.0f;

                // Выводим лог о том, что модель скачалась
                std::cout << "Model: " << asset->name << " is downloaded!!!" << std::endl; 
            }
            else {
                asset->isDownloaded = false;
                asset->progress = 0.0f;

                // Выводим лог о том, что модель не скачалась
                std::cout << "Model: " << asset->name << " is don't downloaded(" << std::endl;

                // Если скачалось криво или оборвалось удаляем недокаченный мусорный файл
                std::error_code ec;
                std::filesystem::remove(asset->filepath, ec);
            }

            asset->isDownloading = false;

            if(callback) {
                callback(success);
            }
        };

        std::thread AsyncDownload(downloadTask);
        AsyncDownload.detach();
    }

    bool ModelManager::checkModelAssetInFolder(const std::string& id) {
    /*
        Проверяет загружена ли модель в папку или нет
    */
        auto it = std::find_if(registry.begin(), registry.end(), [id](std::unique_ptr<ModelAsset>& asset) -> bool {
            return asset->id == id;
        });

        if(it == registry.end()) throw std::invalid_argument("This id is not found");
        
        if(std::filesystem::exists((*it)->filepath)) {
            (*it)->isDownloaded = true;
            (*it)->isDownloading = false;

            return true;
        }
        return false;
    }

    bool ModelManager::checkAllModelAssetInFolder() {
    /*
        Проверяет загружени ли все модели в папку или нет
    */
        for(auto& model : registry) {
            if(!checkModelAssetInFolder(model->id)) return false;
        }
        return true;
    }

    bool ModelManager::loadModelToMemory(const std::string& id) {
    /*
        Иницилизация модели и контекста 
        Добавление в регистр активных моделей (ActiveModels)
    */
        auto* asset = getAssetByID(id);
        if (!asset) {
            throw std::invalid_argument("This id is not found in registry: " + id);
        }

        // 2. Теперь проверяем, есть ли файл физически на диске
        if (!checkModelAssetInFolder(id)) {
            throw std::runtime_error("Model file not found on disk at path: " + asset->filepath);
        }
        
        if(activeModels.find(id) != activeModels.end()) return true;
           
        ActiveModel active_model;

        std::cout << "Active Model is add. ID: " << id << std::endl;

        llama_model_params model_params = llama_model_default_params();
        llama_context_params cnt_params = llama_context_default_params();

        cnt_params.n_ctx = active_model.config.n_ctx;

        active_model.model = llama_load_model_from_file(getAssetByID(id)->filepath.c_str(), model_params);
        active_model.ctx = llama_new_context_with_model(active_model.model, cnt_params);
        active_model.vocab = llama_model_get_vocab(active_model.model);

        activeModels[id] = active_model;
        return true;
    }

    void ModelManager::unloadModelByID(const std::string& id) {
    /*
        Выгрузка модели из памяти по ID
    */
        auto it = activeModels.find(id);

        // Проверка, что активная модель существует
        if(it == activeModels.end()) throw std::invalid_argument("This id is not found");

        // Модель что нужно выгрузить из памяти
        ActiveModel active_model = activeModels[id];    

        // Выгружаем данные из памяти
        if(active_model.ctx != nullptr) llama_free(active_model.ctx);
        if(active_model.model != nullptr) llama_free_model(active_model.model);
        
        // Удаляем модель из карты
        activeModels.erase(it);
    }

    void ModelManager::unloadAllModel() {
    /*
        Выгрузка всех модлей из памяти
    */
        for(auto& [id, aM] : activeModels) {

            // Выгружаем данные из памяти
            if(aM.ctx != nullptr) llama_free(aM.ctx);
            if(aM.model != nullptr) llama_free_model(aM.model);
        }

        // Удаляем карту с моделями
        activeModels.clear();
    }

    std::string ModelManager::generateResponse(const std::string& modelId, 
                                           const std::string& prompt, 
                                           const std::string& systemInstruction) {
        // 1. Валидация и Извлечение модели
        auto it = activeModels.find(modelId);
        if (it == activeModels.end()) throw std::invalid_argument("This model id is not found");
    
        ActiveModel active_model = it->second;

        if (!active_model.isValid()) throw std::runtime_error("Model is loaded but internal pointers are invalid!"); 

        // Очистить KV кэш ДО начала генерации
        if (active_model.ctx != nullptr) {
            llama_memory_t mem = llama_get_memory(active_model.ctx);
            // Удаляем всю последовательность 0 из памяти
            llama_memory_seq_rm(mem, 0, -1, -1);
        }

        // Применяем настройки многопоточности для вычислений
        // (В актуальном API за это отвечает llama_set_n_threads)
        llama_set_n_threads(active_model.ctx, active_model.config.n_threads, active_model.config.n_threads);

        // 2. Токенизация входных данных (Формируем итоговый текст)
        std::string text;
        if (!systemInstruction.empty()) {
            text = "System: " + systemInstruction + "\nUser: " + prompt + "\nAssistant:";
        } else {
            text = prompt;
        }

        std::vector<llama_token> tokens(text.size() + 4);
        int n_tokens = llama_tokenize(active_model.vocab, text.c_str(), text.size(), tokens.data(), tokens.size(), true, true);
        if (n_tokens < 0) {
            return "Error: tokenization failed";
        }
        tokens.resize(n_tokens);

        // Проверяем, не вылезем ли мы за рамки контекста модели сразу на старте
        if (n_tokens + active_model.config.n_predict > active_model.config.n_ctx) {
            return "Error: context length exceeded configuration limits";
        }

        // 3. Инициализация ресурсов генерации
        int max_batch_size = (std::max)(n_tokens, active_model.config.n_predict);
        llama_batch batch = llama_batch_init(max_batch_size, 0, 1);

        // Копируем весь промпт в батч для первого просчета
        for (int i = 0; i < n_tokens; i++) {
            batch.token[i]    = tokens[i];
            batch.pos[i]      = i;
            batch.n_seq_id[i] = 1;
            batch.seq_id[i][0] = 0;
            batch.logits[i]   = (i == n_tokens - 1);
        }
        batch.n_tokens = n_tokens;

        // Настройка цепочки сэмплеров
        llama_sampler* chain = llama_sampler_chain_init(llama_sampler_chain_default_params());
        
        if (active_model.config.temperature > 0.0f) {
            llama_sampler_chain_add(chain, llama_sampler_init_temp(active_model.config.temperature));
            llama_sampler_chain_add(chain, llama_sampler_init_top_p(active_model.config.top_p, 1));
            llama_sampler_chain_add(chain, llama_sampler_init_dist(active_model.config.seed)); 
        } else {
            llama_sampler_chain_add(chain, llama_sampler_init_greedy());
        }

        std::string answer = "";
        int n_cur = n_tokens; // Текущая позиция в контексте

        // 4. Цикл генерации (Предикт)
        for (int i = 0; i < active_model.config.n_predict; i++) {
            // Защита от переполнения физического контекста (чтобы не упасть в сигу)
            if (n_cur >= active_model.config.n_ctx) {
                break;
            }

            if (llama_decode(active_model.ctx, batch) != 0) {
                llama_sampler_free(chain);
                llama_batch_free(batch);
                return "Error: llama_decode failed";
            }

            llama_token id = llama_sampler_sample(chain, active_model.ctx, batch.n_tokens - 1);

            if (id == llama_token_eos(active_model.vocab)) {
                break;
            }

            char buffer[256];
            int n_chars = llama_token_to_piece(active_model.vocab, id, buffer, sizeof(buffer), 0, false);

            if (n_chars > 0) {
                std::string piece(buffer, n_chars);
                answer += piece;
            }

            // Проверки стоп-слов
            bool should_stop = false;
            for (const std::string& stop_word : active_model.config.anti_prompts) {
                if (answer.size() >= stop_word.size()) {
                    // Извлекаем из самого конца строки кусок размером со стоп-слово
                    std::string end_of_answer = answer.substr(answer.size() - stop_word.size());
                    if (end_of_answer == stop_word) {
                        should_stop = true;
                        break;
                    }
                }
            }
            
            if (should_stop) {
                goto end_generation;
            }

            // Очищаем батч и готовим его для следующего токена
            batch.n_tokens = 1;
            batch.token[0] = id;
            batch.pos[0]   = n_cur;
            batch.logits[0] = true;
            batch.n_seq_id[0] = 1;
            batch.seq_id[0][0] = 0;

            n_cur++; 
        }

    end_generation:

        // Очистка памяти
        llama_sampler_free(chain);
        llama_batch_free(batch);

        return answer;
    }
}