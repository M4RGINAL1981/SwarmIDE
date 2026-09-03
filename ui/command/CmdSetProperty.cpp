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
        /** @todo find node → find descriptor by key → сохранить old → set(new) */
    }


    void CmdSetProperty::Undo() {
        Core::GraphManager::getInstance().findNodeByID(nodeID);
        /** @todo set(old). */
    }

}