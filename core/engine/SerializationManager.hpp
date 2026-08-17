#pragma once
#include "SwarmIDE.hpp"
#include "core/graph/GraphManager.hpp"
#include "core/graph/ConnectionManager.hpp"
#include "core/models/ModelManager.hpp"
#include "ui/canvas/CanvasManager.hpp"
#include "core/engine/LoggingManager.hpp"
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

namespace SwarmIDE::Core {

    /**
     * @class SerializationManager
     * @brief Менеджер сохранения и загрузки JSON файлов со сцены (Синглтон).
     * @details Вызывается через Menu и сохраняет, загружает или очищает сцену
     * в зависимости от выбранного действия. Обеспечивает целостность данных
     * при переходе между файлами.
     * 
     * @note Класс спроектирован как синглтон.
     * @see SwarmIDE::UI::Menu
     */
    class SerializationManager {
    public:
        /**
         * @brief Глобальная точка доступа к единственному экземпляру SerializationManager.
         * @return SerializationManager& Ссылка на инстанс менеджера.
         */
        static SerializationManager& getInstance();

        /**
         * @brief Сохраняет текущий граф в указанный путь.
         * @param filepath Путь к файлу .json
         */
        void SaveGraph(const std::string& filepath); 

        /**
         * @brief Загружает граф из файла, выполняя двухэтапную валидацию нод и связей.
         * @param filepath Путь к исходному файлу.
         */
        void LoadGraph(const std::string& filepath);      

        /**
         * @brief Полностью очищает текущую сессию (ноды и связи).
         */
        void ClearGraph();                                    

        /**
         * @brief Генерирует уникальное имя файла для сохранения.
         * @param baseName Базовое имя (без расширения)
         * @return std::string Путь к новому файлу.
         */
        std::string GetNextAvailableFileName(const std::string& baseName); 

        // Запрет копирования и перемещения
        SerializationManager(const SerializationManager&) = delete;
        SerializationManager& operator=(const SerializationManager&) = delete;
        SerializationManager(SerializationManager&&) = delete;
        SerializationManager& operator=(SerializationManager&&) = delete;
        
    private:
        friend GraphManager;
        friend ConnectionManager;
        friend ModelManager;

        SerializationManager() { LoggingManager::Info("[SERIALIZATION_MANAGER]: Constructor"); }
        ~SerializationManager() { LoggingManager::Info("[SERIALIZATION_MANAGER]: Destructor");}
    };
}
