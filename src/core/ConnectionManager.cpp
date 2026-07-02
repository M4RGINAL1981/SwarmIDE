#include "core/ConnectionManager.hpp"
#include "ui/Topology.hpp"

namespace SwarmIDE::Core {

    bool Connection::operator==(const Connection& other) const  {
        if((this->sourceObjectID == other.sourceObjectID) && (this->targetObjectID == other.targetObjectID) &&
           (this->sourcePinID == other.sourcePinID) && (this->targetPinID == other.targetPinID)) { return true; }

        if((this->sourceObjectID == other.targetObjectID) && (this->targetObjectID == other.sourceObjectID) &&
           (this->sourcePinID == other.targetPinID) && (this->targetPinID == other.sourcePinID)) { return true; }
        return false;
    }


    void ConnectionManager::connect (int sObjectID, int tObjectID, SwarmIDE::UI::AnchorPin sPin, SwarmIDE::UI::AnchorPin tPin) {
        if(sPin.id == tPin.id && sObjectID == tObjectID) return; 
        if(sPin.type == SwarmIDE::UI::PinType::Input && tPin.type == SwarmIDE::UI::PinType::Input) return;
        if(sPin.type == SwarmIDE::UI::PinType::Output && tPin.type == SwarmIDE::UI::PinType::Output) return;

        Connection connect = {sObjectID, tObjectID, sPin.id, tPin.id};
        auto it = std::find(connection.begin(), connection.end(), connect);
        
        if(it == connection.end()) connection.push_back(connect);
    }

    void ConnectionManager::disconnect (Connection connect) {
        auto it = std::find(connection.begin(), connection.end(), connect);

        if(it != connection.end()) connection.erase(it);
    }

    std::vector<int> ConnectionManager::getConnectedOutputs(int ObjectID) {
        std::vector<int> connectedOutputs;

        for(auto connect : connection){
            if(connect.sourceObjectID == ObjectID) connectedOutputs.push_back(connect.targetObjectID);
        }
        return connectedOutputs;
    }

    void ConnectionManager::removeConnectionsForObject(int objectID) {
        // Бежим по вектору связей и удаляем все, где объект был источником или целью
        connection.erase(
            std::remove_if(connection.begin(), connection.end(),
                [objectID](const Connection& conn) {
                    return conn.sourceObjectID == objectID || conn.targetObjectID == objectID;
                }), 
            connection.end()
        );
    }

    ConnectionManager& ConnectionManager::getInstance() {
        static ConnectionManager instance;
        return instance;
    }
}