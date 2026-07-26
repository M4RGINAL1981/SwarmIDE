#pragma once 
#include "SwarmIDE.hpp"
#include "ui/views/ViewNode.hpp"
#include "ui/views/ViewConnection.hpp"
#include "ui/windows/ThemeManager.hpp"
#include "graphics/Grid.hpp"
#include "graphics/Camera.hpp"
#include "uuid_v4/uuid_v4.h"
#include <unordered_map>
#include <vector>

namespace SwarmIDE::UI {

    
    /**
     * @class CanvasManager
     * @brief
     * 
     * @see SwarmIDE::UI::ViewNode
     * @see SwarmIDE::UI::ViewConnection
     * @see SwarmIDE::UI::ThemeManager
     */
    class CanvasManager {
    public:

        /**
         * @brief Глобальная точка доступа к единственному экземпляру CanvasManager.
         * @return CanvasManager& Ссылка на синглтон генератора идентификаторов.
         */
        static CanvasManager& getInstance();

        /**
         * @brief Добавить визуальную ноду.
         * 
         * @param[in] nodeID   ID ноды, для которой нужно создать визуальную ноду.
         * @param[in] spawnPos Координаты спавна.
         */
        void AddViewNode(UUIDv4::UUID nodeID, glm::vec2 spawnPos);

        /**
         * @brief Удалить визульную нод.
         * 
         * @param[in] nodeID ID ноды, что нужно удалить.
         */
        void RemoveViewNode(UUIDv4::UUID nodeID);

        /**
         * @brief Полностью очистить граф сцены.
         * @details Удаляет абсолютно все визуальные ноды и обнуляет хранилище. Используется при создании 
         * нового проекта или перед импортом другого графа.
         */                 
        void removeAllViewNodes();                                     

        /**
         * @brief Получить текущее количество визуальных нод на сцене холста.
         * @return int Количество активных визуальных нод.
         */
        int getCountViewNodes() { return viewNodes.size(); }        
        
        /**
         * @brief Получить доступ к контейнеру со всеми узлами.
         * @return std::unordered_map<UUIDv4::UUID, std::unique_ptr<ViewNode>>& Ссылка на массив визуальных нод.
         */   
        std::unordered_map<UUIDv4::UUID, std::unique_ptr<ViewNode>>& getViewNodes() { return viewNodes; }

        /**
         * @brief Найти ноду по ID
         * 
         * @param[in] nodeID ID ноду, что нужно найти 
         * @return ViewNode* 
         */
        ViewNode* findViewNodeByID(UUIDv4::UUID nodeID);

        /**
         * @brief Удаляет все соединения с нодой по ID
         * 
         * @param nodeID ID ноды, с которой нужно удалить все связи
         */
        void removeViewConnectionsForNode(UUIDv4::UUID nodeID);

        /**
         * @brief Добавить новое соединение
         * 
         * @param[in] conn 
         */
        void AddViewConnection(Core::Connection* conn);

        /**
         * @brief Удаляет соединение по указателю
         * 
         * @param[in] conn Указатель на визуальное соединение 
         */
        void RemoveViewConnection(Core::Connection* conn);

        /**
         * @brief Удаляетвсе визуальные соединения
         */
        void RemoveAllViewConnection() { viewLinks.clear(); }

        /**
         * @brief Получить текущее количество визуальных нод на сцене холста.
         * @return int Количество активных визуальных нод.
         */
        int getCountViewConnection() { return viewLinks.size(); }        
        
        /**
         * @brief Получить доступ к контейнеру со всеми визуальными соединениями.
         * @return std::vector<std::unique_ptr<ViewNode>>& Ссылка на массив визуальных соединений.
         */   
        std::vector<std::unique_ptr<ViewConnection>>& getViewConnections() { return viewLinks; }

        /**
         * @brief Найти визуальное соединение 
         */
        //ViewNode* findViewConnection();

        /**
         * @brief Отрисовывает бесконечную сетку, все линки, все ноды.
         */
        void Draw();

        /**
         * @brief Клик по пустому месту, выделение рамкой (Box Selection),
         * панорамирование холста средней кнопкой мыши.
         */
        void HandleCanvasInputs();

        /**
         * @brief Синхронизирует появление/удаление нод бэкенда с UI-компонентами.
         */
        void SyncWithCoreGraph();

        Graphics::Camera& GetCamera() { return camera; }
        Graphics::Grid& GetGrid() { return grid; }
        void Update(float deltaTime) { camera.update(deltaTime); }


        // Блокируем копирование и перемещение менеджера
        CanvasManager(const CanvasManager&) = delete;
        CanvasManager& operator=(const CanvasManager&) = delete;
        CanvasManager(CanvasManager&&) = delete;
        CanvasManager& operator=(CanvasManager&&) = delete;
    private:
        CanvasManager();
        ~CanvasManager() = default;

        glm::vec2 windowSize = glm::vec2(3840, 2160);                                      /** @brief < Размер окна */

        Graphics::Camera camera;                                                           /** @brief < Объект камера. Для получения текущего zoomFactor. */
        Graphics::Grid grid;                                                               /** @brief < Объект сетки. Для отрисовки сетки. */
        std::unordered_map<UUIDv4::UUID, std::unique_ptr<ViewNode>> viewNodes;             /** @brief < Хеш-Таблица. Ключ - ID логической ноды. Значение - Визуальная нода. */
        std::vector<std::unique_ptr<ViewConnection>> viewLinks;                            /** @brief < Вектор визуальных соединений. */

        /**
         * @struct activeLinkContext
         * @brief Структура, хранящая состояние сборки связи 
         * (тянет ли пользователь провод из пина прямо сейчас, откуда тянет, где мышь).
         */
        struct activeLinkContext {
            // Не знаю, какие поля должны быть
        };
    };
}