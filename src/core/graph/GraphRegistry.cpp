#include "core/graph/GraphRegistry.hpp"
#include "core/graph/LogicNode.hpp"

namespace SwarmIDE::Core::Graph {

    GraphRegistry& GraphRegistry::getInstance() {
        static GraphRegistry instance;
        return instance;
    }

    
        // Создаёт новый "раздел" в storage
        void GraphRegistry::registerNode(const std::string& typeName, std::function<std::unique_ptr<LogicNode>()> creator) {
            if(!storage.contains(typeName)) storage[typeName] = creator;
        }

        // Создаёт и возвращает новый объект
        std::unique_ptr<LogicNode> GraphRegistry::createNode(const std::string& typeName) {
            if(storage.contains(typeName)) return storage[typeName]();
            return nullptr;
        }

        // Возвращает все зарезервированные имена
        std::vector<std::string> GraphRegistry::getRegisteredNodeTypes() {
            std::vector<std::string> registeredNodeTypes;
            for(const auto& [typeName, creator] : storage) {
                registeredNodeTypes.push_back(typeName);
            }
            return registeredNodeTypes;
        }

}