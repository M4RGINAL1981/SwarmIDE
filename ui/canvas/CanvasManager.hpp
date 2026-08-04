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
     * @brief Указывает текущиее состояние событий мыши
     */
    enum class MouseEvents {
        IDLE,                  /** @brief < Ничего не делаем / Просто водим мышкой */
        DRAGGING_NODE,         /** @brief < Тащим ноду */
        DRAGGING_WIRE          /** @brief < Тянем связь из пина */
    };

    /**
     * @struct ActiveLinkContext
     * @brief Структура, хранящая состояние сборки связи 
     * (тянет ли пользователь провод из пина прямо сейчас, откуда тянет, где мышь).
     */
    struct ActiveLinkContext {
        MouseEvents state = MouseEvents::IDLE;         /** @brief < Текущиее состояние */

        // Данные для перемещения ноды
        UUIDv4::UUID grabberNodeID;                    /** @brief < Какую ноду тащим */
        glm::vec2 initialNodePos;                      /** @brief < Где она была до начала перетаскивания (для отмены) */

        // Данные для создания связи
        UUIDv4::UUID startNodeID;                      /** @brief < Из какой ноды тянем связь */  
        UUIDv4::UUID startPinID;                       /** @brief < Из какого пина тянем связь */
        glm::vec2 currentMouseWorldPos;                /** @brief < Текущие координаты мыши в мире */

        void Reset() {                                 /** @brief < Методы для очистки контекста */
            state = MouseEvents::IDLE;
            initialNodePos = glm::vec2(0.0f, 0.0f);
            currentMouseWorldPos = glm::vec2(0.0f, 0.0f);
        }    
    };
    
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
        int getCountViewConnection() { return viewConnections.size(); }        
        
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

        /**
         * @brief Клик по пустому месту, выделение рамкой (Box Selection),
         * панорамирование холста средней кнопкой мыши.
         * @param[in] zoomFactor - текущий зум.
         */
        void HandleCanvasInputs(float zoomFactor);

        Graphics::Camera& GetCamera() { return camera; }
        Graphics::Grid& GetGrid() { return grid; }
        void Update(float deltaTime) { camera.update(deltaTime); }

        /**
         * @brief Установить размер окна
         * 
         * @param width 
         * @param height 
         */
        void SetWindowSize(int width, int height) { windowSize = glm::vec2((float)width, (float)height); }

        // Блокируем копирование и перемещение менеджера
        CanvasManager(const CanvasManager&) = delete;
        CanvasManager& operator=(const CanvasManager&) = delete;
        CanvasManager(CanvasManager&&) = delete;
        CanvasManager& operator=(CanvasManager&&) = delete;
    private:
        friend class AppController;

        CanvasManager();
        ~CanvasManager();

        glm::vec2 windowSize = glm::vec2(3840, 2160);                                           /** @brief < Размер окна */

        Graphics::Camera camera;                                                                /** @brief < Объект камера. Для получения текущего zoomFactor. */
        Graphics::Grid grid;                                                                    /** @brief < Объект сетки. Для отрисовки сетки. */
        std::unordered_map<UUIDv4::UUID, std::unique_ptr<ViewNode>> viewNodes;                  /** @brief < Хеш-Таблица. Ключ - ID логической ноды. Значение - Визуальная нода. */
        std::unordered_map<UUIDv4::UUID, std::unique_ptr<ViewConnection>> viewConnections;      /** @brief < Вектор визуальных соединений. */

        ActiveLinkContext activeLinkContext;
    };
}