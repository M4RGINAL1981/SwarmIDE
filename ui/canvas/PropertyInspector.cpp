#include "ui/canvas/PropertyInspector.hpp"
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"

#include <cfloat>
#include <stdexcept>

namespace {

    std::string VariantToString(const SwarmIDE::Core::DataVariant& value) {
        if (value.isType<std::string>()) return value.getValue<std::string>();
        if (value.isType<int>()) return std::to_string(value.getValue<int>());
        if (value.isType<float>()) return std::to_string(value.getValue<float>());
        if (value.isType<bool>()) return value.getValue<bool>() ? "true" : "false";
        return {};
    }

    SwarmIDE::Core::DataVariant StringToVariant(
        SwarmIDE::Core::PropertyKind kind,
        const std::string& value) {
        switch (kind) {
        case SwarmIDE::Core::PropertyKind::Int:
            return SwarmIDE::Core::DataVariant(std::stoi(value));
        case SwarmIDE::Core::PropertyKind::Float:
            return SwarmIDE::Core::DataVariant(std::stof(value));
        case SwarmIDE::Core::PropertyKind::Bool:
            return SwarmIDE::Core::DataVariant(value == "true" || value == "1");
        default:
            return SwarmIDE::Core::DataVariant(value);
        }
    }

}

namespace SwarmIDE::UI {

    void PropertyInspector::OpenForNode(UUIDv4::UUID nodeID) {
        activeNodeID = nodeID;
        editBuffers.clear();

        auto node = Core::GraphManager::getInstance().findNodeByID(nodeID);
        if (node) {
            for (const auto& property : node->properties()) {
                editBuffers[property.key] = VariantToString(property.get());
            }
        }

        openTrigger = true;
    }

    void PropertyInspector::Draw() {
        if (openTrigger) {
            ImGui::OpenPopup("Edit Node Data");
            openTrigger = false;
        }

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImVec2 size = ImGui::GetMainViewport()->Size;
        ImGui::SetNextWindowSize(ImVec2(size.x * 0.5f, size.y * 0.5f), ImGuiCond_Appearing);

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

        if (ImGui::BeginPopupModal("Edit Node Data", nullptr, flags)) {
            if (activeNodeID) {
                auto node = Core::GraphManager::getInstance().findNodeByID(activeNodeID.value());

                if (node) {
                    ImGui::Text("Editing: %s", node->GetName().c_str());
                    ImGui::Separator();

                    const auto properties = node->properties();
                    const float buttonsHeight = ImGui::GetFrameHeightWithSpacing();
                    ImGui::BeginChild("EditorRegion", ImVec2(0, -buttonsHeight), true);

                    if (properties.empty()) {
                        ImGui::TextUnformatted("This node has no editable properties");
                    }

                    for (const auto& property : properties) {
                        auto& buffer = editBuffers[property.key];
                        ImGui::PushID(property.key.c_str());
                        ImGui::TextUnformatted(property.label.c_str());

                        bool changed = false;
                        switch (property.kind) {
                        case Core::PropertyKind::Text:
                            changed = ImGui::InputText("##value", &buffer);
                            break;
                        case Core::PropertyKind::MultilineText:
                            changed = ImGui::InputTextMultiline("##value", &buffer, ImGui::GetContentRegionAvail());
                            break;
                        case Core::PropertyKind::Int: {
                            int value = 0;
                            try { value = std::stoi(buffer); } catch (const std::exception&) {}
                            if (ImGui::InputInt("##value", &value)) {
                                buffer = std::to_string(value);
                                changed = true;
                            }
                            break;
                        }
                        case Core::PropertyKind::Float: {
                            float value = 0.0f;
                            try { value = std::stof(buffer); } catch (const std::exception&) {}
                            if (ImGui::InputFloat("##value", &value)) {
                                buffer = std::to_string(value);
                                changed = true;
                            }
                            break;
                        }
                        case Core::PropertyKind::Bool: {
                            bool value = buffer == "true" || buffer == "1";
                            if (ImGui::Checkbox("##value", &value)) {
                                buffer = value ? "true" : "false";
                                changed = true;
                            }
                            break;
                        }
                        case Core::PropertyKind::Enum:
                        case Core::PropertyKind::ModelRef: {
                            int selected = -1;
                            for (size_t index = 0; index < property.enumOptions.size(); ++index) {
                                if (property.enumOptions[index] == buffer) {
                                    selected = static_cast<int>(index);
                                    break;
                                }
                            }
                            if (ImGui::Combo("##value", &selected, [](void* data, int index) -> const char* {
                                auto* options = static_cast<std::vector<std::string>*>(data);
                                if (index < 0 || index >= static_cast<int>(options->size())) return nullptr;
                                return (*options)[index].c_str();
                            }, const_cast<std::vector<std::string>*>(&property.enumOptions),
                                static_cast<int>(property.enumOptions.size()))) {
                                buffer = property.enumOptions[selected];
                                changed = true;
                            }
                            break;
                        }
                        }

                        if (property.readOnly) ImGui::BeginDisabled();
                        if (changed && property.readOnly) buffer = VariantToString(property.get());
                        if (property.readOnly) ImGui::EndDisabled();
                        ImGui::PopID();
                    }

                    ImGui::EndChild();

                    const float buttonWidth = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) * 0.5f;
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - buttonWidth * 2.0f - ImGui::GetStyle().ItemSpacing.x);
                    if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0))) {
                        ImGui::CloseCurrentPopup();
                        activeNodeID = std::nullopt;
                        editBuffers.clear();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Apply", ImVec2(buttonWidth, 0))) {
                        std::vector<std::unique_ptr<Command::Cmd>> commands;
                        for (const auto& property : properties) {
                            auto buffer = editBuffers.find(property.key);
                            if (!property.readOnly && buffer != editBuffers.end() && buffer->second != VariantToString(property.get())) {
                                try {
                                    commands.push_back(std::make_unique<Command::CmdSetProperty>(
                                        activeNodeID.value(), property.key,
                                        StringToVariant(property.kind, buffer->second)));
                                } catch (const std::exception&) {
                                    buffer->second = VariantToString(property.get());
                                }
                            }
                        }

                        if (commands.size() == 1) {
                            Command::CommandBus::getInstance().PushAndExecute(std::move(commands.front()));
                        } else if (!commands.empty()) {
                            auto batch = std::make_unique<Command::CmdBatch>();
                            for (auto& command : commands) batch->AddCommand(std::move(command));
                            Command::CommandBus::getInstance().PushAndExecute(std::move(batch));
                        }

                        ImGui::CloseCurrentPopup();
                        activeNodeID = std::nullopt;
                        editBuffers.clear();
                    }
                }
            }
            ImGui::EndPopup();
        }
    }

} // SwarmIDE::UI