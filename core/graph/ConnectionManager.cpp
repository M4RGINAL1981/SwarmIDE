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

    void ConnectionManager::connect(UUIDv4::UUID fromNodeID, UUIDv4::UUID toNodeID, std::string fromPinID, std::string toPinID) {
        
        // Убираем самосвязывание
        if(fromNodeID == toNodeID) return; 

        // Находим ноды через Менеджер
        Nodes::LogicNode* srcNode = graph_manager->findNodeByID(fromNodeID);
        Nodes::LogicNode* dstNode = graph_manager->findNodeByID(toNodeID);
        if(!srcNode || !dstNode) return;

        // Достаем пины по указателю
        LogicPin* srcPin = srcNode->getLogicOutputPinByID(fromPinID);
        LogicPin* dstPin = dstNode->getLogicInputPinByID(toPinID);
        if (!srcPin || !dstPin) return;

        // Проверяем совместимость типов
        if(srcPin->typeName != dstPin->typeName) return;

        // (Будущее) Здесь добавим проверку: если к dstPin уже кто-то подключен - удаляем старую связь!

        Connection connect = {fromNodeID, toNodeID, fromPinID, toPinID};
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