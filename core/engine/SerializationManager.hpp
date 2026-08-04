#pragma once
#include "SwarmIDE.hpp"
#include "core/graph/GraphManager.hpp"
#include "core/graph/ConnectionManager.hpp"
#include "core/models/ModelManager.hpp"
#include "core/engine/LoggingManager.hpp"
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

namespace SwarmIDE::Core {

    /**
     * @class SerializationManager
     * @brief Менеджер сохранения и загрузки JSON файлов со сценами (Синглтон).
     * @details Вызывается через Menu и сохраняет, загружает или очищает сцену
     * взависимости от нажатой кнопки на TopBar.
     * 
     * @note Класс спроектирован синглтон.
     * @see SwarmIDE::UI::Menu
     */
    class SerializationManager {
    public:
        /**
         * @brief Глобальная точка доступа к единственному экземпляру SerializationManager.
         * @return SerializationManager& Ссылка на синглтон генератора идентификаторов.
         */
        static SerializationManager& getInstance();


        /**
         * @brief Сохраняет JSON файл со сценой
         * @details Собирает данные из менеджеров и пишет JSON
         * 
         * @param filepath 
         */
        void SaveGraph(const std::string& filepath); 

        /**
         * @brief Загружает JSON файл со сценой
         * @details Парсит JSON, чистит сцену и воссоздаёт граф
         * 
         * @param filepath 
         */
        void LoadGraph(const std::string& filepath);      

        /**
         * @brief Очищает граф
         * @details Удаляет все подключения между нодами,
         * а затем удаляет сами ноды
         */
        void ClearGraph();                                    

        /**
         * @brief Получить объект «Следующее доступное имя файла»
         * 
         * @param[in] baseName 
         * @return std::string 
         */
        std::string GetNextAvailableFileName(const std::string& baseName); 

        // Блокируем копирование и перемещение менеджера
        SerializationManager(const SerializationManager&) = delete;
        SerializationManager& operator=(const SerializationManager&) = delete;
        SerializationManager(SerializationManager&&) = delete;
        SerializationManager& operator=(SerializationManager&&) = delete;
        
    private:
        friend GraphManager;
        friend ConnectionManager;
        friend ModelManager;

        SerializationManager() { Core::LoggingManager::Info("[SERIALIZATION_MANAGER]: Constructor"); }
        ~SerializationManager() { Core::LoggingManager::Info("[SERIALIZATION_MANAGER]: Destructor");}
    };
}

