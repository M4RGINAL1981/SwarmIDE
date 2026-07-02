#include "ui/Menu.hpp"
#include "core/ObjectManager.hpp"
#include "imgui.h"
#include "graphics/Camera.hpp"

// Подключаем все типы объектов для инстанцирования
#include "object/ai/ConversationalAgent.hpp"
#include "object/ai/ToolsAgent.hpp"
#include "object/ai/PlanExecuteAgent.hpp"
#include "object/memory/PostgreSQL.hpp"
#include "object/model/Gemini.hpp"
#include "object/tool/JiraSoftware.hpp"

using namespace SwarmIDE::Object::AI;
using namespace SwarmIDE::Object::Memory;
using namespace SwarmIDE::Object::Model;
using namespace SwarmIDE::Object::Tool;

namespace SwarmIDE::UI {

    Menu& Menu::getInstance() {
        static Menu instance;
        return instance;
    }

    void Menu::Push(std::unique_ptr<SwarmIDE::Object::BaseObject> object) {
        if (manager) { 
            manager->addObject(std::move(object));
        }
    }

    void Menu::DrawMenu(Camera& camera) {
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.12f, 0.85f));

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove | 
                                        ImGuiWindowFlags_NoResize | 
                                        ImGuiWindowFlags_NoCollapse;

        // Позиционируем панель управления, например, в левом верхнем углу экрана
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(700, 2160), ImGuiCond_Always);

        ImGui::Begin("SwarmIDE Spawn Panel", nullptr, window_flags);
        
        glm::vec2 centerScreen(winSize.x * 0.5f, winSize.y * 0.5f);
        glm::vec2 spawnWorldPos = camera.screenToWorld(centerScreen, winSize);

        // --- КАТЕГОРИЯ 1: AI AGENTS ---
        if (ImGui::CollapsingHeader("AI Agents", ImGuiTreeNodeFlags_DefaultOpen)) {
            static int selectedAgent = 0;
            const char* agents[] = { "ConversationalAgent", "ToolsAgent", "PlanExecuteAgent" };
            ImGui::Combo("Type##Agent", &selectedAgent, agents, IM_ARRAYSIZE(agents));
            
            if (ImGui::Button("Spawn Agent")) {
                std::unique_ptr<SwarmIDE::Object::BaseObject> newObj = nullptr;
                if (selectedAgent == 0) newObj = std::make_unique<ConversationalAgent>("ConversationalAgent", spawnWorldPos);
                if (selectedAgent == 1) newObj = std::make_unique<ToolsAgent>("ToolsAgent", spawnWorldPos);
                if (selectedAgent == 2) newObj = std::make_unique<PlanExecuteAgent>("PlanExecuteAgent", spawnWorldPos);
                
                if (newObj) {
                    newObj->GetFonts(this->fonts);
                    Push(std::move(newObj));
                }
            }
        }

        ImGui::Separator();

        // --- КАТЕГОРИЯ 2: MODELS ---
        if (ImGui::CollapsingHeader("Cognitive Models", ImGuiTreeNodeFlags_DefaultOpen)) {
            static int selectedModel = 0;
            const char* models[] = { "GeminiLLM" };
            ImGui::Combo("Type##Model", &selectedModel, models, IM_ARRAYSIZE(models));
            
            if (ImGui::Button("Spawn Model")) {
                std::unique_ptr<SwarmIDE::Object::BaseObject> newObj = nullptr;
                if (selectedModel == 0) newObj = std::make_unique<Gemini>("Gemini", spawnWorldPos);
                
                if (newObj) {
                    newObj->GetFonts(this->fonts);
                    Push(std::move(newObj));
                }
            }
        }

        ImGui::Separator();

        // --- КАТЕГОРИЯ 3: MEMORY ---
        if (ImGui::CollapsingHeader("Storage & Memory", ImGuiTreeNodeFlags_DefaultOpen)) {
            static int selectedMemory = 0;
            const char* memories[] = { "PostgreSQL" };
            ImGui::Combo("Type##Memory", &selectedMemory, memories, IM_ARRAYSIZE(memories));
            
            if (ImGui::Button("Spawn Memory")) {
                std::unique_ptr<SwarmIDE::Object::BaseObject> newObj = nullptr;
                if (selectedMemory == 0) newObj = std::make_unique<PostgreSQL>("PostgreSQL", spawnWorldPos);
                
                if (newObj) {
                    newObj->GetFonts(this->fonts);
                    Push(std::move(newObj));
                }
            }
        }

        ImGui::Separator();

        // --- КАТЕГОРИЯ 4: TOOLS ---
        if (ImGui::CollapsingHeader("External Tools", ImGuiTreeNodeFlags_DefaultOpen)) {
            static int selectedTool = 0;
            const char* tools[] = { "JiraSoftware" };
            ImGui::Combo("Type##Tool", &selectedTool, tools, IM_ARRAYSIZE(tools));
            
            if (ImGui::Button("Spawn Tool")) {
                std::unique_ptr<SwarmIDE::Object::BaseObject> newObj = nullptr;
                if (selectedTool == 0) newObj = std::make_unique<JiraSoftware>("JiraSoftware", spawnWorldPos);
                
                if (newObj) {
                    newObj->GetFonts(this->fonts);
                    Push(std::move(newObj));
                }
            }
        }

        ImGui::End();
        ImGui::PopStyleColor();
    }
}