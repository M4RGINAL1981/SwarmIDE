#pragma once
#include "core/graph/nodes/LogicNode.hpp"
#include "ui/Topology.hpp"
#include "core/SerializationManager.hpp"
#include <iostream>
#include <vector>
#include <set>
#include <optional>

namespace SwarmIDE::Core::Graph {

    class GraphManager {
    public:
        // Глобальная точка доступа к единственному экземпляру
        static GraphManager& getInstance();

        void StartExecution(UUIDv4::UUID triggerNodeID);           // Вызов обхода графа с конкретной ноды (ЛОГИКА ПОКА НЕ ЯСНА)
        void StartExecutionAll();                                  // Полный обход графа

        void addNode(std::unique_ptr<LogicNode> node);
        void removeNode(UUIDv4::UUID nodeID);                      // Удаление нод по id
        void removeAllNodes();                                     // Удаление вообще всех нод

        int getCountObject() { return nodes.size(); }              // Вернуть кол-во нод
        std::vector<std::unique_ptr<LogicNode>>& getNodes() { return nodes; }

        LogicNode* findNodeByID(UUIDv4::UUID id);                  // Поиск нод по id

        GraphManager(const GraphManager&) = delete;
        GraphManager& operator=(const GraphManager&) = delete;
        GraphManager(GraphManager&&) = delete;
        GraphManager& operator=(GraphManager&&) = delete;

    private:
        friend SwarmIDE::UI::Menu;    
        friend SwarmIDE::UI::ConnectionDrawer;
        friend SwarmIDE::Core::AppController;
        friend SwarmIDE::Core::SerializationManager;

        GraphManager() {};
        ~GraphManager() = default;

        // Рекурсивно собирает все родительские ноды
        void ResolveDependencies(UUIDv4::UUID nodeID, std::vector<UUIDv4::UUID>& executionOrder, std::vector<UUIDv4::UUID>& visited);
        // Обход графа и запуск всех нод (Если можно - запуск по потокам)
        void ProcessQueueParallel(const std::vector<UUIDv4::UUID>& allNodes);                   

        std::vector<std::unique_ptr<LogicNode>> nodes; // Хранилище всех нод сцены
    };

}

