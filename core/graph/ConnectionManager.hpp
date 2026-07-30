#pragma once
#include "SwarmIDE.hpp"
#include "nodes/base/LogicNode.hpp" 
#include "core/engine/SerializationManager.hpp"
#include "core/graph/GraphManager.hpp"

namespace SwarmIDE::Core {

    /**
     * @struct Connection
     * @brief Структура, описывающая ребро графа (связь между двумя пинами двух разных узлов).
     * @details Хранит в себе полные метаданные о соединении: от какого узла и пина (выхода) 
     * данные уходят, и в какой узел и пин (вход) они направляются.
     * 
     * @see ConnectionManager
     */
    struct Connection {
        UUIDv4::UUID fromNodeID;      /**< Уникальный UUID узла-источника (откуда идет связь). */
        PinID fromPinID;              /**< Идентификатор выходного пина узла-источника. */
        int fromIndex = 0;            /**< Индекс выходного пина */

        UUIDv4::UUID toNodeID;        /**< Уникальный UUID узла-цели (куда приходит связь). */
        PinID toPinID;                /**< Идентификатор входного пина узла-цели. */
        int toIndex = 0;              /**< Индекс входного пина */

        /**
         * @brief Оператор сравнения двух соединений.
         * @details Используется для поиска конкретной связи в векторе при её удалении или валидации.
         * Связи считаются идентичными, если совпадают все 4 идентификатора (ноды и пины). 
         * Также данный оператор работает и для зеркальных соединений
         * 
         * @param[in] other Другое соединение для сравнения.
         * @return          true Если соединения полностью совпадают или является зеркальным.
         * @return          false Если есть хотя бы одно различие.
         */
        bool operator==(Connection other);
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
         * @param[in] fromNodeID UUID узла-источника данных.
         * @param[in] fromPinID  Идентификатор порта ноды-источника.
         * @param[in] fromIndex  Индекс порта ноды-источника.
         * @param[in] toNodeID   UUID узла-получателя данных.
         * @param[in] toPinID    Идентификатор порта ноды-цели.
         * @param[in] toIndex    Индекс порта ноды-цели.
         * @return Connection*   Указатель на соединение
         * 
         * @see disconnect()
         * @see Nodes::LogicNode
         * @see Core::LogicPin
         */
        Connection* connect(UUIDv4::UUID fromNodeID, PinID fromPinID, int fromIndex,
                            UUIDv4::UUID toNodeID, PinID toPinID, int toIndex);

        /**
         * @brief Создать новую логическую связь между узлами в графе.
         * @details Метод фиксирует соединение во внутреннем реестре. При запуске вычислений
         * данные из выходного пина источника будут автоматически прокинуты во входной пин цели.
         * 
         * @param conn         Указатель на соединение
         * @return Connection* Возвращает само соединение
         */
        Connection* connect(Connection* conn);
       
        /**
         * @brief Разорвать конкретную логическую связь в графе.
         * @details Удаляет ребро из общего списка. Обычно вызывается, когда пользователь 
         * кликает правой кнопкой мыши по линии связи на холсте или удаляет линию.
         * 
         * @param[in] conn Указатель на объект связи, которую необходимо разорвать.
         */
        void disconnect(Connection* conn);

        /**
         * @brief Возвращает список всех исходящих связей для конкретного узла.
         * @param[in] nodeID UUID интересующего узла.
         * @return std::vector<Connection*> Вектор указателей на связи, где данный узел выступает источником (`fromNodeID`).
         */
        std::vector<Connection*> getOutgoingConnectionsForNode(UUIDv4::UUID nodeID);

        /**
         * @brief Возвращает список всех входящих связей для конкретного узла.
         * @param[in] nodeID UUID интересующего узла.
         * @return std::vector<Connection*> Вектор указателей на связи, где данный узел выступает целью (`toNodeID`).
         */
        std::vector<Connection*> getIncomingConnectionsForNode(UUIDv4::UUID nodeID);

        /**
         * @brief Возвращает абсолютно все связи (как входящие, так и исходящие), подключенные к узлу.
         * @param[in] nodeID UUID узла.
         * @return std::vector<Connection*> Вектор указателей на все ребра, привязанные к объекту.
         */
        std::vector<Connection> getAllConnectionsForNode(UUIDv4::UUID nodeID);

        /**
         * @brief Получить прямой доступ ко всему списку связей текущей сцены.
         * @return std::vector<Connection>& Ссылка на внутренний контейнер ребер графа.
         */
        std::vector<Connection>& getAllConnections() { return connection; }
        
        /**
         * @brief Ищет соединение по её аргументам. Если соединение не найдено - возвращает nullptr
         * 
         * @param[in] fromNodeID UUID узла-источника данных.
         * @param[in] fromPinID  Идентификатор порта ноды-источника.
         * @param[in] fromIndex  Индекс порта ноды-источника.
         * @param[in] toNodeID   UUID узла-получателя данных.
         * @param[in] toPinID    Идентификатор порта ноды-цели.
         * @param[in] toIndex    Индекс порта ноды-цели.
         * @return Connection*   Указатель на соединение
         */
        Connection* findConnection(UUIDv4::UUID fromNodeID, PinID fromPinID, int fromIndex,
                                   UUIDv4::UUID toNodeID, PinID toPinID, int toIndex);

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
        void removeAllConnections() { connection.clear(); }

        // Запрещаем дублирование синглтона менеджера связей
        ConnectionManager(const ConnectionManager&) = delete;
        ConnectionManager& operator=(const ConnectionManager&) = delete;
        ConnectionManager(ConnectionManager&&) = delete;
        ConnectionManager& operator=(ConnectionManager&&) = delete;

    private:
        friend class SerializationManager; /**< Доступ для сохранения/загрузки ребер графа в JSON конфигурацию. */

        ConnectionManager() { Core::LoggingManager::Info("[CONNECTION_MANAGER]: Constructor"); }
        ~ConnectionManager();
     
        std::vector<Connection> connection;    /**< Внутренний вектор, хранящий все ребра текущей сцены. */
    };
}