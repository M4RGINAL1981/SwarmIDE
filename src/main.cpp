/* #include "core/AppController.hpp"

int main(){
    setlocale(LC_ALL, "ru");

    SwarmIDE::Core::AppController app;
    app.run();
    return 0;
}
*/
#ifdef _WIN32
#include <windows.h> // Нужно для переключения кодировки консоли на Windows
#endif

#include "core/graph/GraphManager.hpp"
#include "core/graph/DataVariant.hpp"
#include "core/graph/GraphRegistry.hpp"
#include "core/ConnectionManager.hpp"
#include "core/ModelManager.hpp"
#include "core/graph/nodes/ModelNode.hpp"
#include "object/ai/ConversationalAgent.hpp"
#include <iostream>

using namespace SwarmIDE::Core;

int main() {
    // Настройка кодировки UTF-8 для Windows-консоли (убирает кракозябры)
#ifdef _WIN32
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
#endif
    setlocale(LC_ALL, "ru_RU.UTF-8");

    std::cout << "--- START BACKEND TEST ---" << std::endl;

    try {
        // Получаем менеджеры
        auto& registry = Graph::GraphRegistry::getInstance();                
        auto& graph = Graph::GraphManager::getInstance();                    
        auto& connections = ConnectionManager::getInstance();                
        auto& model_manager = ModelManager::getInstance();                   
        connections.SetGraphManager(&graph);

        // 1. Создаем агентов
        auto agent1 = registry.createNode("ConversationalAgent");
        auto agent2 = registry.createNode("ConversationalAgent");
        auto agent3 = registry.createNode("ConversationalAgent");

        auto model1 = registry.createNode("ModelNode");         
        auto model2 = registry.createNode("ModelNode");
        auto model3 = registry.createNode("ModelNode");

        if (!agent1 || !agent2 || !agent3 || !model1 || !model2 || !model3 ) {
            std::cerr << "Failed to create agents or models!" << std::endl;
            return -1;
        }

        agent1->SetName("Agent_Alpha");
        agent2->SetName("Agent_Beta");
        agent3->SetName("Agent_Gamma");

        model1->SetName("Model 1");
        model2->SetName("Model 2");
        model3->SetName("Model 3");

        auto mNode1 = dynamic_cast<Graph::ModelNode*>(model1.get());
        if (mNode1) mNode1->setTargetModelID("0");

        auto mNode2 = dynamic_cast<Graph::ModelNode*>(model2.get());
        if (mNode2) mNode2->setTargetModelID("0");

        auto mNode3 = dynamic_cast<Graph::ModelNode*>(model3.get());
        if (mNode3) mNode3->setTargetModelID("0");

        UUIDv4::UUID id1 = agent1->GetID();
        UUIDv4::UUID id2 = agent2->GetID();  
        UUIDv4::UUID id3 = agent3->GetID();
        UUIDv4::UUID id4 = model1->GetID();  
        UUIDv4::UUID id5 = model2->GetID();  
        UUIDv4::UUID id6 = model3->GetID();

        // 2. Добавляем в менеджер
        graph.addNode(std::move(agent1));
        graph.addNode(std::move(agent2));
        graph.addNode(std::move(agent3));
        graph.addNode(std::move(model1));
        graph.addNode(std::move(model2));
        graph.addNode(std::move(model3));

        // 3. Соединения (Исправили направление: от Выхода модели -> к Входу агента)
        connections.connect(id1, id3, "output_text", "input_text"); // Агент 1 -> Агент 3
        connections.connect(id2, id3, "output_text", "input_text"); // Агент 2 -> Агент 3
        connections.connect(id4, id1, "model_core", "model_core");  // Модель 1 -> Агент 1
        connections.connect(id5, id2, "model_core", "model_core");  // Модель 2 -> Агент 2
        connections.connect(id6, id3, "model_core", "model_core");  // Модель 3 -> Агент 3
        
        // 4. Имитируем ввод данных
        Graph::LogicNode* alpha = graph.findNodeByID(id1);
        if (!alpha) {
            throw std::runtime_error("Agent Alpha node not found in graph!");
        }
        alpha->setInputPinValue("input_text", SwarmIDE::Core::Graph::DataVariant(std::string("Hello from Human!")));
        
        // 5. Вызываем обход графа
        graph.StartExecutionAll();

    } 
    catch (const std::exception& e) {
        std::cerr << "\n[CRITICAL ERROR IN MAIN]: " << e.what() << std::endl;
    } 
    catch (...) {
        std::cerr << "\n[CRITICAL ERROR IN MAIN]: Unknown exception occurred!" << std::endl;
    }

    std::cout << "--- END BACKEND TEST ---" << std::endl;
    return 0;
}