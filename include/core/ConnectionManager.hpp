#pragma once
#include "core/graph/nodes/LogicNode.hpp" 
#include "core/SerializationManager.hpp"
#include "core/graph/GraphManager.hpp"

namespace SwarmIDE::Core {

    struct Connection {
        UUIDv4::UUID fromNodeID;               // ID Ноды-Источника
        UUIDv4::UUID toNodeID;                 // ID Ноды-Цели

        std::string fromPinID;        // ID Пина-Источника
        std::string toPinID;          // ID Пина-Цели

        bool operator==(Connection other);
    };

    class ConnectionManager{
    public:
        // Глобальная точка доступа к единственному экземпляру
        static ConnectionManager& getInstance();

        // Создание связи
        void connect(UUIDv4::UUID fromNodeID, UUIDv4::UUID toNodeID, std::string fromPinID, std::string toPinID);
       
        // Разрыв связи
        void disconnect(Connection connect);

        std::vector<Connection*> getOutgoingConnectionsForNode(UUIDv4::UUID nodeID);    // Вернуть все связи, где объект-источник
        std::vector<Connection*> getIncomingConnectionsForNode(UUIDv4::UUID nodeID);    // Вернуть все связи, где объект-цель
        std::vector<Connection*> getAllConnectionsForNode(UUIDv4::UUID nodeID);         // Вернуть все связи с объектом

        std::vector<Connection>& getAllConnections() { return connection; }             // Получить все связи           
        
        void removeConnectionsForNode(UUIDv4::UUID nodeID);                             // Удалить все связи с данным объктом
        void removeAllConnections() { connection.clear(); }                             // Удалить все связи

        // Подключаем Менеджера графа
        void SetGraphManager(Graph::GraphManager* grph_manager) { graph_manager = grph_manager; }

        ConnectionManager(const ConnectionManager&) = delete;
        ConnectionManager& operator=(const ConnectionManager&) = delete;
        ConnectionManager(ConnectionManager&&) = delete;
        ConnectionManager& operator=(ConnectionManager&&) = delete;

    private:
        friend SerializationManager;

        ConnectionManager() = default;
        ~ConnectionManager() = default;

        Graph::GraphManager* graph_manager = nullptr;
     
        std::vector<Connection> connection;                  // Список связей
    };
}