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

        auto& nodes = GraphManager::getInstance().getNodes();

        // Запись в файл информацию о нодах
        for(auto& node : nodes) root["objects"].push_back(node->serializeNode());            

        // Запись в файл информацию о подключениях
        auto& connection = ConnectionManager::getInstance().getAllConnections();
        for(auto& connect : connection) {
            json connJson;
            connJson["fromNodeID"] = connect.fromNodeID;
            connJson["fromPinID"] = connect.fromPinID;
            connJson["fromIndex"] = connect.fromIndex;
            connJson["toNodeID"] = connect.toNodeID;
            connJson["toPinID"] = connect.toPinID;
            connJson["toIndex"] = connect.toIndex;

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

                UUIDv4::UUID fromNodeID = connJson["fromNodeID"];
                PinID fromPinID = connJson["fromPinID"];
                int fromIndex = connJson["fromIndex"];

                UUIDv4::UUID toNodeID = connJson["toNodeID"];
                PinID toPinID = connJson["toPinID"];
                int toIndex = connJson["toIndex"];

                ConnectionManager::getInstance().connect(fromNodeID, fromPinID, fromIndex, toNodeID, toPinID, toIndex);
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