#include "SwarmIDE.hpp"
#include "core/graph/ConnectionManager.hpp"

namespace {
    bool isValidConnection(const SwarmIDE::Core::LogicPin* srcPin, const SwarmIDE::Core::LogicPin* dstPin) {
        if (!srcPin || !dstPin) return false;                           // Проверка на существование 
        if (srcPin->dataType != dstPin->dataType) return false;         // Проверка на Соответсвие типам данных пинов
        if (srcPin->direction == dstPin->direction) return false;       // Проверка на Соответсвие направлению пинов
        if (srcPin->type != dstPin->type) return false;                 // Проверка на Соответсвие типов пинов
        return true;
    }
}

namespace SwarmIDE::Core {

    bool LogicConnection::operator==(const LogicConnection& other) const {
        if(fromPinID == other.fromPinID && toPinID == other.toPinID && fromNodeID == other.fromNodeID && toNodeID == other.toNodeID) return true;
        if(fromPinID == other.toPinID && toPinID == other.fromPinID && fromNodeID == other.toNodeID && toNodeID == other.fromNodeID) return true;
        return false;
    }


    json LogicConnection::serializeLogicConnection() const {
        json jsonLogicConnection;

        jsonLogicConnection["id"] = id.str();

        jsonLogicConnection["fromPinID"] = fromPinID.str();
        jsonLogicConnection["toPinID"] = toPinID.str();
        jsonLogicConnection["fromNodeID"] = fromNodeID.str();
        jsonLogicConnection["toNodeID"] = toNodeID.str();
        
        return jsonLogicConnection;
    }

    void LogicConnection::deserializeLogicConnection(json jsonLogicConnection) {
        id = UUIDv4::UUID(jsonLogicConnection["id"].get<std::string>());
        fromPinID = UUIDv4::UUID(jsonLogicConnection["fromPinID"].get<std::string>());
        toPinID = UUIDv4::UUID(jsonLogicConnection["toPinID"].get<std::string>());
        fromNodeID = UUIDv4::UUID(jsonLogicConnection["fromNodeID"].get<std::string>());
        toNodeID = UUIDv4::UUID(jsonLogicConnection["toNodeID"].get<std::string>());
    }

    std::optional<UUIDv4::UUID> ConnectionManager::connect(UUIDv4::UUID fromNodeID, UUIDv4::UUID fromPinID, UUIDv4::UUID toNodeID, UUIDv4::UUID toPinID) {

        if (findConnection(fromNodeID, fromPinID, toNodeID, toPinID) != nullptr) {
            return std::nullopt; // Связь уже существует!
        }

        // Убираем самосвязывание
        if(fromNodeID == toNodeID) return std::nullopt; 

        // Находим логические ноды
        Nodes::LogicNode* srcNode = GraphManager::getInstance().findNodeByID(fromNodeID);
        Nodes::LogicNode* dstNode = GraphManager::getInstance().findNodeByID(toNodeID);

        // Если ноды не обнаружены - возвращаем пустышку
        if(!srcNode || !dstNode) return std::nullopt;

        // Находим логические пины нод
        LogicPin* srcPin = srcNode->findPin(fromPinID);
        LogicPin* dstPin = dstNode->findPin(toPinID);

        // Проверка валидации пинов
        if (!isValidConnection(srcPin, dstPin)) return std::nullopt;

        // Если количество подключений не соблюдено - возвращем пустышку
        if((srcPin->maxConnections != -1 && srcPin->currentConnections >= srcPin->maxConnections) || 
           (dstPin->maxConnections != -1 && dstPin->currentConnections >= dstPin->maxConnections)) return std::nullopt;

        // Увеличиваем счётчик подключений к пинам
        srcPin->currentConnections++;
        dstPin->currentConnections++;

        // Создаём соединение
        LogicConnection connect = {fromNodeID, fromPinID, toNodeID, toPinID };

        LoggingManager::Info("[CREATE]: Connection is create");
        connections[connect.getID()] = connect;
        return connect.getID();
    }

    std::optional<UUIDv4::UUID> ConnectionManager::connect(const LogicConnection& conn){
        if(conn.fromNodeID == conn.toNodeID) return std::nullopt; 

        // Находим логические ноды
        Nodes::LogicNode* srcNode = GraphManager::getInstance().findNodeByID(conn.fromNodeID);
        Nodes::LogicNode* dstNode = GraphManager::getInstance().findNodeByID(conn.toNodeID);

        // Если нodы не обнаружены - возвращаем пустышку
        if(!srcNode || !dstNode) return std::nullopt;

        // Находим логические пины нod
        LogicPin* srcPin = srcNode->findPin(conn.fromPinID);
        LogicPin* dstPin = dstNode->findPin(conn.toPinID);

        // Проверка валидации пинов
        if (!isValidConnection(srcPin, dstPin)) return std::nullopt;

        // Если количество подключений не соблюдено - возвращем пустышку
        if((srcPin->maxConnections != -1 && srcPin->currentConnections >= srcPin->maxConnections) || 
           (dstPin->maxConnections != -1 && dstPin->currentConnections >= dstPin->maxConnections)) return std::nullopt;

        // Проверяем на уже существование соединения
        if(!connections.contains(conn.getID())) {
            LoggingManager::Info("[CREATE]: Connection is create");
            // Увеличиваем счётчик подключений к пинам
            srcPin->currentConnections++;
            dstPin->currentConnections++;
            connections[conn.getID()] = conn;
        }
        return conn.getID();
    } 

    void ConnectionManager::disconnect(UUIDv4::UUID connectionID) {
        auto conn = findConnection(connectionID);
        if(!conn) return;

        if(connections.contains(connectionID)) {
            
            auto srcNode = GraphManager::getInstance().findNodeByID(conn->fromNodeID);
            auto dstNode = GraphManager::getInstance().findNodeByID(conn->toNodeID);
            
            if (srcNode && dstNode) {
                auto srcPin = srcNode->findPin(conn->fromPinID);
                auto dstPin = dstNode->findPin(conn->toPinID);
                
                if (srcPin && srcPin->currentConnections > 0) srcPin->currentConnections--;
                if (dstPin && dstPin->currentConnections > 0) dstPin->currentConnections--;
            }

            connections.erase(connectionID);
            LoggingManager::Info("[DELETE]: Connection is disconect");
        }
    }

    std::vector<LogicConnection*> ConnectionManager::getOutgoingConnectionsForNode(UUIDv4::UUID nodeID) {
        std::vector<LogicConnection*> outgoing;
        for(auto& [id, connection] : connections) {
            if(connection.fromNodeID == nodeID) {
                outgoing.push_back(&connection);
            }
        }
        return outgoing;
    }

    std::vector<LogicConnection*> ConnectionManager::getIncomingConnectionsForNode(UUIDv4::UUID nodeID) {
        std::vector<LogicConnection*> incoming;
        for(auto& [id, connection] : connections) {
            if(connection.toNodeID == nodeID) {
               incoming.push_back(&connection);
            }
        }
        return incoming;
    }

    std::vector<Core::LogicConnection> ConnectionManager::getAllConnectionsForNode(UUIDv4::UUID nodeID) {
        std::vector<LogicConnection> result;
        for(auto& [id, connection] : connections) {
            if(connection.fromNodeID == nodeID || connection.toNodeID == nodeID) {
                result.push_back(connection);
            }
        }
        return result;
    }       

    LogicConnection* ConnectionManager::findConnection(UUIDv4::UUID fromNodeID, UUIDv4::UUID fromPinID, UUIDv4::UUID toNodeID, UUIDv4::UUID toPinID) {
        for(auto& [id, connection] : connections) {
            if((connection.fromPinID == fromPinID && connection.toPinID == toPinID) ||
            (connection.fromPinID == toPinID && connection.toPinID == fromPinID)) {
                return &connection;
            }
        }
        return nullptr;
    }

    LogicConnection* ConnectionManager::findConnection(UUIDv4::UUID connectionID) {
        if(connections.contains(connectionID)) return &connections[connectionID];
        return nullptr;
    }

    std::optional<LogicConnection> ConnectionManager::extractLogicConnection(UUIDv4::UUID connectionID) {
        if (connections.contains(connectionID)) {
            auto conn = connections[connectionID];
            disconnect(connectionID);
            return conn;
        }
        return std::nullopt;
    }

    void ConnectionManager::removeConnectionsForNode(UUIDv4::UUID nodeID) {
        std::erase_if(connections, [nodeID](const std::pair<UUIDv4::UUID, LogicConnection>& kvp) {
            const LogicConnection& conn = kvp.second;
            return conn.fromNodeID == nodeID || conn.toNodeID == nodeID;
        });
    }

    ConnectionManager& ConnectionManager::getInstance() {
        static ConnectionManager instance;
        return instance;
    }

    ConnectionManager::~ConnectionManager() {
        Core::LoggingManager::Info("[CONNECTION_MANAGER]: Destructor"); 
        Core::LoggingManager::Info("[CONNECTION_MANAGER]: LogicConnectionID:");
        for(auto& [id, connection] : connections) {
            Core::LoggingManager::Info("-", id.str());
        }
        connections.clear();
    }
}