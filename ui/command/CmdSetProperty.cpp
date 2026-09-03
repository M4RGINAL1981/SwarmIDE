#include "SwarmIDE.hpp"
#include "ui/command/CmdSetProperty.hpp"
#include "core/graph/GraphManager.hpp"

namespace SwarmIDE::UI::Command {

    CmdSetProperty::CmdSetProperty(UUIDv4::UUID nodeID, std::string key, Core::DataVariant newValue) {
        this->nodeID = nodeID;
        this->key = key;
        this->newValue = newValue;
    }


    void CmdSetProperty::Execute() {
        auto logicNode = Core::GraphManager::getInstance().findNodeByID(nodeID);
        if (!logicNode) return;

        for (auto& property : logicNode->properties()) {
            if (property.key == key) {
                if (!hasOldValue) {
                    oldValue = property.get();
                    hasOldValue = true;
                }
                property.set(newValue);
                return;
            }
        }
    }


    void CmdSetProperty::Undo() {
        auto logicNode = Core::GraphManager::getInstance().findNodeByID(nodeID);
        if (!logicNode) return;

        for (auto& property : logicNode->properties()) {
            if (property.key == key) {
                property.set(oldValue);
                return;
            }
        }
    }

}