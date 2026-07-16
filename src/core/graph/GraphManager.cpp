#include "core/graph/GraphManager.hpp"
#include <iostream>
#include "core/ConnectionManager.hpp"
#include <thread>

namespace SwarmIDE::Core::Graph {

    GraphManager& GraphManager::getInstance() {
        static GraphManager instance;
        return instance;
    }

    void GraphManager::StartExecution(UUIDv4::UUID triggerNodeID) {
        /* С многопоточностью (Пока рано)
        // Очишаем очередь
        while(executionQueue.size() != 0) {
            LogicNode* node = findNodeByID(executionQueue.front());
            node->SetState(Idle);
            executionQueue.pop();
        }

        // Помещаем стартовую ноду в начало очереди
        executionQueue.push(triggerNodeID);

        // Запускаем цикл обработки очереди
        std::thread processQueue(&GraphManager::ProcessQueue, this);
        processQueue.detach();
        */

        // Очищаем очередь выполнения
        std::queue<UUIDv4::UUID> empty;
        std::swap(executionQueue, empty);

        std::vector<UUIDv4::UUID> executionOrder;
        std::vector<UUIDv4::UUID> visited;

        std::cout << "[GraphManager]: Выстраиваем цепочку зависимостей для ноды " << triggerNodeID << "..." << std::endl;
        
        // Строим правильный порядок выполнения
        ResolveDependencies(triggerNodeID, executionOrder, visited);

        std::cout << "[GraphManager]: Порядок выполнения собран! Всего нод к запуску: " << executionOrder.size() << std::endl;

        // Переносим упорядоченные ноды в очередь выполнения
        for (const auto& nodeID : executionOrder) {
            executionQueue.push(nodeID);
        }
        
        // Запускаем обработку
        ProcessQueue();
    }

    void GraphManager::StartExecutionAll() {
        // 1. Очищаем очередь
        std::queue<UUIDv4::UUID> empty;
        std::swap(executionQueue, empty);

        std::vector<UUIDv4::UUID> executionOrder;
        std::vector<UUIDv4::UUID> visited;

        std::cout << "[GraphManager]: Запуск всего графа (полная топологическая сортировка)..." << std::endl;

        // 2. Вместо вызова для одной ноды, проходим по ВСЕМ нодам в векторе
        for (const auto& node : nodes) {
            ResolveDependencies(node->GetID(), executionOrder, visited);
        }

        std::cout << "[GraphManager]: Порядок выполнения собран! Всего нод: " << executionOrder.size() << std::endl;

        // 3. Заполняем очередь
        for (const auto& nodeID : executionOrder) {
            executionQueue.push(nodeID);
        }
        
        // 4. Запускаем
        ProcessQueue();
    }

    void GraphManager::ProcessQueue() {
        while(!executionQueue.empty()) {
            UUIDv4::UUID currentID = executionQueue.front();
            executionQueue.pop(); 

            LogicNode* node = findNodeByID(currentID);
            if(!node) continue;

            // --- ВАЖНАЯ ПРОВЕРКА ЗАВИСИМОСТЕЙ ---
            // Если хотя бы одна родительская нода, от которой мы зависим, завершилась с ошибкой,
            // то текущую ноду мы выполнять НЕ имеем права.
            bool parentFailed = false;
            auto incoming = ConnectionManager::getInstance().getIncomingConnectionsForNode(currentID);
            for (auto* conn : incoming) {
                if (conn != nullptr) {
                    LogicNode* parentNode = findNodeByID(conn->fromNodeID);
                    if (parentNode && parentNode->GetState() == State::Failed) {
                        parentFailed = true;
                        break;
                    }
                }
            }

            if (parentFailed) {
                std::cout << "  [GraphManager]: Пропуск выполнения ноды " << node->GetName() 
                        << ", так как одна из родительских нод завершилась с ошибкой!" << std::endl;
                node->SetState(State::Failed); // Каскадно помечаем себя как Failed для наших потомков
                continue; 
            }
            // ------------------------------------

            // Выполняем текущую ноду (теперь её входы гарантированно корректны!)
            std::cout << "\n=== [RUNNING NODE]: " << node->GetName() << " (ID: " << node->GetID() << ") ===" << std::endl;
            node->Execute();

            if (node->GetState() == State::Failed) {
                std::cout << "  [GraphManager]: Нода " << node->GetName() << " завершилась с ошибкой. Propagation остановлен." << std::endl;
                continue; 
            }

            // Передаем результаты её выполнения дальше по связям
            std::vector<Connection*> connections = ConnectionManager::getInstance().getOutgoingConnectionsForNode(node->GetID());
            for(auto* conn : connections) {
                if(conn != nullptr) {
                    LogicNode* secondNode = findNodeByID(conn->toNodeID);
                    if(secondNode != nullptr) {
                        std::cout << "  -> Передача данных: [" << node->GetName() << " -> " << secondNode->GetName() << "]" << std::endl;
                        secondNode->setInputPinValue(conn->toPinID, node->getOutputPinValue(conn->fromPinID));
                    }
                }
            }
        }
    }


    void GraphManager::addNode(std::unique_ptr<LogicNode> node) { 
        nodes.push_back(std::move(node));
        std::cout << "Node is add" << std::endl;
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

}

