#include "SwarmIDE.hpp"
#include "nodes/ai/BaseAgent.hpp"

namespace SwarmIDE::Nodes {

    std::vector<Core::PropertyDescriptor> BaseAgent::properties() { 
        return { Prop("systemInstruction", "System prompt", Core::PropertyKind::MultilineText, systemInstruction) };
    }

    json BaseAgent::serializeNode() {
        json nodeJson = LogicNode::serializeNode();
        nodeJson["SystemInstruction"] = getSystemInstruction();
        
        return nodeJson;
    }

    void BaseAgent::deserializeNode(const json& nodeJson) {
        LogicNode::deserializeNode(nodeJson);
        setSystemInstruction(nodeJson["SystemInstruction"]);
    }
}