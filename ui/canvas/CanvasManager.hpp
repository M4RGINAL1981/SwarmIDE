#pragma once 
#include "SwarmIDE.hpp"
#include "ui/views/ViewNode.hpp"
#include "ui/views/ViewConnection.hpp"
#include "ui/windows/ThemeManager.hpp"
#include "core/engine/LoggingManager.hpp"
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
        void removeAllViewNodes() { RemoveAllViewConnection(); viewNodes.clear(); }                            

        /**
         * @brief Получить текущее количество визуальных нод на сцене холста.
         * @return int Количество активных визуальных нод.
         */
        int getCountViewNodes() const { return viewNodes.size(); }        
        
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
         * @brief Добавить новое визуальное соединение
         * 
         * @param[in] connectionID ID логического соединения, для которого нужно создать визуальное соединение
         */
        void AddViewConnection(UUIDv4::UUID connectionID);

        /**
         * @brief Удаляет соединение по ID логического соединения
         * 
         * @param[in] connectionID ID логического соединения, которое нужно удалить
         */
        void RemoveViewConnection(UUIDv4::UUID connectionID);

        /**
         * @brief Удаляетвсе визуальные соединения
         */
        void RemoveAllViewConnection() { viewConnections.clear(); }

        /**
         * @brief Получить текущее количество визуальных нод на сцене холста.
         * @return int Количество активных визуальных нод.
         */
        int getCountViewConnection() const { return viewConnections.size(); }        
        
        /**
         * @brief Получить доступ к контейнеру со всеми визуальными соединениями.
         * @return std::unordered_map<UUIDv4::UUID, std::unique_ptr<ViewConnection>>>& Ссылка на массив визуальных соединений.
         */   
        std::unordered_map<UUIDv4::UUID, std::unique_ptr<ViewConnection>>& getViewConnections() { return viewConnections; }

        /**
         * @brief Найти визуальное соединение 
         * @param[in] connectionID - ID логического соединения
         */
        ViewConnection* findViewConnectionByID(UUIDv4::UUID connectionID);

        /**
         * @brief Синхронизация логики и визуала
         */
        void SyncWithCoreGraph();

        /**
         * @brief Отрисовывает бесконечную сетку, все соединения и все ноды.
         */
        void Draw();

        Graphics::Camera& GetCamera() { return camera; }
        Graphics::Grid& GetGrid() { return grid; }
        void Update(float deltaTime) { camera.update(deltaTime); }

        /**
         * @brief Получить текущий размер окна
         * @return glm::vec2 Текущий размер окна
         */
        glm::vec2 GetWindowSize() const { return windowSize; }

        /**
         * @brief Установить размер окна
         * 
         * @param width 
         * @param height 
         */
        void SetWindowSize(int width, int height) { windowSize = glm::vec2((float)width, (float)height); }

        /**
         * @brief Установить состояние временного провода
         * @param[in] nodeID   ID ноды
         * @param[in] pinID    ID пина
         * @param[in] mousePos Текущее положение мыши на экране
         */
        void SetTempWire(const UUIDv4::UUID& nodeID, const UUIDv4::UUID& pinID, const glm::vec2& mousePos) {
            tempWire = {nodeID, pinID, mousePos};
        }

        /**
         * @brief Очистить состояние временного провода
         */
        void ClearTempWire() { tempWire = std::nullopt; }

        // Блокируем копирование и перемещение менеджера
        CanvasManager(const CanvasManager&) = delete;
        CanvasManager& operator=(const CanvasManager&) = delete;
        CanvasManager(CanvasManager&&) = delete;
        CanvasManager& operator=(CanvasManager&&) = delete;
    private:
        friend class AppController;

        /**
         * @struct TempWireContext 
         * @brief Состояние временного провода
         */
        struct TempWireContext {
            UUIDv4::UUID startNodeID;
            UUIDv4::UUID startPinID;
            glm::vec2 currentMouseWorldPos;
        };
        std::optional<TempWireContext> tempWire = std::nullopt;

        CanvasManager();
        ~CanvasManager();

        glm::vec2 windowSize = glm::vec2(3840, 2160);                                           /** @brief < Размер окна */

        Graphics::Camera camera;                                                                /** @brief < Объект камера. Для получения текущего zoomFactor. */
        Graphics::Grid grid;                                                                    /** @brief < Объект сетки. Для отрисовки сетки. */
        std::unordered_map<UUIDv4::UUID, std::unique_ptr<ViewNode>> viewNodes;                  /** @brief < Хеш-Таблица. Ключ - ID логической ноды. Значение - Визуальная нода. */
        std::unordered_map<UUIDv4::UUID, std::unique_ptr<ViewConnection>> viewConnections;      /** @brief < Вектор визуальных соединений. */
    };
}