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
    void GraphRegistry::registerNode(NodeTypeInfo info, std::function<std::unique_ptr<Nodes::LogicNode>()> creator) {
        if (info.iconPath.empty()) {
            static const std::unordered_map<std::string, std::string> icons = {
                {"ConversationalAgent", "resources/icons/openai.png"},
                {"ToolsAgent", "resources/icons/gemini-512.png"},
                {"PlanExecuteAgent", "resources/icons/deepseek.png"},
                {"TextNode", "resources/icons/gemma.png"},
                {"MergeNode", "resources/icons/anthropic.png"},
                {"PostgreSQL", "resources/icons/postgres.png"},
                {"JiraSoftware", "resources/icons/google-gemini.png"}
            };
            if (icons.contains(info.typeName)) info.iconPath = icons.at(info.typeName);
        }
        static const std::unordered_map<std::string, NodeTypeInfo::ShapeHint> shapes = {
            {"TextNode", NodeTypeInfo::ShapeHint::D},
            {"ModelNode", NodeTypeInfo::ShapeHint::Circle},
            {"ConversationalAgent", NodeTypeInfo::ShapeHint::Rectangle},
            {"ToolsAgent", NodeTypeInfo::ShapeHint::Rectangle},
            {"PlanExecuteAgent", NodeTypeInfo::ShapeHint::Rectangle},
            {"MergeNode", NodeTypeInfo::ShapeHint::Rectangle},
            {"PostgreSQL", NodeTypeInfo::ShapeHint::Rectangle},
            {"JiraSoftware", NodeTypeInfo::ShapeHint::Rectangle}
        };
        if (info.shape == NodeTypeInfo::ShapeHint::Auto && shapes.contains(info.typeName)) {
            info.shape = shapes.at(info.typeName);
        }
        const std::string registeredTypeName = info.typeName;
        if(!storage.contains(info.typeName)) storage[info.typeName] = {std::move(info), std::move(creator)};

        LoggingManager::Info("[REGISTRED]: Type name is register. Type Name: ", registeredTypeName);
    }

    // Создаёт и возвращает новый объект
    std::unique_ptr<Nodes::LogicNode> GraphRegistry::createNode(const std::string& typeName) {
        if(storage.contains(typeName)) return storage[typeName].creator();

        LoggingManager::Fatal("[FAILED]: Node is not created");
        throw std::runtime_error("[FAILED]: Node is not created");
    }

    // Возвращает все зарезервированные имена
    std::vector<std::string> GraphRegistry::getRegisteredNodeTypes() {
        std::vector<std::string> registeredNodeTypes;
        for(const auto& [typeName, node] : storage) registeredNodeTypes.push_back(typeName);
        std::sort(registeredNodeTypes.begin(), registeredNodeTypes.end());
        return registeredNodeTypes;
    }

    std::vector<GraphRegistry::NodeTypeInfo> GraphRegistry::getRegisteredNodeInfos() const {
        std::vector<NodeTypeInfo> result;
        for (const auto& [typeName, node] : storage) result.push_back(node.info);
        std::sort(result.begin(), result.end(), [](const auto& left, const auto& right) {
            return left.displayName == right.displayName ? left.typeName < right.typeName : left.displayName < right.displayName;
        });
        return result;
    }

    std::map<std::string, std::vector<GraphRegistry::NodeTypeInfo>> GraphRegistry::getTypesByCategory() const {
        std::map<std::string, std::vector<NodeTypeInfo>> result;
        for (const auto& info : getRegisteredNodeInfos()) result[info.category].push_back(info);
        return result;
    }

    const GraphRegistry::NodeTypeInfo* GraphRegistry::findNodeTypeInfo(const std::string& typeName) const {
        auto it = storage.find(typeName);
        return it == storage.end() ? nullptr : &it->second.info;
    }

}