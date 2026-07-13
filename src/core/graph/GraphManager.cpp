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

        // Очищаем очередь (лучше делать это через swap, у queue нет метода clear)
        std::queue<UUIDv4::UUID> empty;
        std::swap(executionQueue, empty);

        executionQueue.push(triggerNodeID);
        
        // Пока запускаем синхронно, прямо здесь
        ProcessQueue();
    }

    void GraphManager::ProcessQueue() {
        while(!executionQueue.empty()) {
            // 1. Извлекаем ID из очереди правильно
            UUIDv4::UUID currentID = executionQueue.front();
            executionQueue.pop(); 

            LogicNode* node = findNodeByID(currentID);
            if(!node) continue;

            // 2. Выполняем логику
            node->Execute();
            std::cout << "NODE ID: " << node->GetID() << std::endl;

            // 3. Передаем результаты дальше
            std::vector<Connection*> connections = ConnectionManager::getInstance().getOutgoingConnectionsForNode(node->GetID());

            for(auto* conn : connections) {
                if(conn != nullptr) {
                    LogicNode* secondNode = findNodeByID(conn->toNodeID);
                    if(secondNode != nullptr) {
                        std::cout << "Second Node ID: " << secondNode->GetID() << std::endl;
                        secondNode->setInputPinValue(conn->toPinID, node->getOutputPinValue(conn->fromPinID));

                        if(secondNode->isFullInputPin()) { 
                            executionQueue.push(secondNode->GetID()); 
                        }
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

}

