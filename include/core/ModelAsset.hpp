#pragma once 
#include <string>
#include <atomic>

namespace SwarmIDE::Core {

    struct ModelAsset {
        std::string id;                          // ID модели
        std::string name;                        // Имя модели
        std::string filepath;                    // Путь до самой модели
        std::string fileIcon;                    // Путь до иконки
        std::string downloadUrl;                 // URL для скачивания

        // Потокобезопасные флаги для UI
        std::atomic<bool> isDownloaded{false};   // Скачана ли модель
        std::atomic<bool> isDownloading{false};  // Качается ли модель прямо сейчас
        std::atomic<float> progress{0.0f};       // Прогресс от 0.0f до 1.0f
    };

}