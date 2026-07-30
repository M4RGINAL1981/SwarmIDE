#pragma once
#include "core/models/ModelAsset.hpp"
#include <vector>
#include <string>
#include <memory>
#include <mutex>
#include <functional>

// Предварительное объявление низкоуровневых C-типов библиотеки llama.cpp
struct llama_model;
struct llama_context;
struct llama_vocab;

namespace SwarmIDE::Core {

    namespace Graph { class ModelNode; }
              
    /**
     * @struct GenerationConfig
     * @brief Настройки параметров генерации и инференса текста для LLM.
     * @details Передается из графического узла холста в менеджер моделей для тонкой 
     * настройки поведения нейросети (например, уровня креативности или лимита токенов).
     */
    struct GenerationConfig { 
        int n_predict = 512;                                                 /**< Максимальное количество генерируемых токенов за один вызов. */
        float temperature = 0.7f;                                            /**< Креативность модели (0.0 — жесткий точный ответ, 1.0 — случайный творческий выбор слов). */
        float top_p = 0.9f;                                                  /**< Параметр Nucleus sampling: отсечение маловероятных слов (выбор из лучших 90%). */
        
        uint32_t seed = 42;                                                  /**< Зерно генерации для воспроизводимости результатов (-1 или случайное число для вариативности). */
        int n_threads = 4;                                                   /**< Количество используемых потоков CPU для вычислений (оптимальный старт — 4). */
        std::vector<std::string> anti_prompts = {"User:", "Observation:"};   /**< Стоп-слова (анти-промпты). При генерации этих токенов инференс немедленно прерывается. */
        int n_ctx = 2048;                                                    /**< Размер контекстного окна памяти модели (стандарт для базовых GGUF — 2048 токенов). */

        /**
         * @brief Фабричный метод для создания конфигурации с настройками по умолчанию.
         * @return GenerationConfig Объект настроек по умолчанию.
         */
        GenerationConfig& createDefaultGenerationConfig() { GenerationConfig gC; return gC; };
    };

    /**
     * @struct ActiveModel
     * @brief Контейнер активной модели, загруженной непосредственно в оперативную память (ОЗУ/VRAM).
     * @details Обертка над сырыми указателями контекста `llama.cpp`. Управляет жизненным циклом 
     * загруженных весов нейросети и защищает вычислительный контекст от одновременных запросов с помощью мьютекса.
     * 
     * @see ModelManager
     */
    struct ActiveModel {
        struct llama_model* model = nullptr;                        /**< Сырой указатель на загруженные в память веса модели GGUF. */
        struct llama_context* ctx = nullptr;                        /**< Рабочая область вычислений и текущее состояние контекста инференса. */
        const struct llama_vocab* vocab = nullptr;                  /**< Токенизатор и словарь для перевода текста в цифровые токены и обратно. */
        
        int ctx_size = 0;                                           /**< Фактический полный размер выделенного контекста. */
        int tokens_past = 0;                                        /**< Счетчик уже обработанных токенов (используется для сохранения истории диалога). */
        
        GenerationConfig config;                                    /**< Конфигурация параметров генерации для данной модели. */
        mutable std::unique_ptr<std::mutex> execution_mutex;        /**< Мьютекс для синхронизации вычислений: предотвращает одновременный запуск инференса из параллельных агентов. */

        ActiveModel() = default;
        /** @brief Деструктор. Автоматически освобождает ресурсы llama.cpp через вызов unload-методов. */
        ~ActiveModel();

        /**
         * @brief Проверяет, валидны ли указатели и готова ли модель к вычислениям.
         * @return true Если все указатели (`model`, `ctx`, `vocab`) успешно инициализированы.
         */
        bool isValid() const;
        
        /** @brief Сбрасывает состояние токенов и очищает контекст памяти истории. */
        void reset();
    };

    /**
     * @class ModelManager
     * @brief Центральный менеджер ИИ-моделей, управления ассетами и инференса (Синглтон).
     * @details Управляет реестром доступных моделей на диске (`ModelAsset`), осуществляет их 
     * асинхронную загрузку из сети, аллокацию весов в RAM/VRAM и предоставляет потокобезопасный 
     * интерфейс для текстовой генерации ИИ-агентов.
     * 
     * @see ModelAsset
     * @see ActiveModel
     */
    class ModelManager {
    public:
        /**
         * @brief Глобальная точка доступа к единственному экземпляру ModelManager.
         * @return ModelManager& Ссылка на синглтон менеджера моделей.
         */
        static ModelManager& getInstance();

        // --- API для UI (Управление ассетами и загрузкой) ---
        
        /**
         * @brief Получить доступ к полному списку зарегистрированных в системе ассетов моделей.
         * @return const std::vector<std::unique_ptr<ModelAsset>>& Константная ссылка на вектор моделей.
         */
        const std::vector<std::unique_ptr<ModelAsset>>& getAllAssets() const;

        /**
         * @brief Найти ассет модели по его уникальному строковому ID.
         * @param[in] id Идентификатор модели (например, "llama-3-8b").
         * @return ModelAsset* Указатель на ассет или `nullptr`, если модель не зарегистрирована.
         */
        ModelAsset* getAssetByID(const std::string& id);
        
        /**
         * @brief Запустить асинхронное скачивание файла модели из сети.
         * @details Метод мгновенно возвращает управление, запуская скачивание в отдельном `std::thread` 
         * через CPR/Curl. Это гарантирует, что интерфейс ImGui продолжит плавно отрисовываться.
         * 
         * @param[in] id Уникальный идентификатор скачиваемой модели.
         * @param[in] callback Опциональный колбэк, вызываемый по завершении загрузки (`true` — успешно, `false` — ошибка).
         * @note В процессе скачивания метод динамически обновляет флаги внутри SwarmIDE::Core::ModelAsset.
         */
        void startAsyncDownload(const std::string& id, std::function<void(bool)> callback = nullptr);

        /**
         * @brief Проверить физическое наличие файла модели в локальной папке ресурсов.
         * @param[in] id Идентификатор модели.
         * @return true Если файл `.gguf` присутствует на диске и его размер валиден.
         */
        bool checkModelAssetInFolder(const std::string& id);
        
        /**
         * @brief Проверить наличие на диске вообще всех зарегистрированных моделей.
         * @return true Если абсолютно все файлы из конфигурационного реестра скачаны.
         */
        bool checkAllModelAssetInFolder();

        // --- Управление памятью (ОЗУ / VRAM) ---

        /**
         * @brief Загрузить веса модели с диска в оперативную память для инференса.
         * @details Парсит `.gguf` файл, аллоцирует структуры `llama.cpp` и создает `ActiveModel`.
         * 
         * @param[in] id Идентификатор загружаемой модели.
         * @return true Если модель успешно загружена и прошла валидацию.
         * @see ActiveModel::isValid()
         */
        bool loadModelToMemory(const std::string& id);
        
        /** @brief Выгрузить абсолютно все активные модели из памяти и освободить ОЗУ/VRAM. */
        void unloadAllModel();
        
        /**
         * @brief Выгрузить конкретную модель из памяти по её ID.
         * @param[in] id Идентификатор выгружаемой модели.
         */
        void unloadModelByID(const std::string& id);

        // --- Высокоуровневый инференс (Генерация текста) ---

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
        std::string generateResponse(const std::string& modelId, 
                                     const std::string& prompt, 
                                     const std::string& systemInstruction);

        // Блокируем копирование и перемещение менеджера
        ModelManager(const ModelManager&) = delete;
        ModelManager& operator=(const ModelManager&) = delete;
        ModelManager(ModelManager&&) = delete;
        ModelManager& operator=(ModelManager&&) = delete;

    private:
        ModelManager();
        ~ModelManager();

        std::string modelsPath = "resources/models/models.json";                         /**< Путь к конфигурационному JSON-файлу со списком всех доступных моделей. */

        std::vector<std::unique_ptr<ModelAsset>> registry;                               /**< Базовый реестр всех известных системе моделей (загруженных из JSON). */
        std::unordered_map<std::string, std::shared_ptr<ActiveModel>> activeModels;      /**< Карта (хэш-таблица) активных моделей, находящихся сейчас в памяти. */

        std::mutex manager_mutex;                                                        /**< Мьютекс для защиты критических секций аллокации/деаллокации моделей в памяти. */
    };
}
