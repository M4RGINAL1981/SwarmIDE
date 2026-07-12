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
    }

    void GraphManager::ProcessQueue() {
        while(executionQueue.size() != 0){
            LogicNode* node = findNodeByID(executionQueue.front());

            node->Execute();

            /*
            После успешного завершения ноды движок смотрит в `ConnectionManager`: _«К каким входам подключены выходы этой завершенной ноды?»_.
            
            Для каждой найденной связи менеджер **физически копирует** `DataVariant` из выходного порта отработавшей ноды во входной порт целевой ноды.
            
            Проверяет целевую ноду: если все её обязательные входы получили данные, её ID добавляется в `executionQueue`.
            */

            executionQueue.pop();
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

