#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <core/models/ModelManager.hpp>
#include "nodes/core/ModelNode.hpp"
#include <fstream>
#include <filesystem>               // Для проверки - Есть ли модель по указаному пути
#include <iostream>
#include <json.hpp>                 // Для загрузки образов модели
#include <cerrno>
#include <cstring>
#include <thread>
#include <mutex>
#include <llama.h>
#include <cpr/cpr.h>

using json = nlohmann::json;

// Пустая функция обратного вызова, которая просто игнорирует все сообщения от llama.cpp
void null_llama_log_callback(ggml_log_level level, const char* text, void* user_data) {}

namespace SwarmIDE::Core {

    ActiveModel::~ActiveModel() noexcept {
        if(ctx != nullptr) llama_free(ctx);
        if(model != nullptr) llama_free_model(model);
    }

    bool ActiveModel::isValid() const {
        return model != nullptr && ctx != nullptr && vocab != nullptr;
    }

    void ActiveModel::reset() {
        // 1. Сбросить счётчики
        ctx_size = 0;
        tokens_past = 0;

        // 2. Очистить KV cache полностью (чтобы история не накапливалась)
        if(ctx != nullptr) {
            llama_memory_t mem = llama_get_memory(ctx);
            llama_memory_clear(mem, true);
        }

        // 3. Сбросить конфиг к дефолту 
        // config = GenerationConfig(); 
    }

    ModelManager& ModelManager::getInstance() {
        static ModelManager instance;
        return instance;
    }

    ModelManager::ModelManager() {               
        std::cout << "\n[BEGIN]: loading models\n" << std::endl;

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

        std::cout << "\n[END]: loading models\n" << std::endl;
    }

    ModelManager::~ModelManager() {
        unloadAllModel();      // Очишаем память от моделей
        registry.clear();      // Очишаем регистр от моделей

        llama_backend_free();  // Освобождает глобальные ресурсы библиотеки llama
    }
    
   
    
    const std::vector<std::unique_ptr<ModelAsset>>& ModelManager::getAllAssets() const { return registry; }

    ModelAsset* ModelManager::getAssetByID(const std::string& id) {
        for(auto& asset : registry) {
            if(asset->id == id)  return asset.get();  
        }
        return nullptr;
    }

    void ModelManager::startAsyncDownload(const std::string& id, std::function<void(bool)> callback) {

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
        for(auto& model : registry) {
            if(!checkModelAssetInFolder(model->id)) return false;
        }
        return true;
    }

    bool ModelManager::loadModelToMemory(const std::string& id) {
        std::lock_guard<std::mutex> lock(manager_mutex);        // Защита от потоков

        auto* asset = getAssetByID(id);
        if (!asset) {
            throw std::invalid_argument("This id is not found in registry: " + id);
        }

        // 2. Теперь проверяем, есть ли файл физически на диске
        if (!checkModelAssetInFolder(id)) {
            throw std::runtime_error("Model file not found on disk at path: " + asset->filepath);
        }
        
        if(activeModels.find(id) != activeModels.end()) return true;
           
        auto active_model = std::make_shared<ActiveModel>();

        std::cout << "Active Model is add. ID: " << id << std::endl;

        llama_model_params model_params = llama_model_default_params();
        llama_context_params cnt_params = llama_context_default_params();

        cnt_params.n_ctx = active_model->config.n_ctx;
        active_model->model = llama_load_model_from_file(getAssetByID(id)->filepath.c_str(), model_params);
        active_model->ctx = llama_new_context_with_model(active_model->model, cnt_params);
        active_model->vocab = llama_model_get_vocab(active_model->model);
        active_model->execution_mutex = std::make_unique<std::mutex>();

        activeModels[id] = active_model;
        return true;
    }

    void ModelManager::unloadModelByID(const std::string& id) {

        std::lock_guard<std::mutex> lock(manager_mutex);        // Защита от потоков
        auto it = activeModels.find(id);

        // Проверка, что активная модель существует
        if(it == activeModels.end()) throw std::invalid_argument("This id is not found");
        
        // Удаляем модель из карты
        activeModels.erase(it);
    }

    void ModelManager::unloadAllModel() {
        std::lock_guard<std::mutex> lock(manager_mutex);        // Защита от потоков
        activeModels.clear();                                   // Удаляем карту с моделями
    }

    /**
     * @brief Пошаговая реализация инференса и текстовой генерации через llama.cpp.
     * @details Функция выполняет полный цикл работы с нейросетью: от потокобезопасного 
     * извлечения контекста до пошагового декодирования токенов и фильтрации стоп-слов.
     * 
     * @param[in] modelId Уникальный строковый идентификатор модели.
     * @param[in] prompt Основной текст запроса для ИИ-агента.
     * @param[in] systemInstruction Системный промпт для задания роли/правил.
     * @return std::string Чистый сгенерированный текст ответа (без стоп-слов).
     * 
     * @throws std::invalid_argument Если запрашиваемый @p modelId отсутствует в реестре активных моделей.
     * @throws std::runtime_error Если модель найдена, но её внутренние указатели `llama.cpp` сломаны.
     */
    std::string ModelManager::generateResponse(const std::string& modelId, 
                                            const std::string& prompt, 
                                            const std::string& systemInstruction) {

        // =====================================================================
        // 1. ВАЛИДАЦИЯ И ИЗВЛЕЧЕНИЕ МОДЕЛИ (Потокобезопасный критический участок)
        // =====================================================================
        std::shared_ptr<ActiveModel> active_model;
        {
            // Защищаем хэш-таблицу activeModels от одновременного чтения/записи другими потоками ядра
            std::lock_guard<std::mutex> lock(manager_mutex);
            auto it = activeModels.find(modelId);
            if (it == activeModels.end()) {
                throw std::invalid_argument("This model id is not found");
            }
            active_model = it->second;
        }

        // Блокируем индивидуальный мьютекс модели, запрещая параллельный инференс на одном контексте
        std::lock_guard<std::mutex> lock(*active_model->execution_mutex);

        if (!active_model->isValid()) {
            throw std::runtime_error("Model is loaded but internal pointers are invalid!"); 
        }

        // Очищаем предыдущее состояние памяти контекста перед новой итерацией диалога
        active_model->reset();

        // Устанавливаем количество вычислительных потоков CPU в актуальном API llama.cpp
        llama_set_n_threads(active_model->ctx, active_model->config.n_threads, active_model->config.n_threads);

        // =====================================================================
        // 2. ТОКЕНИЗАЦИЯ ВХОДНЫХ ДАННЫХ (Формирование промпта и перевод в цифру)
        // =====================================================================
        std::string text;
        if (!systemInstruction.empty()) {
            // Форматируем классический промпт-шаблон "Система-Пользователь-Ассистент"
            text = "System: " + systemInstruction + "\nUser: " + prompt + "\nAssistant:";
        } else {
            text = prompt;
        }

        // Выделяем буфер под токены с запасом
        std::vector<llama_token> tokens(text.size() + 4);
        int n_tokens = llama_tokenize(active_model->vocab, text.c_str(), text.size(), tokens.data(), tokens.size(), true, true);
        if (n_tokens < 0) {
            return "Error: tokenization failed";
        }
        tokens.resize(n_tokens); // Подгоняем вектор под реальное число токенов

        // Жесткая проверка лимитов, чтобы не вызвать переполнение контекста (Context Overflow)
        if (n_tokens + active_model->config.n_predict > active_model->config.n_ctx) {
            return "Error: context length exceeded configuration limits";
        }

        // =====================================================================
        // 3. ИНИЦИАЛИЗАЦИЯ РЕСУРСОВ ГЕНЕРАЦИИ (Выделение C-памяти и сэмплеров)
        // =====================================================================
        int max_batch_size = (std::max)(n_tokens, active_model->config.n_predict);
        llama_batch batch = llama_batch_init(max_batch_size, 0, 1);

        // Копируем весь токенизированный промпт в батч для первого массивного просчета (Prefill Phase)
        for (int i = 0; i < n_tokens; i++) {
            batch.token[i]    = tokens[i];
            batch.pos[i]      = i;
            batch.n_seq_id[i] = 1;
            batch.seq_id[i][0] = 0;
            batch.logits[i]   = (i == n_tokens - 1); // Логиты нужны только для самого последнего токена
        }
        batch.n_tokens = n_tokens;

        // Создаем цепочку сэмплеров (Sampler Chain) для фильтрации распределения вероятностей слов
        llama_sampler* chain = llama_sampler_chain_init(llama_sampler_chain_default_params());
        
        if (active_model->config.temperature > 0.0f) {
            // Настройка динамического сэмплинга (Креативность -> Nucleus sampling -> Выбор распределения)
            llama_sampler_chain_add(chain, llama_sampler_init_temp(active_model->config.temperature));
            llama_sampler_chain_add(chain, llama_sampler_init_top_p(active_model->config.top_p, 1));
            llama_sampler_chain_add(chain, llama_sampler_init_dist(active_model->config.seed)); 
        } else {
            // Жадный сэмплинг (Greedy): всегда берется слово с максимальной вероятностью
            llama_sampler_chain_add(chain, llama_sampler_init_greedy());
        }

        std::string answer = "";
        int n_cur = n_tokens; // Храним текущую позицию указателя в окне контекста

        // =====================================================================
        // 4. ЦИКЛ ГЕНЕРАЦИИ (Предикт по одному токену / Generation Phase)
        // =====================================================================
        for (int i = 0; i < active_model->config.n_predict; i++) {
            // Защита от выхода за физические границы выделенного контекста (предотвращает Segmentation Fault)
            if (n_cur >= active_model->config.n_ctx) {
                break;
            }

            // Просчет графа нейросети для текущего батча
            if (llama_decode(active_model->ctx, batch) != 0) {
                llama_sampler_free(chain);
                llama_batch_free(batch);
                return "Error: llama_decode failed";
            }

            // Извлекаем токен из сэмплера на основе просчитанных логитов
            llama_token id = llama_sampler_sample(chain, active_model->ctx, batch.n_tokens - 1);

            // Если модель выдала токен конца генерации (End of Sequence) — завершаем цикл
            if (id == llama_token_eos(active_model->vocab)) {
                break;
            }

            // Конвертируем цифровой токен обратно в текстовый кусок (String Piece)
            char buffer[256];
            int n_chars = llama_token_to_piece(active_model->vocab, id, buffer, sizeof(buffer), 0, false);

            if (n_chars > 0) {
                std::string piece(buffer, n_chars);
                answer += piece; // Приклеиваем кусок к итоговому ответу
            }

            // Проверка триггеров преждевременной остановки (Анти-промпты / Стоп-слова)
            bool should_stop = false;
            for (const std::string& stop_word : active_model->config.anti_prompts) {
                if (answer.size() >= stop_word.size()) {
                    // Извлекаем из самого конца строки кусок размером со стоп-слово
                    std::string end_of_answer = answer.substr(answer.size() - stop_word.size());
                    if (end_of_answer == stop_word) {
                        // Срезаем стоп-слово из вывода, чтобы пользователь его не увидел
                        answer.resize(answer.size() - stop_word.size()); 
                        should_stop = true;
                        break;
                    }
                }
            }

            if (should_stop) {
                goto end_generation;
            }

            // Очищаем батч и подготавливаем его структуру для генерации следующего ЕДИНИЧНОГО токена
            batch.n_tokens = 1;
            batch.token[0] = id;
            batch.pos[0]   = n_cur;
            batch.logits[0] = true;
            batch.n_seq_id[0] = 1;
            batch.seq_id[0][0] = 0;

            n_cur++; // Сдвигаем курсор контекста вперед
        }

    end_generation:

        // =====================================================================
        // 5. ОЧИСТКА РЕСУРСОВ (Предотвращение утечек памяти / Memory Leaks)
        // =====================================================================
        llama_sampler_free(chain);
        llama_batch_free(batch);

        return answer;
    }

}