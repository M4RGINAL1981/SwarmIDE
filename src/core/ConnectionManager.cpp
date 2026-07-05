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


    bool ConnectionManager::isPinOverloaded(const std::string& pinID, int maxConnections) {
        if (maxConnections <= 0) return false;   //  Безлимитный пин
        return pinCounters[pinID] >= maxConnections;
    }


    void ConnectionManager::connect(int sObjectID, int tObjectID, SwarmIDE::UI::AnchorPin& sPin, SwarmIDE::UI::AnchorPin& tPin) {
        if(sPin.id == tPin.id && sObjectID == tObjectID) return; 
        
        // 1. Создаем уникальные глобальные ключи для карты счетчиков
        std::string sKey = makeGlobalKey(sObjectID, sPin.id); // Например: "1_2"
        std::string tKey = makeGlobalKey(tObjectID, tPin.id); // Например: "2_0"

        // 2. Проверяем лимиты по уникальным ключам
        if(isPinOverloaded(sKey, sPin.maxConnections)) return;
        if(isPinOverloaded(tKey, tPin.maxConnections)) return;

        // Битовая оптимизация (остается без изменений)
        if (sPin.type != SwarmIDE::UI::PinType::Universal && tPin.type != SwarmIDE::UI::PinType::Universal) {
            int sVal = static_cast<int>(sPin.type);
            int tVal = static_cast<int>(tPin.type);
            if (((sVal & 0x0F) != (tVal & 0x0F)) || ((sVal & 0x10) == (tVal & 0x10))) return;
        }

        Connection connect = {sObjectID, tObjectID, sPin.id, tPin.id};
        auto it = std::find(connection.begin(), connection.end(), connect);
        
        if(it == connection.end()) {
            connection.push_back(connect);

            outputCache[sObjectID].push_back(tObjectID);
            
            // 3. Инкрементируем счетчики по уникальным ключам
            pinCounters[sKey]++;
            pinCounters[tKey]++;
        }
        
        // Синхронизируем локальные счетчики для UI
        sPin.connectionCount = pinCounters[sKey];
        tPin.connectionCount = pinCounters[tKey];
    }

    void ConnectionManager::disconnect(Connection conn) {
        auto it = std::find(connection.begin(), connection.end(), conn);

        if(it != connection.end()) {
            // Генерируем ключи на основе сохраненных в Connection данных
            std::string sKey = makeGlobalKey(it->sourceObjectID, it->sourcePinID);
            std::string tKey = makeGlobalKey(it->targetObjectID, it->targetPinID);

            if (pinCounters[sKey] > 0) pinCounters[sKey]--;
            if (pinCounters[tKey] > 0) pinCounters[tKey]--;

            auto& vec = outputCache[it->sourceObjectID];
            vec.erase(std::remove(vec.begin(), vec.end(), it->targetObjectID), vec.end());

            connection.erase(it);
        }
    }

    void ConnectionManager::removeConnectionsForObject(int objectID) {
        for (const auto& conn : connection) {
            if (conn.sourceObjectID == objectID || conn.targetObjectID == objectID) {
                std::string sKey = makeGlobalKey(conn.sourceObjectID, conn.sourcePinID);
                std::string tKey = makeGlobalKey(conn.targetObjectID, conn.targetPinID);

                if (pinCounters[sKey] > 0) pinCounters[sKey]--;
                if (pinCounters[tKey] > 0) pinCounters[tKey]--;
            }
        }

        // Очищаем кэш выходов для самого удаляемого объекта
        outputCache.erase(objectID);
        // Удаляем упоминания этого объекта из списков выходов других нод
        for (auto& [sourceID, targets] : outputCache) {
            targets.erase(std::remove(targets.begin(), targets.end(), objectID), targets.end());
        }

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