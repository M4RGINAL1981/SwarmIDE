#pragma once 
#include <string>
#include <vector>
#include <memory>         
#include <functional>     
#include <unordered_map>  

namespace SwarmIDE::Core::Graph {

    class LogicNode;

    class GraphRegistry {   // Фабрика объектов
    public:
        static GraphRegistry& getInstance();

        void registerNode(const std::string& typeName, std::function<std::unique_ptr<LogicNode>()> creator);
        std::unique_ptr<LogicNode> createNode(const std::string& typeName);
        std::vector<std::string> getRegisteredNodeTypes();

        GraphRegistry(const GraphRegistry&) = delete;
        GraphRegistry& operator=(const GraphRegistry&) = delete;
        GraphRegistry(GraphRegistry&&) = delete;
        GraphRegistry& operator=(GraphRegistry&&) = delete;
        
    private:
        GraphRegistry() = default;
        ~GraphRegistry() = default;

        std::unordered_map<std::string, std::function<std::unique_ptr<LogicNode>()>> storage;
    };

    #define REGISTER_NODE(NodeType, TypeNameString) \
    static bool NodeType##_registered = []() { \
        SwarmIDE::Core::Graph::GraphRegistry::getInstance().registerNode(TypeNameString, []() -> std::unique_ptr<SwarmIDE::Core::Graph::LogicNode> { \
            return std::make_unique<NodeType>(TypeNameString); \
        }); \
        return true; \
    }();
}