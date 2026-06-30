#pragma once
#include "object/BaseObject.hpp" 

namespace SwarmIDE::Core {

    struct Connection {
        int sourceObjectID;  // ID Объекта-Источника
        int targetObjectID;  // ID Объекта-Цели

        std::string sourcePinID;     // ID Пина-Источника
        std::string targetPinID;     // ID Пина-Цели

        bool operator==(const Connection& other) const;
    };

    class ConnectionManager{
    public:
        // Глобальная точка доступа к единственному экземпляру
        static ConnectionManager& getInstance();

        void connect(int sObjectID, int tObjectID, SwarmIDE::UI::AnchorPin sPin, SwarmIDE::UI::AnchorPin tPin);
        void disconnect(Connection connect);

        std::vector<Connection> getConnection() { return connection; }
        std::vector<int> getConnectedOutputs(int ObjectID);

        ConnectionManager(const ConnectionManager&) = delete;
        ConnectionManager& operator=(const ConnectionManager&) = delete;
        ConnectionManager(ConnectionManager&&) = delete;
        ConnectionManager& operator=(ConnectionManager&&) = delete;

    private:
        ConnectionManager() = default;
        ~ConnectionManager() = default;

        std::vector<Connection> connection;      // Список связей
    };
}