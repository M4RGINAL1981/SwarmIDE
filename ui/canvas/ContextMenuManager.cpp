#include "SwarmIDE.hpp"
#include "ui/canvas/ContextMenuManager.hpp"
#include "core/engine/LoggingManager.hpp"
#include "ui/canvas/PropertyInspector.hpp"
#include "core/engine/SerializationManager.hpp"
#include "ui/canvas/CanvasManager.hpp"
#include "ui/command/CmdNodeOperations.hpp"
#include "ui/command/CmdBatch.hpp"

namespace SwarmIDE::UI {

    ContextMenuManager& ContextMenuManager::getInstance() {
        static ContextMenuManager instance;
        return instance;
    }

    void ContextMenuManager::RequestMenu(glm::vec2 mousePos, std::optional<UUIDv4::UUID> optionalNodeID) {
        nodeID = optionalNodeID;
        ImGui::OpenPopup("CanvasContextMenu");
    }

    void ContextMenuManager::Draw() {
        if (ImGui::BeginPopup("CanvasContextMenu")) {
            bool isNodeClicked = nodeID.has_value();

            if (isNodeClicked) {
                ImGui::TextDisabled("Node Operations");
                ImGui::Separator();

                if (ImGui::MenuItem("Edit Node Data")) {
                    PropertyInspector::getInstance().OpenForNode(nodeID.value());
                }

                ImGui::Separator();

                if (ImGui::MenuItem("Duplicate Selected")) {
                    const auto& selected = CanvasManager::getInstance().GetSelectedNodesID();
                    
                    if (selected.contains(nodeID.value()) && selected.size() > 1) {
                        auto batch = std::make_unique<Command::CmdBatch>();
                        for (const auto& id : selected) {
                            batch->AddCommand(std::make_unique<Command::CmdDuplicateNode>(id));
                        }
                        command_bus->PushAndExecute(std::move(batch));
                    } else {
                        auto cmd = std::make_unique<Command::CmdDuplicateNode>(nodeID.value());
                        command_bus->PushAndExecute(std::move(cmd));
                    }
                }

                ImGui::Separator();

                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
                if (ImGui::MenuItem("Delete Selected")) {
                    const auto& selected = CanvasManager::getInstance().GetSelectedNodesID();
                    
                    if (selected.contains(nodeID.value()) && selected.size() > 1) {
                        auto batch = std::make_unique<Command::CmdBatch>();
                        for (const auto& id : selected) {
                            batch->AddCommand(std::make_unique<Command::CmdDeleteNode>(id));
                        }
                        command_bus->PushAndExecute(std::move(batch));
                        CanvasManager::getInstance().ClearSelection();
                    } else {
                        auto cmd = std::make_unique<Command::CmdDeleteNode>(nodeID.value());
                        command_bus->PushAndExecute(std::move(cmd));
                        CanvasManager::getInstance().DeselectNode(nodeID.value());
                    }
                }
                ImGui::PopStyleColor();
            }
            else {
                ImGui::TextDisabled("Canvas Options");
                ImGui::Separator();

                if (ImGui::MenuItem("Clear Canvas")) {
                    Core::LoggingManager::Info("[CONTEXT_MENU_MANAGER]: Clear Canvas");
                    Core::SerializationManager::getInstance().ClearGraph();
                    CanvasManager::getInstance().removeAllViewNodes();
                    CanvasManager::getInstance().ClearSelection();
                }
            }
            ImGui::EndPopup();
        }
    }

} // SwarmIDE::UI