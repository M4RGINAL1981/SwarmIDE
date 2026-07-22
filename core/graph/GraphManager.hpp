#pragma once
#include "SwarmIDE.hpp"
#include "core/engine/SerializationManager.hpp"
#include "nodes/base/LogicNode.hpp"
#include <iostream>
#include <vector>
#include <set>
#include <optional>

namespace SwarmIDE::Core {

    /**
     * @class GraphManager
     * @brief Главный управляющий менеджер вычислительного графа (Синглтон).
     * @details Является "сердцем" SwarmIDE. Отвечает за жизненный цикл всех узлов на сцене,
     * их поиск, валидацию связей, топологическую сортировку зависимостей и запуск 
     * параллельного или последовательного вычисления логики агентов.
     * 
     * @note Доступ к менеджеру осуществляется исключительно через метод getInstance().
     * @see SwarmIDE::Core::LogicNode
     * @see SwarmIDE::Core::AppController
     */
    class GraphManager {
    public:
        /**
         * @brief Глобальная точка доступа к единственному экземпляру класса GraphManager.
         * @return GraphManager& Ссылка на синглтон менеджера графа.
         */
        static GraphManager& getInstance();

        /**
         * @brief Вызов направленного обхода графа, начиная с конкретного узла.
         * @param[in] triggerNodeID Уникальный UUID узла, который инициализирует цепочку выполнения.
         * 
         * @warning ЛОГИКА ПОКА НЕ ЯСНА. Метод находится в стадии проектирования.
         * @todo Финализировать алгоритм частичного обхода подграфа.
         */
        void StartExecution(UUIDv4::UUID triggerNodeID);           

        /**
         * @brief Полный обход всего вычислительного графа.
         * @details Метод строит карту зависимостей всех узлов, сортирует их и отправляет
         * на выполнение в многопоточное вычислительное ядро.
         * 
         * @see ResolveDependencies()
         * @see ProcessQueueParallel()
         */
        void StartExecutionAll();                                  

        /**
         * @brief Зарегистрировать и добавить новый узел в граф.
         * @param[in] node Умный указатель (std::unique_ptr) на добавляемый объект, наследуемый от LogicNode.
         */
        void addNode(std::unique_ptr<Nodes::LogicNode> node);

        /**
         * @brief Удалить конкретный узел из графа по его уникальному идентификатору.
         * @param[in] nodeID UUID узла, который необходимо уничтожить.
         * @note Метод автоматически разрывает все логические связи (пины), привязанные к этой ноде.
         */
        void removeNode(UUIDv4::UUID nodeID);     

        /**
         * @brief Полностью очистить граф сцены.
         * @details Удаляет абсолютно все узлы и обнуляет хранилище. Используется при создании 
         * нового проекта или перед импортом другого графа.
         */                 
        void removeAllNodes();                                     

        /**
         * @brief Получить текущее количество узлов на сцене холста.
         * @return int Количество активных объектов в графе.
         */
        int getCountNodes() { return nodes.size(); }        
        
        /**
         * @brief Получить доступ к контейнеру со всеми узлами.
         * @return std::vector<std::unique_ptr<Nodes::LogicNode>>& Ссылка на внутренний массив объектов графа.
         */   
        std::vector<std::unique_ptr<Nodes::LogicNode>>& getNodes() { return nodes; }

        /**
         * @brief Поиск узла на сцене по его UUID.
         * @param[in] id Уникальный идентификатор искомого узла.
         * @return Nodes::LogicNode* Указатель на найденный узел или `nullptr`, если объект не существует.
         */
        Nodes::LogicNode* findNodeByID(UUIDv4::UUID id);                  

        // Запрещаем копирование и перемещение синглтона
        GraphManager(const GraphManager&) = delete;
        GraphManager& operator=(const GraphManager&) = delete;
        GraphManager(GraphManager&&) = delete;
        GraphManager& operator=(GraphManager&&) = delete;

    private:
        // Дружественные классы, имеющие доступ к приватным данным для рендеринга и системных операций
        friend class UI::Menu;                 /**< Доступ для отрисовки списков нод в меню. */
        friend class UI::ConnectionDrawer;     /**< Доступ для связи визуальных пинов с логическими. */
        friend class AppController;            /**< Доступ для глобального контроля жизненного цикла приложения. */
        friend class SerializationManager;     /**< Доступ для сохранения/загрузки структуры графа в JSON/файл. */

        /** @brief Закрытый конструктор синглтона. */
        GraphManager() {};
        ~GraphManager() = default;

        /**
         * @brief Рекурсивно собирает все родительские узлы (построение дерева зависимостей).
         * @details Используется для топологической сортировки графа перед запуском вычислений,
         * чтобы гарантировать, что дочерние узлы не начнут выполняться раньше, чем их родители отдадут данные.
         * 
         * @param[in] nodeID Текущий целевой узел.
         * @param[out] executionOrder Очередь UUID в порядке их будущего выполнения.
         * @param[in,out] visited Контейнер уже посещенных узлов для предотвращения зацикливания графа.
         */
        void ResolveDependencies(UUIDv4::UUID nodeID, std::vector<UUIDv4::UUID>& executionOrder, std::vector<UUIDv4::UUID>& visited);
        
        /**
         * @brief Асинхронный многопоточный обход очереди и запуск вычислений в узлах.
         * @details Распределяет готовые к выполнению узлы (у которых заполнены входные пины)
         * по пулу потоков для достижения максимальной производительности мультиагентной системы.
         * 
         * @param[in] allNodes Отсортированный вектор UUID узлов, готовых к обработке вычислительным ядром.
         * @note Внутри метода активно отслеживается статус SwarmIDE::Nodes::State::Executing.
         */
        void ProcessQueueParallel(const std::vector<UUIDv4::UUID>& allNodes);                   

        std::vector<std::unique_ptr<Nodes::LogicNode>> nodes; /**< Внутреннее изолированное хранилище всех узлов текущей сцены. */
    };

}

