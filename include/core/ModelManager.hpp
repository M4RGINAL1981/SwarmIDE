#pragma once
#include <iostream>
#include <exception>
#include <string>
#include <vector>

namespace SwarmIDE::Core {

    struct Model {                 // Структура модели

        std::string name;          // Имя модели
        std::string path;          // Строковой путь до модели
        std::string iconPath;      // Путь до иконки модели
 
        std::string downloadUrl;   // Url для скачивания модели
        bool isDownloaded;         // Переменная показывающая скачана ли модель или нет
        std::string id;            // ID модели
    };

    class ModelNotFoundException : public std::exception {
    public:
        explicit ModelNotFoundException(const std::string id, const std::string& msg) : message(msg), modelId(id) {};

        virtual const char* what() const noexcept override { return msg.c_str(); }

    private:
        std::string modelId;       // id модели с которой произошла ошибка
        std::string message;       // Текст ошибки
    };

    class ModelManager { 
    public:
        // Глобальная точка доступа к единственному экземпляру
        static ModelManager& getInstance();

        std::vector<std::string> GetNamesModels();                // Возврашает имена всех моделей
        std::vector<Model>& GetModels() { return models; }        // Возврашает все модели
        Model* GetModelForID(std::string id);                     // Возврашает модель по её ID

        bool downloadModelForID(std::string id);                  // Скачавание модели по её ID

        ModelManager(const ModelManager&) = delete;
        ModelManager& operator=(const ModelManager&) = delete;
        ModelManager(ModelManager&&) = delete;
        ModelManager& operator=(ModelManager&&) = delete;

    private:
        ~ModelManager() = default;
        ModelManager();

        std::vector<Model> models;                                // Вектор всех моделей
    };
}