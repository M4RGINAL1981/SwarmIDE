#pragma once
#include "SwarmIDE.hpp"
#include "core/ObjectManager.hpp"
#include "core/ConnectionManager.hpp"
#include "ui/Theme.hpp"
#include <string>

namespace SwarmIDE::Core {
    class SerializationManager {
    public:
        // Глобальная точка доступа к единственному экземпляру
        static SerializationManager& getInstance();

        void SaveGraph(const std::string& filepath);          // Собирает данные из менеджеров и пишет JSON
        void LoadGraph(const std::string& filepath);          // Парсит JSON, чистит сцену и воссоздаёт граф
        void ClearGraph();                                    // Чистка сцены

        void SetFonts(const SwarmIDE::UI::Fonts& fontsAtlas) { this->fonts = fontsAtlas; }  // Установка шрифтов
        SwarmIDE::UI::Fonts& GetFonts() { return fonts; }                                   // Передача шрифтов

        SerializationManager(const SerializationManager&) = delete;
        SerializationManager& operator=(const SerializationManager&) = delete;
        SerializationManager(SerializationManager&&) = delete;
        SerializationManager& operator=(SerializationManager&&) = delete;
        
    private:
        friend SwarmIDE::Core::ObjectManager;
        friend SwarmIDE::Core::ConnectionManager;

        SwarmIDE::UI::Fonts fonts;                                       // Шрифты объектов

        SerializationManager() {};
        ~SerializationManager() = default;
    };
}