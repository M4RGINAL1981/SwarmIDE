#include "SwarmIDE.hpp"
#include "core/graph/ConnectionManager.hpp"

namespace SwarmIDE::Core {

    bool Connection::operator==(Connection other) {
        if(fromNodeID == other.fromNodeID &&
        toNodeID == other.toNodeID &&
        fromPinID == other.fromPinID &&
        toPinID == other.toPinID) return true;
        return false;
    }

    void ConnectionManager::connect(UUIDv4::UUID fromNodeID, PinID fromPinID, int fromIndex, UUIDv4::UUID toNodeID, PinID toPinID, int toIndex) {
        
        // Убираем самосвязывание
        if(fromNodeID == toNodeID) return; 

        // Находим ноды через Менеджер
        Nodes::LogicNode* srcNode = graph_manager->findNodeByID(fromNodeID);
        Nodes::LogicNode* dstNode = graph_manager->findNodeByID(toNodeID);
        if(!srcNode || !dstNode) return;

        // Достаем пины по указателю
        LogicPin* srcPin = srcNode->findPin(fromPinID, Direction::Output, fromIndex);
        LogicPin* dstPin = dstNode->findPin(toPinID, Direction::Input, toIndex);

        // Проверяем существование пинов
        if (!srcPin || !dstPin) return;

        // Проверяем совместимость типов
        if(srcPin->dataType != dstPin->dataType) return;

        // Проверка на соответсвия ограничениям к соединениям
        if((srcPin->currentConnections >= srcPin->maxConnections || srcPin->maxConnections == -1) ||
           (dstPin->currentConnections >= dstPin->maxConnections || dstPin->maxConnections == -1)) return;

        // (Будущее) Здесь добавим проверку: если к dstPin уже кто-то подключен - удаляем старую связь!

        Connection connect = {fromNodeID, fromPinID, fromIndex, toNodeID, toPinID, toIndex};
        auto it = std::find(connection.begin(), connection.end(), connect);
        
        if(it == connection.end()) connection.push_back(connect);
    }

    void ConnectionManager::disconnect(Connection conn) {
        auto it = std::find(connection.begin(), connection.end(), conn);

        if(it != connection.end()) connection.erase(it);
    }

    // Вернуть все связи, где нода-источник
    std::vector<Connection*> ConnectionManager::getOutgoingConnectionsForNode(UUIDv4::UUID nodeID) {
        std::vector<Connection*> outgoing;
        for(auto& conn : connection) {
            if(conn.fromNodeID == nodeID) {
                outgoing.push_back(&conn);
            }
        }
        return outgoing;
    }

    // Вернуть все связи, где нода-цель
    std::vector<Connection*> ConnectionManager::getIncomingConnectionsForNode(UUIDv4::UUID nodeID) {
        std::vector<Connection*> incoming;
        for(auto& conn : connection) {
            if(conn.toNodeID == nodeID) {
               incoming.push_back(&conn);
            }
        }
        return incoming;
    }

    // Вернуть все связи с нодой
    std::vector<Connection*> ConnectionManager::getAllConnectionsForNode(UUIDv4::UUID nodeID) {
        std::vector<Connection*> result;
        
        for(auto& conn : connection) {
            if(conn.fromNodeID == nodeID || conn.toNodeID == nodeID) {
                result.push_back(&conn);
            }
        }
        return result;
    }

    void ConnectionManager::removeConnectionsForNode(UUIDv4::UUID nodeID) {
        for(auto& conn : connection){
            if(conn.fromNodeID == nodeID) disconnect(conn);
            if(conn.toNodeID == nodeID) disconnect(conn);
        }
    }

    ConnectionManager& ConnectionManager::getInstance() {
        static ConnectionManager instance;
        return instance;
    }
}