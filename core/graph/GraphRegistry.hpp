#pragma once 
#include "SwarmIDE.hpp"
#include "nodes/base/LogicNode.hpp"
#include "core/engine/LoggingManager.hpp"
#include <string>
#include <vector>
#include <memory>         
#include <functional>     
#include <unordered_map>  

namespace SwarmIDE::Core {

    class LogicNode;

    /**
     * @class GraphRegistry
     * @brief Фабрика объектов и центральный регистратор типов узлов (Синглтон).
     * @details Реализует паттерн "Фабрика" (Factory Pattern). Хранит в себе колбэки (лямбды)
     * для создания всех существующих типов нод в SwarmIDE. Позволяет графической оболочке
     * или менеджеру сериализации динамически создавать узлы по их строгому имени типа.
     * 
     * @note Для автоматической регистрации новых нод используйте макрос REGISTER_NODE.
     * @see REGISTER_NODE
     * @see SwarmIDE::Core::GraphManager
     */
    class GraphRegistry {   // Фабрика объектов
    public:
        /**
         * @brief Глобальная точка доступа к единственному экземпляру фабрики GraphRegistry.
         * @return GraphRegistry& Ссылка на синглтон регистратора.
         */
        static GraphRegistry& getInstance();

        /**
         * @brief Регистрирует новый тип узла в системе, связывая его с функцией-создателем.
         * 
         * @param[in] typeName Уникальный строковый идентификатор типа (например, "JiraSoftware").
         * @param[in] creator Функция-лямбда, возвращающая `std::unique_ptr<Nodes::LogicNode>` для этого типа.
         * 
         * @note Обычно вызывается автоматически на этапе инициализации приложения через макрос авторегистрации.
         */
        void registerNode(const std::string& typeName, std::function<std::unique_ptr<Nodes::LogicNode>()> creator);

        /**
         * @brief Динамически создает и возвращает новый экземпляр узла по его типу.
         * 
         * @param[in] typeName Строковое имя типа искомого узла.
         * @return std::unique_ptr<Nodes::LogicNode> Умный указатель на созданный узел.
         * 
         * @throws std::runtime_error, если передан незарегистрированный тип.
         * @see registerNode()
         */
        std::unique_ptr<Nodes::LogicNode> createNode(const std::string& typeName);

        /**
         * @brief Возвращает список всех зарегистрированных типов узлов в системе.
         * @details Используется UI-слоем (`UI::Menu`) для динамического построения контекстного 
         * меню добавления нод на холст сцены.
         * 
         * @return std::vector<std::string> Список доступных строковых идентификаторов.
         */
        std::vector<std::string> getRegisteredNodeTypes();

        // Запрещаем дублирование синглтона фабрики
        GraphRegistry(const GraphRegistry&) = delete;
        GraphRegistry& operator=(const GraphRegistry&) = delete;
        GraphRegistry(GraphRegistry&&) = delete;
        GraphRegistry& operator=(GraphRegistry&&) = delete;
        
    private:
        GraphRegistry() { Core::LoggingManager::Info("[GRAPH_REGISTRY]: Constructor"); }
        ~GraphRegistry() { Core::LoggingManager::Info("[GRAPH_REGISTRY]: Destructor");}

        /** @brief Внутренняя хэш-таблица, связывающая имя типа ноды с её фабричной функцией. */
        std::unordered_map<std::string, std::function<std::unique_ptr<Nodes::LogicNode>()>> storage;
    };

    /**
     * @def REGISTER_NODE
     * @brief Макрос для ленивой автоматической регистрации класса узла в фабрике.
     * @details Помещается в `.cpp` файл созданной ноды. Создает скрытую статическую глобальную
     * переменную-флаг, которая инициализируется до вызова функции `main()`, автоматически 
     * внедряя лямбду создания узла в синглтон GraphRegistry.
     * 
     * **Пример использования:**
     * @code
     * // Внутри файла ConversationalAgent.cpp:
     * REGISTER_NODE(ConversationalAgent, "ConversationalAgent")
     * @endcode
     * 
     * @param NodeType Настоящий C++ тип (класс) вашей ноды.
     * @param TypeNameString Строковое имя для регистрации на холсте и в JSON-конфигах.
     */
    #define REGISTER_NODE(NodeType, TypeNameString) \
    static bool NodeType##_registered = []() { \
        SwarmIDE::Core::GraphRegistry::getInstance().registerNode(TypeNameString, []() -> std::unique_ptr<SwarmIDE::Nodes::LogicNode> { \
            return std::make_unique<NodeType>(TypeNameString); \
        }); \
        return true; \
    }();
}