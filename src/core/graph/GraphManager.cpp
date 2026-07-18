#include "core/graph/GraphManager.hpp"
#include "core/LoggingManager.hpp"
#include <iostream>
#include "core/ConnectionManager.hpp"
#include <thread>
#include <future>

namespace SwarmIDE::Core::Graph {

    GraphManager& GraphManager::getInstance() {
        static GraphManager instance;
        return instance;
    }

    void GraphManager::StartExecution(UUIDv4::UUID triggerNodeID) {
        std::vector<UUIDv4::UUID> executionOrder;
        std::vector<UUIDv4::UUID> visited;
        
        ResolveDependencies(triggerNodeID, executionOrder, visited);
        ProcessQueueParallel(executionOrder);
    }

    void GraphManager::StartExecutionAll() {
        std::vector<UUIDv4::UUID> allNodes;
        for(const auto& node : nodes) allNodes.push_back(node->GetID());
        ProcessQueueParallel(allNodes);
    }

    void GraphManager::addNode(std::unique_ptr<LogicNode> node) { 
        std::cout << "[ADDING A NODE]: Node ID: " << node->GetID() << " type Node: " << node->getTypeName() << std::endl;
        nodes.push_back(std::move(node));
    }

    void GraphManager::removeNode(UUIDv4::UUID nodeID) {
        SwarmIDE::Core::ConnectionManager::getInstance().removeConnectionsForNode(nodeID);

        nodes.erase(
            std::remove_if(nodes.begin(), nodes.end(),
                [nodeID](const std::unique_ptr<LogicNode>& nd) {
                    return nd->GetID() == nodeID;
                }), 
            nodes.end()
        );

        std::cout << "Node is delete" << std::endl;
    }

    void GraphManager::removeAllNodes() {
        ConnectionManager::getInstance().removeAllConnections();
        nodes.clear();
    }

    LogicNode* GraphManager::findNodeByID(UUIDv4::UUID id) {
        for(auto& node : nodes){
            if(id == node->GetID()) return node.get();
        }
        return nullptr;
    }

    void GraphManager::ResolveDependencies(UUIDv4::UUID nodeID, std::vector<UUIDv4::UUID>& executionOrder, std::vector<UUIDv4::UUID>& visited) {
    /*
        Рекурсивный поиск всех предков. Обратный обход графа

        nodeID - текущая вершина, для которой обходят всех предков
        executionOrder - 
        visited - 
    */
        if(std::find(visited.begin(), visited.end(), nodeID) != visited.end()) return;
        visited.push_back(nodeID);

        // Получаем все входящие связи для текущей ноды
        auto incoming = ConnectionManager::getInstance().getIncomingConnectionsForNode(nodeID);

        for(auto* conn : incoming) {
            if(conn != nullptr) ResolveDependencies(conn->fromNodeID, executionOrder, visited);
        }

        if(std::find(executionOrder.begin(), executionOrder.end(), nodeID) == executionOrder.end()) {
            executionOrder.push_back(nodeID);
        }
    }

    void GraphManager::ProcessQueueParallel(const std::vector<UUIDv4::UUID>& allNodesID){
        // 1. Иницилизация
        std::set<UUIDv4::UUID> remaining;                           // Все ноды, что ещё не отработали
        std::set<UUIDv4::UUID> completedNodes;                      // Все ноды, что завершились с успехом
        std::set<UUIDv4::UUID> failedNodes;                         // Все ноды, что завершились с ошибкой

        for(auto& nodeID : allNodesID) remaining.insert(nodeID);    

        for(auto& nodeID : allNodesID) {                            // Устанавливаем состояние всех нод в Waiting (Ожидание)
            auto node = findNodeByID(nodeID);
            node->SetState(Waiting);
        }

        // 2. Цикл (пока remaining НЕ пусто)

        while(remaining.size() != 0) {
            std::vector<UUIDv4::UUID> readyNodes;                   // Все ноды, чьи родители отработали успешно

            for(auto& nodeID : remaining) {                                                                 // Для каждой ноды проверяем все ли родители отработали
                bool flag = true;                                                                           // Флаг-проверка, что все родительские ноды отработали
                auto connections = ConnectionManager::getInstance().getIncomingConnectionsForNode(nodeID);  // Получаем все входяшие связи для данной ноды

                LoggingManager::Print("\n[READINESS CHECK]: type Node: ", findNodeByID(nodeID)->getTypeName(), " Node ID: ", nodeID, "\n"); 
                LoggingManager::Print("[COUNT CONNECTIONS]:", connections.size(), "\n"); 

                for(auto connect : connections) {
                    auto it = std::find(completedNodes.begin(), completedNodes.end(), connect->fromNodeID); // Проверяем что нода отработала
                    if(it == completedNodes.end()) { 
                        flag = false;
                        break;
                    }

                    auto jt = std::find(failedNodes.begin(), failedNodes.end(), connect->fromNodeID);       // Проверяем что нода не находиться в ошибках
                    if(jt != failedNodes.end()) {
                        flag = false;
                        // Здесь в будущем можно сразу добавлять текущую ноду в failedNodes (каскадный сбой)
                        break;
                    }

                    // Передача данных между нодами

                    auto node1 = findNodeByID(connect->toNodeID);    // Узел, во входной пин которого передается значение
                    auto node2 = findNodeByID(connect->fromNodeID);  // Узел, из выходного пина которого берется значение

                    node1->setInputPinValue(connect->toPinID, node2->getOutputPinValue(connect->fromPinID));
                }
                
                if(flag)  {                                                                                 // Если все родительские ноды, отработали И родитель не в failedNodes
                    readyNodes.push_back(nodeID);                                           
                    LoggingManager::Print("[QUEUE RESULT]: This node is ready. Connection: ", connections.size(), "\n"); 
                }
                else {
                    LoggingManager::Print("[QUEUE RESULT]: This node is not ready. Connection: ", connections.size(), "\n"); 
                }
            }

            // Проверка на дедлок
            if(readyNodes.size() == 0 && remaining.size() != 0) throw std::runtime_error("[ERROR]: Cyclic dependency");

            std::vector<std::future<void>> futures;                       // Талончики для каждой ноды, что должна отработать

            for(auto& nodeID : readyNodes) {
                auto node = findNodeByID(nodeID);
                if(node != nullptr) {
                    // Запустить поток и сохранить future
                    futures.push_back(
                        std::async(std::launch::async, [node]() {
                            node->Execute();
                        })
                    );
                }   
            }

            for(auto& future : futures) future.get();                                // Ждём завершения всех нод
            for(auto& nodeID : readyNodes) {                                         // Удаляем ID уже завершенных свою работу нод
                remaining.erase(nodeID);      
                auto node = findNodeByID(nodeID);
                if(node->GetState() == Success) completedNodes.insert(nodeID);       // Если нода завершилась успешно - добавляем её в множество нод, отработавших успешно
                else if (node->GetState() == Failed) failedNodes.insert(nodeID);     // Если нода завершилась ошибкой - добавляем её в множество нод, отработавших с ошибкой
            }
            futures.clear();                                                         // Очишаем вектор талоничков
        }
    }
}

