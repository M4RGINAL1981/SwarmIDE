#include "SwarmIDE.hpp"
#include "ui/windows/NotificationMenu.hpp"
#include "ui/windows/ThemeManager.hpp"
#include "imgui.h"

namespace SwarmIDE::UI {

    NotificationMenu& NotificationMenu::getInstance() {
        static NotificationMenu instance;
        return instance;
    }

    void NotificationMenu::AddNotification(const std::string& title, const std::string& message, const NotificationType type, float duration) {
        Notification notification = { title, message, type, duration };
        notifications.push_back(notification);
    }


    void NotificationMenu::Draw(float deltaTime) {
        // Если сообщений нет - выходим
        if (notifications.empty()) return;

        // Фиксируем окно в правом верхнем углу
        ImGuiIO& io = ImGui::GetIO();
        ImVec2 windowPos = ImVec2(io.DisplaySize.x - 360.0f, 40.0f);
        ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(340.0f, 0.0f), ImGuiCond_Always);

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | 
                                 ImGuiWindowFlags_NoInputs | 
                                 ImGuiWindowFlags_NoMove | 
                                 ImGuiWindowFlags_AlwaysAutoResize |
                                 ImGuiWindowFlags_NoFocusOnAppearing;

        for (auto it = notifications.begin(); it != notifications.end(); ) {
            it->duration -= deltaTime;
            if (it->duration <= 0.0f) {
                it = notifications.erase(it);
                continue;
            }

            ImU32 bgU32;
            auto& theme = ThemeManager::getInstance();

            switch (it->type) {
                case NotificationType::Error:   bgU32 = theme.GetColorU32(ColorRole::NotificationError); break;
                case NotificationType::Warning: bgU32 = theme.GetColorU32(ColorRole::NotificationWarning); break;
                default:                        bgU32 = theme.GetColorU32(ColorRole::NotificationInfo); break;
            }

            // ImGui::PushStyleColor может принимать ImVec4, поэтому конвертируем ImU32 -> ImVec4
            ImVec4 bgColor = ImGui::ColorConvertU32ToFloat4(bgU32);
            ImGui::PushStyleColor(ImGuiCol_WindowBg, bgColor);
            std::string winID = "##Notify_" + it->title + std::to_string(it->duration);
            
            ImGui::Begin(winID.c_str(), nullptr, flags);
            ImGui::Text(it->title.c_str()); 
            ImGui::Separator();
            ImGui::TextWrapped("%s", it->message.c_str());
            ImGui::End();
            
            ImGui::PopStyleColor();
            ++it;
        }
    }
}