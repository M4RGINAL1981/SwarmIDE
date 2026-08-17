#pragma once
#include "SwarmIDE.hpp"
#include "nodes/base/LogicNode.hpp" 
#include "core/engine/SerializationManager.hpp"
#include "core/graph/GraphManager.hpp"
#include "core/engine/GenerateIDManager.hpp"
#include <unordered_map>
#include <optional>

namespace SwarmIDE::Core {

    /**
     * @struct LogicConnection
     * @brief Структура, описывающая ребро графа (связь между двумя пинами двух разных узлов).
     * @details Хранит в себе метаданные о соединении: ID пина ноды-источника и ID пина ноды-цели.
     * 
     * @see ConnectionManager
     */
    struct LogicConnection {

        /**
         * @brief Конструктор по умолчанию для создания пустого соединения.
         */
        LogicConnection() { id = GenerateIDManager::getInstance().GetNewID(); }

        /**
         * @brief Конструктор для создания соединения между конкретными пинами.
         * @param[in] fromNode ID ноды-источника.
         * @param[in] fromPin  ID пина ноды-источника.
         * @param[in] toNode   ID ноды-цели.
         * @param[in] toPin    ID пина ноды-цели.
         */
        LogicConnection(UUIDv4::UUID fromNode, UUIDv4::UUID fromPin, UUIDv4::UUID toNode, UUIDv4::UUID toPin)
            : fromNodeID(fromNode), fromPinID(fromPin), toNodeID(toNode), toPinID(toPin) {
            id = GenerateIDManager::getInstance().GetNewID();
        }

        UUIDv4::UUID fromNodeID;            /** @brief ID ноды-источника. <*/
        UUIDv4::UUID fromPinID;             /** @brief ID пина ноды-источника. <*/
        UUIDv4::UUID toNodeID;              /** @brief ID ноды-цели. <*/
        UUIDv4::UUID toPinID;               /** @brief ID пина ноды-цели. <*/


        /**
         * @brief Получить уникальный идентификатор соединения.
         * @return UUIDv4::UUID уникальный идентификатор соединения.
         */
        UUIDv4::UUID getID() const { return id; } 

        /**
         * @brief Загрузка соединения в JSON
         */
        json serializeLogicConnection() const;
        
        /**
         * @brief Загрузка соединения из JSON
         * @param[in] jsonLogicConnection JSON файл с логическим соединением
         */
        void deserializeLogicConnection(json jsonLogicConnection);

        /**
         * @brief Оператор сравнения двух соединений.
         * @details Используется для поиска конкретной связи в векторе при её удалении или валидации.
         * Связи считаются идентичными, если совпадают оба ID пинов. 
         * Также данный оператор работает и для зеркальных соединений
         * 
         * @param[in] other Другое соединение для сравнения.
         * @return          true Если соединения полностью совпадают или является зеркальным.
         * @return          false Если есть хотя бы одно различие.
         */
        bool operator==(const LogicConnection& other) const;
    
    private:
        UUIDv4::UUID id; /** @brief < Внутренний уникальный идентификатор соединения. Используется для сериализации и отрисовки. */
    };

    /**
     * @class ConnectionManager
     * @brief Менеджер управления всеми связями в вычислительном графе (Синглтон).
     * @details Отвечает за создание, валидацию и уничтожение логических ребер между пинами.
     * Снабжает `GraphManager` информацией о топологии сети для последующего обхода,
     * а также передает списки соединений в UI-слой (`UI::ConnectionDrawer`) для их отрисовки на холсте.
     * 
     * @see Connection
     * @see SwarmIDE::Core::GraphManager
     */
    class ConnectionManager {
    public:
        /**
         * @brief Глобальная точка доступа к единственному экземпляру ConnectionManager.
         * @return ConnectionManager& Ссылка на синглтон менеджера соединений.
         */
        static ConnectionManager& getInstance();

        /**
         * @brief Создать новую логическую связь между узлами в графе.
         * @details Метод фиксирует соединение во внутреннем реестре. При запуске вычислений
         * данные из выходного пина источника будут автоматически прокинуты во входной пин цели.
         * 
         * @param[in] fromNodeID                 ID ноды-источника.
         * @param[in] fromPinID                  Идентификатор порта нodы-источника.
         * @param[in] toNodeID                   ID нodы-цели.
         * @param[in] toPinID                    Идентификатор порта нodы-цели.
         * @return std::optional<UUIDv4::UUID>   Возвращает уникальный идентификатор соединения
         * 
         * @see disconnect()
         * @see Nodes::LogicNode
         * @see Core::LogicPin
         */
        std::optional<UUIDv4::UUID> connect(UUIDv4::UUID fromNodeID, UUIDv4::UUID fromPinID, UUIDv4::UUID toNodeID, UUIDv4::UUID toPinID);

        /**
         * @brief Создать новую логическую связь между узлами в графе.
         * @details Метод фиксирует соединение во внутреннем реестре. При запуске вычислений
         * данные из выходного пина источника будут автоматически прокинуты во входной пин цели.
         * 
         * @param conn                         Указатель на соединение
         * @return std::optional<UUIDv4::UUID> Возвращает уникальный идентификатор соединения
         */
        std::optional<UUIDv4::UUID> connect(const LogicConnection& conn);
       
        /**
         * @brief Разорвать конкретную логическую связь в графе.
         * @details Удаляет ребро из общего списка. Обычно вызывается, когда пользователь 
         * кликает правой кнопкой мыши по линии связи на холсте или удаляет линию.
         * 
         * @param[in] connectionID ID соединения, которое необходимо разорвать.
         */
        void disconnect(UUIDv4::UUID connectionID);

        /**
         * @brief Возвращает список всех исходящих связей для конкретного узла.
         * @param[in] nodeID UUID интересующего узла.
         * @return std::vector<LogicConnection*> Вектор указателей на связи, где данный узел выступает источником (`fromNodeID`).
         */
        std::vector<LogicConnection*> getOutgoingConnectionsForNode(UUIDv4::UUID nodeID);

        /**
         * @brief Возвращает список всех входящих связей для конкретного узла.
         * @param[in] nodeID UUID интересующего узла.
         * @return std::vector<LogicConnection*> Вектор указателей на связи, где данный узел выступает целью (`toNodeID`).
         */
        std::vector<LogicConnection*> getIncomingConnectionsForNode(UUIDv4::UUID nodeID);

        /**
         * @brief Возвращает абсолютно все связи (как входящие, так и исходящие), подключенные к узлу.
         * @param[in] nodeID UUID узла.
         * @return std::vector<LogicConnection*> Вектор указателей на все ребра, привязанные к объекту.
         */
        std::vector<LogicConnection> getAllConnectionsForNode(UUIDv4::UUID nodeID);

        /**
         * @brief Получить прямой доступ ко всему списку связей текущей сцены.
         * @return std::vector<LogicConnection>& Ссылка на внутренний контейнер ребер графа.
         */
        std::unordered_map<UUIDv4::UUID, LogicConnection>& getAllConnections() { return connections; }
        
        /**
         * @brief Ищет соединение по её аргументам. Если соединение не найдено - возвращает nullptr
         * 
         * @param[in] fromNodeID ID ноды-источника.
         * @param[in] fromPinID  Идентификатор порта нodы-источника.
         * @param[in] toNodeID   ID нodы-цели.
         * @param[in] toPinID    Идентификатор порта нodы-цели.
         * @return Connection*   Указатель на соединение
         */
        LogicConnection* findConnection(UUIDv4::UUID fromNodeID, UUIDv4::UUID fromPinID, UUIDv4::UUID toNodeID, UUIDv4::UUID toPinID);

        /**
         * @brief Ищет соединение по её ID. Если соединение не найдено - возвращает nullptr
         * 
         * @param[in] connectionID ID соединения.
         * @return Connection* Указатель на соединение
         */
        LogicConnection* findConnection(UUIDv4::UUID connectionID);

        /**
         * @brief Извлечь соединение.
         *
         * @param connectionID  ID соединения.
         * @return Core::LogicConnection Соединение.
         */
        std::optional<LogicConnection> extractLogicConnection(UUIDv4::UUID connectionID);

        /**
         * @brief Каскадное удаление всех связей, подключенных к конкретному узлу.
         * @details Автоматически вызывается из `GraphManager::removeNode()`. Гарантирует, что на сцене 
         * не останется "висящих в воздухе" стрелок и сломанных указателей после удаления ноды.
         * 
         * @param[in] nodeID UUID удаляемого с холста объекта.
         */
        void removeConnectionsForNode(UUIDv4::UUID nodeID);

        /**
         * @brief Полностью очистить реестр соединений проекта.
         */
        void removeAllConnections() { connections.clear(); }

        // Запрещаем дублирование синглтона менеджера связей
        ConnectionManager(const ConnectionManager&) = delete;
        ConnectionManager& operator=(const ConnectionManager&) = delete;
        ConnectionManager(ConnectionManager&&) = delete;
        ConnectionManager& operator=(ConnectionManager&&) = delete;

    private:
        friend class SerializationManager; /**< Доступ для сохранения/загрузки ребер графа в JSON конфигурацию. */

        ConnectionManager() { Core::LoggingManager::Info("[CONNECTION_MANAGER]: Constructor"); }
        ~ConnectionManager();
     
        std::unordered_map<UUIDv4::UUID, LogicConnection> connections; /** @brief Внутренний хэш-таблица для быстрого поиска соединений по их уникальному идентификатору. */
    };
}