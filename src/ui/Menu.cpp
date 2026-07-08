#include "ui/Menu.hpp"
#include "imgui.h"
#undef APIENTRY
#include <pfd/portable-file-dialogs.h>
#include <filesystem>
#include <windows.h>
#pragma comment(lib, "urlmon.lib")

// Подключаем все типы объектов для инстанцирования
#include "object/ai/ConversationalAgent.hpp"
#include "object/ai/ToolsAgent.hpp"
#include "object/ai/PlanExecuteAgent.hpp"
#include "object/BaseModel.hpp"
#include "object/memory/PostgreSQL.hpp"
#include "object/tool/JiraSoftware.hpp"

using namespace SwarmIDE::Object::AI;
using namespace SwarmIDE::Object::Memory;
using namespace SwarmIDE::Object::Tool;

namespace SwarmIDE::UI {

    Menu& Menu::getInstance() {
        static Menu instance;
        return instance;
    }

    void Menu::Push(std::unique_ptr<SwarmIDE::Object::BaseObject> object) {
        if (object_manager) { 
            object_manager->addObject(std::move(object));
        }
    }

    void Menu::DrawMenu(Camera& camera) {
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.12f, 0.85f));

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove | 
                                        ImGuiWindowFlags_NoResize | 
                                        ImGuiWindowFlags_NoCollapse;

        // Позиционируем панель управления, например, в левом верхнем углу экрана
        ImGui::SetNextWindowPos(ImVec2(10, 60), ImGuiCond_Always);
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
                    newObj->SetFonts(this->fonts);
                    Push(std::move(newObj));
                }
            }
        }

        ImGui::Separator();

        // --- КАТЕГОРИЯ 2: MODELS ---
        if (ImGui::CollapsingHeader("Cognitive Models", ImGuiTreeNodeFlags_DefaultOpen)) {
            static int selectedModel = 0;

            auto& allModels = model_manager->getInstance().GetModels(); 

            ImGui::Combo("Type##Model", &selectedModel, [](void* data, int idx) {
                auto* vector = static_cast<std::vector<SwarmIDE::Core::Model>*>(data);
                return (*vector)[idx].name.c_str(); 
            }, (void*)&allModels, static_cast<int>(allModels.size()));
            
            if (ImGui::Button("Spawn Model")) {
                if (selectedModel >= 0 && selectedModel < allModels.size()) {
                    auto& modelData = allModels[selectedModel];
                    
                    auto newObj = std::make_unique<SwarmIDE::Object::BaseModel>(modelData.name, spawnWorldPos, &modelData);
                    
                    newObj->SetFonts(this->fonts);
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
                    newObj->SetFonts(this->fonts);
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
                    newObj->SetFonts(this->fonts);
                    Push(std::move(newObj));
                }
            }
        }

        ImGui::End();
        ImGui::PopStyleColor();
    }

    void Menu::DrawTopBar(GLFWwindow* window, Camera& camera) {

        bool isFpsCounterVisible = false;

        // Отрисовка верхнего главного меню
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("New Canvas"))        { serialization_manager->getInstance().ClearGraph(); }


                if (ImGui::MenuItem("Open...", "Ctrl+O")) { 
                    auto openDialog = pfd::open_file(
                        "Open SwarmIDE Scene",
                        "", // Начальный путь пусток (откроется текущая папка)
                        { "SwarmIDE Project (*.json)", "*.json" }
                    );

                    auto selectedFiles = openDialog.result();
                    if(!selectedFiles.empty()) {
                        std::string selectedPath = selectedFiles[0];
                        serialization_manager->getInstance().LoadGraph(selectedPath);
                    }
                }


                if (ImGui::MenuItem("Save", "Ctrl+S"))    { 

                    // 1. Получаем следующее свободное имя, например, "newSceneData1.json"
                    std::string defaultFileName = serialization_manager->getInstance().GetNextAvailableFileName("newSceneData");

                    // 2. Открываем нативное окно "Сохранить файл"
                    auto saveDialog = pfd::save_file(
                        "Save SwarmIDE Scene",                    // Заголовок окна
                        defaultFileName,                          // Имя файла по умолчанию
                        { "SwarmIDE Project (*.json)", "*.json"}  // Фильтр расширений файлов
                    );

                    // 3. Если пользователь выбрал путь и нажал "Сохранить"
                    std::string selectedPath = saveDialog.result();
                    if(!selectedPath.empty()) serialization_manager->getInstance().SaveGraph(selectedPath);
                }


                ImGui::Separator();
                if (ImGui::BeginMenu("Settings")) {
                    if (ImGui::BeginMenu("Models")) {
                        
                        auto& allModels = model_manager->GetModels(); 
                        
                        for (auto& modelData : allModels) {
                            modelData.isDownloaded = std::filesystem::exists(modelData.path);
                            
                            if (!modelData.isDownloaded) {
                                std::string menuLabel = "Download " + modelData.name;
                                if (ImGui::MenuItem(menuLabel.c_str())) {
                                    
                                    std::filesystem::create_directories("models");
                                    
                                    pfd::message("Download", "Starting download for " + modelData.name + "\nThis may take a while...", pfd::choice::ok);
                                    
                                    HRESULT hr = URLDownloadToFileA(NULL, modelData.downloadUrl.c_str(), modelData.path.c_str(), 0, NULL);
                                    
                                    if (SUCCEEDED(hr)) {
                                        modelData.isDownloaded = true;
                                        pfd::message("Success", modelData.name + " downloaded successfully!", pfd::choice::ok);
                                    } else {
                                        pfd::message("Error", "Failed to download " + modelData.name, pfd::choice::ok);
                                    }
                                }
                            } else {
                                std::string statusLabel = modelData.name + " (Ready)";
                                ImGui::TextDisabled("%s", statusLabel.c_str());
                            }
                        }

                        ImGui::EndMenu();
                    }
                    ImGui::EndMenu();
                }

                ImGui::Separator();

                if (ImGui::MenuItem("Exit", "Alt+F4")) { glfwSetWindowShouldClose(window, true); }
                ImGui::EndMenu();
            }

            
            
            if (ImGui::BeginMenu("Edit")) {
                if (ImGui::MenuItem("Clear Connections")) { connect_manager->removeAllConnections(); }
                if (ImGui::MenuItem("FPS Counter")) { isFpsCounterVisible = !isFpsCounterVisible; }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Simulation")) {
                if (ImGui::MenuItem("Run Swarm", "F5")) { /* Запуск ядра CUDA */ }
                if (ImGui::MenuItem("Stop", "Shift+F5")) { /* Остановка */ }
                ImGui::EndMenu();
            }

            // Индикатор FPS
            if (isFpsCounterVisible) {
                float fps = ImGui::GetIO().Framerate;
                std::string fpsText = "FPS: " + std::to_string((int)fps);
                ImGui::SameLine(ImGui::GetWindowWidth() - ImGui::CalcTextSize(fpsText.c_str()).x - 20);
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%s", fpsText.c_str());
            }


            ImGui::EndMainMenuBar();
        }
    }

}