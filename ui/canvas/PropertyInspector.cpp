#include "ui/canvas/PropertyInspector.hpp"
#include "imgui.h"
#include <cstring>

namespace SwarmIDE::UI {

    void PropertyInspector::OpenForNode(UUIDv4::UUID nodeID) {
        activeNodeID = nodeID;
        openTrigger = true;
    }

    void PropertyInspector::Draw() {
        if (openTrigger) {
            ImGui::OpenPopup("Edit Node Data");
            openTrigger = false;
        }

        // Центрируем окно при появлении
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

                    ImGui::BeginChild("EditorRegion", ImVec2(0, -35), true);
                    std::string typeName = node->getTypeName();
                    ImGui::EndChild();

                    // Кнопки управления
                    ImGui::SetCursorPosX(ImGui::GetWindowSize().x - 220);
                    if (ImGui::Button("Cancel", ImVec2(100, 0))) {
                        ImGui::CloseCurrentPopup();
                        activeNodeID = std::nullopt;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Apply", ImVec2(100, 0))) {
                        ImGui::CloseCurrentPopup();
                        activeNodeID = std::nullopt;
                    }
                }
            }
            ImGui::EndPopup();
        }
    }

} // SwarmIDE::UI