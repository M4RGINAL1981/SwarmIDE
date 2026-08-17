#include "SwarmIDE.hpp"
#include "core/engine/SerializationManager.hpp"
#include "nodes/base/LogicNode.hpp"
#include "nodes/core/TextNode.hpp"
#include "nodes/core/ModelNode.hpp"
#include "ui/command/CommandBus.hpp"

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
        LoggingManager::Info("Saving graph to: " + filepath);

        json root;
        root["nodes"] = json::array();       /** @brief Массив узлов. Каждая запись связывает логический узел с его визуальным представлением. */
        root["connections"] = json::array(); /** @brief Массив связей. Каждая запись содержит логическое соединение и его визуальное отображение. */

        auto& viewNodes = UI::CanvasManager::getInstance().getViewNodes();

        // Первый проход при сохранении: собираем данные всех нод
        for(auto const& [id, viewNode] : viewNodes) root["nodes"].push_back(viewNode->serializeViewNode());

        // Второй проход при сохранении: собираем все существующие связи
        auto& logicConnections = ConnectionManager::getInstance().getAllConnections();
        for(auto const& [id, logicConnection] : logicConnections) root["connections"].push_back(logicConnection.serializeLogicConnection());

        std::ofstream file(filepath);
        if (!file.is_open()) { LoggingManager::Error("Save Error", std::strerror(errno), " (Path: " + filepath + ")"); return;}
        file << root.dump(4);
        file.close();

        LoggingManager::Info("Graph successfully saved.");
    }

    void SerializationManager::LoadGraph(const std::string& filepath) {
        LoggingManager::Info("Loading graph from: " + filepath);

        std::ifstream file(filepath);
        if (!file.is_open()) { LoggingManager::Error("Load Error", std::strerror(errno), " (Path: " + filepath + ")"); return; }

        json root;
        try { file >> root;} 
        catch (const std::exception& e) { LoggingManager::Error("Parse Error", e.what(), ""); return;}

        // Очистка старой сцены перед загрузкой новой
        ClearGraph();

        // Первый проход: Загрузка нод. 
        // Мы сначала создаем все объекты, чтобы их ID стали доступны в системе.
        if(root.contains("nodes") && root["nodes"].is_array()) {
            for (const auto& nodeJson : root["nodes"]) {
                if (nodeJson.contains("type")) {
                    UUIDv4::UUID nodeID = UUIDv4::UUID(nodeJson["id"].get<std::string>());
                    glm::vec2 spawnPos = glm::vec2(nodeJson["worldPosX"], nodeJson["worldPosY"]);

                    std::unique_ptr<Nodes::LogicNode> newNode = GraphRegistry::getInstance().createNode(nodeJson["type"]);
                    newNode->deserializeNode(nodeJson);

                    GraphManager::getInstance().addNode(std::move(newNode));                        /** @note Создаём логическую ноду. */
                    UI::CanvasManager::getInstance().AddViewNode(nodeID, spawnPos);                 /** @note Создаём визуальную ноду. */
                }
            }
        }

        // Второй проход: Установка связей.
        // Теперь, когда все ноды гарантированно существуют в памяти, мы соединяем их по ID.
        if(root.contains("connections") && root["connections"].is_array()) {
            for (const auto& connJson : root["connections"]) {
                try {
                    LogicConnection logicConnection;
                    logicConnection.deserializeLogicConnection(connJson);
                    ConnectionManager::getInstance().connect(logicConnection);                      /** @note Создаём логическое соединение. */
                    UI::CanvasManager::getInstance().AddViewConnection(logicConnection.getID());    /** @note Создаём визуальное соединение. */
                } catch (...) {
                    LoggingManager::Warning("Connection skip", "Failed to resolve IDs for a connection during load.", "");
                }
            }
        }

        UI::CanvasManager::getInstance().SyncWithCoreGraph();                                       /** @note Производим синхронизацию. */
        LoggingManager::Info("Graph successfully loaded.");
    }

    void SerializationManager::ClearGraph() {
        LoggingManager::Info("Clearing current graph...");

        /** @note Удаление комманд */
        UI::Command::CommandBus::getInstance().Clear();

        /** @note Удаление визуальных и логических соединений. */
        UI::CanvasManager::getInstance().RemoveAllViewConnection();
        ConnectionManager::getInstance().removeAllConnections();

        /** @note Удаление визуальных и логических нод. */
        UI::CanvasManager::getInstance().removeAllViewNodes();
        GraphManager::getInstance().removeAllNodes();

        LoggingManager::Info("Graph cleared.");
    }
}
