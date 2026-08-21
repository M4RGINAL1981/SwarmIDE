#include "ui/canvas/PropertyInspector.hpp"
#include "nodes/core/TextNode.hpp"
#include "imgui.h"
#include <cstring>

namespace SwarmIDE::UI {

    PropertyInspector::PropertyInspector() { 
        RegisterDrawers(); 
    }

    void PropertyInspector::OpenForNode(UUIDv4::UUID nodeID) {
        activeNodeID = nodeID;
        openTrigger = true;
    }

    void PropertyInspector::RegisterDrawers() {
        drawers["TextNode"] = [this](Nodes::LogicNode* baseNode, bool& applyChanges) {
            auto* textNode = static_cast<Nodes::TextNode*>(baseNode);
            if (!textNode) return;

            if (!applyChanges) {
                // Заполняем буфер при первом появлении окна
                if (ImGui::IsWindowAppearing()) {
                    std::string text = textNode->getText();
                    strncpy(textBuffer, text.c_str(), sizeof(textBuffer) - 1);
                    textBuffer[sizeof(textBuffer) - 1] = '\0';
                }

                ImGui::InputTextMultiline("##text_content", textBuffer, sizeof(textBuffer), 
                                          ImVec2(-FLT_MIN, -FLT_MIN));
            } else {
                // Ветка применения изменений по нажатию кнопки Apply
                textNode->setText(std::string(textBuffer));
            }
        };
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
                bool applyChanges = false;
                
                if (node) {
                    ImGui::Text("Editing: %s", node->GetName().c_str());
                    ImGui::Separator();

                    ImGui::BeginChild("EditorRegion", ImVec2(0, -35), true);
                    std::string typeName = node->getTypeName();
                    if (drawers.contains(typeName)) {
                        drawers[typeName](node, applyChanges);
                    }
                    ImGui::EndChild();

                    // Кнопки управления
                    ImGui::SetCursorPosX(ImGui::GetWindowSize().x - 220);
                    if (ImGui::Button("Cancel", ImVec2(100, 0))) {
                        ImGui::CloseCurrentPopup();
                        activeNodeID = std::nullopt;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Apply", ImVec2(100, 0))) {
                        applyChanges = true;
                        if (drawers.contains(typeName)) { 
                            drawers[typeName](node, applyChanges); 
                        }
                        ImGui::CloseCurrentPopup();
                        activeNodeID = std::nullopt;
                    }
                }
            }
            ImGui::EndPopup();
        }
    }

} // SwarmIDE::UI