#include "SwarmIDE.hpp"
#include "nodes/core/MergeNode.hpp"
#include "core/models/ModelManager.hpp"
#include "core/engine/LoggingManager.hpp"

namespace SwarmIDE::Nodes {

    MergeNode::MergeNode(const std::string& nodeName) : LogicNode(nodeName) {
        // Установка входных пинов
        addPin("Data1", Core::PinType::DataStream, Core::Direction::Input, Core::DataType::STRING);
        addPin("Data2", Core::PinType::DataStream, Core::Direction::Input, Core::DataType::STRING);

        // Установка выходных пинов
        addPin("Output", Core::PinType::DataStream, Core::Direction::Output, Core::DataType::STRING);
    }

    void MergeNode::Execute() {
        SetState(Executing);
        Core::LoggingManager::ExecutingNode("MergeNode", GetID());

        auto readStringValue = [&](const char* pinName) -> std::string {
            auto pinValue = getPinValue(pinName);
            if (pinValue.isEmpty()) {
                Core::LoggingManager::Warning("MergeNode input pin ", pinName, " is empty.");
                return {};
            }

            if (pinValue.isType<std::string>()) {
                return pinValue.getValue<std::string>();
            }

            if (pinValue.isType<int>()) {
                return std::to_string(pinValue.getValue<int>());
            }

            if (pinValue.isType<float>()) {
                return std::to_string(pinValue.getValue<float>());
            }

            if (pinValue.isType<nlohmann::json>()) {
                return pinValue.getValue<nlohmann::json>().dump();
            }

            Core::LoggingManager::Warning("MergeNode input pin ", pinName, " has an unsupported value type.");
            return {};
        };

        try {
            const std::string answer1 = readStringValue("Data1");
            const std::string answer2 = readStringValue("Data2");
            const std::string mergedValue = answer1 + answer2;

            setPinValue("Output", Core::DataVariant(mergedValue));

            if (mergedValue.empty()) {
                Core::LoggingManager::Warning("MergeNode produced an empty output because no input values were available.");
            }

            SetState(Success);
            Core::LoggingManager::SuccessNode("MergeNode", GetID());
        }
        catch (const std::exception& e) {
            SetState(Failed);
            Core::LoggingManager::Error("MergeNode execution failed: ", e.what());
            Core::LoggingManager::FailedNode("MergeNode", GetID());
        }
        catch (...) {
            SetState(Failed);
            Core::LoggingManager::Error("MergeNode execution failed with an unknown exception.");
            Core::LoggingManager::FailedNode("MergeNode", GetID());
        }
    }

    void MergeNode::ApplyDefaultSettings() {
        SetName("MergeNode");
    }

    // РЕГИСТРАЦИЯ НОДЫ В ФАБРИКЕ
    REGISTER_NODE(MergeNode, "MergeNode");
}