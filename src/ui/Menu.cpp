#include "ui/Menu.hpp"
#include "core/AgentManager.hpp"
#include "ai/ConversationalAgent.hpp"
#include "ai/ToolsAgent.hpp"
#include "ai/PlanExecuteAgent.hpp"
#include "imgui.h"
#include "graphics/Camera.hpp"

using namespace SwarmIDE::AI;

namespace SwarmIDE::UI {

    Menu& Menu::getInstance() {
        static Menu instance;
        return instance;
    }

    void Menu::Push(std::unique_ptr<SwarmIDE::AI::BaseAgent> agent) { // Логика
        if (manager) { 
            manager->addAgent(std::move(agent));
        }
    }

    void Menu::DrawMenu(Camera& camera){ // Отрисовка
        // Настраиваем прозрачность фона (например, 0.2f - почти прозрачный)
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.2f));
        
        // Устанавливаем размер на всю высоту (2160) и фиксированную ширину (например, 300)
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(600, 2160));

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove | 
                                        ImGuiWindowFlags_NoResize | 
                                        ImGuiWindowFlags_NoCollapse |
                                        ImGuiWindowFlags_NoBackground;

        ImGui::Begin("Agent Controller", nullptr, window_flags);

        static int selected_type = 0;
        const char* items[] = { "ConversationAgent", "ToolsAgent", "PlanExecuteAgent" };
        ImGui::Combo("Select Agent Type", &selected_type, items, IM_ARRAYSIZE(items));
        std::cout << "selecte agent type " << selected_type << std::endl;

        if(ImGui::Button("Create Selected")) {
            glm::vec2 centerScreen(winSize.x, winSize.y);
            glm::vec2 WorldPos = camera.screenToWorld(centerScreen, winSize);

            // Создаем пустой умный указатель на базовый класс
            std::unique_ptr<SwarmIDE::AI::BaseAgent> newAgent = nullptr;

            // Выделяем память под конкретный тип агента
            if(selected_type == 0) 
                newAgent = std::make_unique<ConversationalAgent>("New Conversational", WorldPos);
            if(selected_type == 1) 
                newAgent = std::make_unique<ToolsAgent>("New ToolsAgent", WorldPos);
            if(selected_type == 2) 
                newAgent = std::make_unique<PlanExecuteAgent>("New PlanExecuteAgent", WorldPos);
            
            if (newAgent != nullptr) {
                // Передаем шрифты в агента перед тем, как отдать его менеджеру
                newAgent->GetFonts(this->fonts);
                // Передаем владение агентом в менеджер
                manager->addAgent(std::move(newAgent));
            }
        }

        ImGui::SetCursorPos(ImVec2(10, 2000)); // Примерно внизу для разрешения 2160
        if(ImGui::Button("Go to World Origin")) {
            camera.reset();
        }

        ImGui::End();
        ImGui::PopStyleColor(); // Возвращаем стиль
    }

}