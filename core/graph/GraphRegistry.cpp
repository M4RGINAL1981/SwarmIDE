#include "SwarmIDE.hpp"
#include "core/graph/GraphRegistry.hpp"
#include "nodes/base/LogicNode.hpp"
#include "core/engine/LoggingManager.hpp"

namespace SwarmIDE::Core {

    GraphRegistry& GraphRegistry::getInstance() {
        static GraphRegistry instance;
        return instance;
    }

    
    // Создаёт новый "раздел" в storage
    void GraphRegistry::registerNode(const std::string& typeName, std::function<std::unique_ptr<Nodes::LogicNode>()> creator) {
        if(!storage.contains(typeName)) storage[typeName] = creator;
        
        LoggingManager::Info("[REGISTRED]: Type name is register. Type Name: ", typeName);
    }

    // Создаёт и возвращает новый объект
    std::unique_ptr<Nodes::LogicNode> GraphRegistry::createNode(const std::string& typeName) {
        if(storage.contains(typeName)) return storage[typeName]();

        LoggingManager::Fatal("[FAILED]: Node is not created");
        throw std::runtime_error("[FAILED]: Node is not created");
    }

    // Возвращает все зарезервированные имена
    std::vector<std::string> GraphRegistry::getRegisteredNodeTypes() {
        std::vector<std::string> registeredNodeTypes;
        for(const auto& [typeName, creator] : storage) registeredNodeTypes.push_back(typeName);
        
        return registeredNodeTypes;
    }

}