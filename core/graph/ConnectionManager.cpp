#include "SwarmIDE.hpp"
#include "core/graph/ConnectionManager.hpp"

namespace {
    bool isValidConnection(const SwarmIDE::Core::LogicPin* srcPin, const SwarmIDE::Core::LogicPin* dstPin) {
        if (!srcPin || !dstPin) return false;                           // Проверка на существование 
        if (srcPin->dataType != dstPin->dataType) return false;         // Проверка на Соответсвие типам данных пинов
        if (srcPin->direction == dstPin->direction) return false;       // Проверка на Соответсвие направлению пинов
        if (srcPin->id != dstPin->id) return false;                     // Проверка на Соответсвие типам пинов (PinID)
        return true;
    }
}

namespace SwarmIDE::Core {

    bool Connection::operator==(Connection other) {
        if(fromNodeID == other.fromNodeID &&
        toNodeID == other.toNodeID &&
        fromPinID == other.fromPinID &&
        toPinID == other.toPinID) return true;
        return false;
    }

    Connection* ConnectionManager::connect(UUIDv4::UUID fromNodeID, PinID fromPinID, int fromIndex, UUIDv4::UUID toNodeID, PinID toPinID, int toIndex) {
        
        // Убираем самосвязывание
        if(fromNodeID == toNodeID) return nullptr; 

        // Находим логические ноды
        Nodes::LogicNode* srcNode = GraphManager::getInstance().findNodeByID(fromNodeID);
        Nodes::LogicNode* dstNode = GraphManager::getInstance().findNodeByID(toNodeID);

        // Если ноды не обнаружены - возвращаем пустышку
        if(!srcNode || !dstNode) return nullptr;

        // Находим логические пины нод
        LogicPin* srcPin = srcNode->findPin(fromPinID, Direction::Output, fromIndex);
        LogicPin* dstPin = dstNode->findPin(toPinID, Direction::Input, toIndex);

        // Проверка валидации пинов
        if (!isValidConnection(srcPin, dstPin)) return nullptr;

        // Если количество подключений не соблюдено - возвращем пустышку
        if((srcPin->maxConnections != -1 && srcPin->currentConnections >= srcPin->maxConnections) || 
           (dstPin->maxConnections != -1 && dstPin->currentConnections >= dstPin->maxConnections)) return nullptr;

        // Увеличиваем счётчик подключений к пинам
        srcPin->currentConnections++;
        dstPin->currentConnections++;

        // Создаём соединение
        Connection connect = {fromNodeID, fromPinID, fromIndex, toNodeID, toPinID, toIndex};

        // Проверяем на уже существование соединения
        auto it = std::find(connection.begin(), connection.end(), connect);
        if(it == connection.end()) {
            LoggingManager::Info("[CREATE]: Connection is create");
            connection.push_back(connect);
            return &connection.back(); 
        }
        
        return &(*it);
    }

    void ConnectionManager::disconnect(Connection* conn) {
        if(!conn) return;
        auto it = std::find(connection.begin(), connection.end(), *conn);
        if(it != connection.end()) connection.erase(it);

        LoggingManager::Info("[DELETE]: Connection is disconect");
    }

    std::vector<Connection*> ConnectionManager::getOutgoingConnectionsForNode(UUIDv4::UUID nodeID) {
        std::vector<Connection*> outgoing;
        for(auto& conn : connection) {
            if(conn.fromNodeID == nodeID) {
                outgoing.push_back(&conn);
            }
        }
        return outgoing;
    }

    std::vector<Connection*> ConnectionManager::getIncomingConnectionsForNode(UUIDv4::UUID nodeID) {
        std::vector<Connection*> incoming;
        for(auto& conn : connection) {
            if(conn.toNodeID == nodeID) {
               incoming.push_back(&conn);
            }
        }
        return incoming;
    }

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
        std::erase_if(connection, [nodeID](const Connection& conn) {
            return conn.fromNodeID == nodeID || conn.toNodeID == nodeID;
        });
    }

    ConnectionManager& ConnectionManager::getInstance() {
        static ConnectionManager instance;
        return instance;
    }

    ConnectionManager::~ConnectionManager() {
        Core::LoggingManager::Info("[CONNECTION_MANAGER]: Destructor"); 
        Core::LoggingManager::Info("[CONNECTION]: Number of connections: ", connection.size());
        connection.clear();
    }
}