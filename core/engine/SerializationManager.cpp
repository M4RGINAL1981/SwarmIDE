#include "SwarmIDE.hpp"
#include "core/engine/SerializationManager.hpp"
#include "nodes/base/LogicNode.hpp"
#include "nodes/core/TextNode.hpp"
#include "nodes/core/ModelNode.hpp"
#include <json.hpp>
#include <fstream>
#include <iostream>
#include <cerrno>
#include <cstring>

using json = nlohmann::json;

namespace SwarmIDE::Core {
    
    SerializationManager& SerializationManager::getInstance() {
        static SerializationManager instance;
        return instance;
    }

    std::string SerializationManager::GetNextAvailableFileName(const std::string& baseName) {
        int counter = 1;
        std::string defaultName = baseName + std::to_string(counter) + ".json";

        while(fs::exists(defaultName)) {
            counter++;
            defaultName = baseName + std::to_string(counter) + ".json";
        }

        return defaultName;
    }
   
    void SerializationManager::SaveGraph(const std::string& filepath) {

        LoggingManager::Info("The graph is saved");

        json root;

        root["nodes"] = json::array();
        root["connections"] = json::array();

        auto& logicNodes = GraphManager::getInstance().getNodes();

        // Запись в файл информацию о нодах
        for(auto& [id, logicNode] : logicNodes) root["nodes"].push_back(logicNode->serializeNode());            

        // Запись в файл информации о подключениях
        auto& logicConnections = ConnectionManager::getInstance().getAllConnections();
        for(auto& [id, logicConnection] : logicConnections) {
            json connJson;
            
            connJson["id"] = logicConnection.getID().str();
            connJson["fromNodeID"] = logicConnection.fromNodeID.str();
            connJson["fromPinID"] = logicConnection.fromPinID.str();
            connJson["toNodeID"] = logicConnection.toNodeID.str();
            connJson["toPinID"] = logicConnection.toPinID.str();

            root["connections"].push_back(connJson);
        }

        std::ofstream file(filepath);
        if (!file.is_open()) {
            LoggingManager::Error("Save Graph", std::strerror(errno), " (Path: ", filepath, ")");
            return;
        }
        file << root.dump(4);
        file.close();

        LoggingManager::Info("The graph has been preserved.");
    }

    void SerializationManager::LoadGraph(const std::string& filepath) {

        LoggingManager::Info("The Graph is loading.");

        std::ifstream file(filepath);
        if (!file.is_open()) {
            LoggingManager::Error("LoadGraph Error: ", std::strerror(errno), " (Path: ", filepath, ")");
            return;
        }

        json root;
        file >> root;
        
        // Очистка графа перед загрузкой
        SerializationManager::ClearGraph();

        if(root.contains("nodes")) {
            for (const auto& nodeJson : root["nodes"]) {
                std::unique_ptr<Nodes::LogicNode> newNode = GraphRegistry::getInstance().createNode(nodeJson["type"]);
                newNode->deserializeNode(nodeJson);
            }
        }

        if(root.contains("connections")) {
            for (const auto& connJson : root["connections"]) {

                // Явно конвертируем json-строку обратно в объект UUID
                UUIDv4::UUID fromNodeID = UUIDv4::UUID(connJson["fromNodeID"].get<std::string>());
                UUIDv4::UUID fromPinID = UUIDv4::UUID(connJson["fromPinID"].get<std::string>());
                UUIDv4::UUID toNodeID = UUIDv4::UUID(connJson["toNodeID"].get<std::string>());
                UUIDv4::UUID toPinID = UUIDv4::UUID(connJson["toPinID"].get<std::string>());

                ConnectionManager::getInstance().connect(fromNodeID, fromPinID, toNodeID, toPinID);
            }
        }

        LoggingManager::Info("The Graph has been loaded.");
    }

    void SerializationManager::ClearGraph() {

        LoggingManager::Info("The Graph is cleaning.");

        ConnectionManager::getInstance().removeAllConnections();
        GraphManager::getInstance().removeAllNodes();
        
        LoggingManager::Info("The Graph has been cleaned.");
    }
}