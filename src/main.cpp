/* #include "core/AppController.hpp"

int main(){
    setlocale(LC_ALL, "ru");

    SwarmIDE::Core::AppController app;
    app.run();
    return 0;
}
*/

#include "core/graph/GraphManager.hpp"
#include "core/graph/DataVariant.hpp"
#include "core/graph/GraphRegistry.hpp"
#include "core/ConnectionManager.hpp"
#include "object/ai/ConversationalAgent.hpp"
#include <iostream>

using namespace SwarmIDE::Core;

int main() {
    setlocale(LC_ALL, "ru");
    std::cout << "--- START BACKEND TEST ---" << std::endl;

    // Получаем менеджеры
    auto& registry = Graph::GraphRegistry::getInstance();
    auto& graph = Graph::GraphManager::getInstance();
    auto& connections = ConnectionManager::getInstance();
    connections.SetGraphManager(&graph);

    // 1. Создаем агентов
    auto agent1 = registry.createNode("ConversationalAgent");
    auto agent2 = registry.createNode("ConversationalAgent");
    if (!agent1 || !agent2) {
        std::cerr << "Failed to create agents!" << std::endl;
        return -1;
    }

    agent1->SetName("Agent_Alpha");
    agent2->SetName("Agent_Beta");

    UUIDv4::UUID id1 = agent1->GetID();
    UUIDv4::UUID id2 = agent2->GetID();

    // 2. Добавляем в менеджер
    graph.addNode(std::move(agent1));
    graph.addNode(std::move(agent2));

    // 3. Соединяем
    connections.connect(id1, id2, "output_text", "input_text");
    
    // 4. Имитируем ввод данных в первого агента извне (например, пользователь ввел текст)
    Graph::LogicNode* alpha = graph.findNodeByID(id1);
    alpha->getInputPinValue("input_text") = SwarmIDE::Core::Graph::DataVariant(std::string("Hello from Human!")); // Записываем данные
    
    // 5. Вызываем логику первой ноды вручную для теста
    alpha->Execute();

    // (Здесь в будущем GraphManager должен сам перенести данные по связи и вызвать Execute у второго)

    std::cout << "--- END BACKEND TEST ---" << std::endl;
    return 0;
}