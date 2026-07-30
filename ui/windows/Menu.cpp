#include "SwarmIDE.hpp"
#include "ui/windows/Menu.hpp"
#include "imgui.h"
#include "ui/command/CmdNodeOperations.hpp"
#include "ui/command/CommandBus.hpp"
#include "core/models/ModelManager.hpp"
#include "ui/windows/ThemeManager.hpp"
#undef APIENTRY
#include <pfd/portable-file-dialogs.h>
#include <filesystem>
#include <thread>
#include <windows.h>
#pragma comment(lib, "urlmon.lib")

namespace SwarmIDE::UI {

    Menu& Menu::getInstance() {
        static Menu instance;
        return instance;
    }

    void Menu::DrawMenu() {

        auto& theme = ThemeManager::getInstance();

        auto backgroundColor = theme.GetColorU32(ColorRole::MenuBackground);
        auto rounding = theme.GetStyleVar(StyleVarRole::MenuRounding);
        auto borderColor = theme.GetColorU32(ColorRole::MenuBorder);

        ImGui::PushStyleColor(ImGuiCol_WindowBg, backgroundColor);
        ImGui::PushStyleColor(ImGuiCol_Border, borderColor);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, rounding);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.0f);

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove | 
                                        ImGuiWindowFlags_NoResize | 
                                        ImGuiWindowFlags_NoCollapse |
                                        ImGuiWindowFlags_NoTitleBar;

        // Позиционируем панель управления, например, в левом верхнем углу экрана
        ImGui::SetNextWindowPos(ImVec2(10, 60), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(700, 2160), ImGuiCond_Always);

        ImGui::Begin("SwarmIDE Spawn Panel", nullptr, window_flags);
        
        glm::vec2 centerScreen(winSize.x * 0.5f, winSize.y * 0.5f);
        glm::vec2 spawnWorldPos = camera->screenToWorld(centerScreen, winSize);
        auto& command_bus = Command::CommandBus::getInstance();
        auto& model_manager = Core::ModelManager::getInstance();

        // --- КАТЕГОРИЯ 1: AI AGENTS ---
        if (ImGui::CollapsingHeader("Agents", ImGuiTreeNodeFlags_DefaultOpen)) {
            static int selectedAgent = 0;
            const char* agents[] = { "ConversationalAgent", "ToolsAgent", "PlanExecuteAgent" };
            ImGui::Combo("Type##Agent", &selectedAgent, agents, IM_ARRAYSIZE(agents));
            
            if (ImGui::Button("Spawn Agent")) {
                if (selectedAgent == 0) command_bus.PushAndExecute(std::make_unique<Command::CmdCreateNode>("ConversationalAgent","ConversationalAgent"));
                if (selectedAgent == 1) command_bus.PushAndExecute(std::make_unique<Command::CmdCreateNode>("ToolsAgent","ToolsAgent"));
                if (selectedAgent == 2) command_bus.PushAndExecute(std::make_unique<Command::CmdCreateNode>("PlanExecuteAgent","PlanExecuteAgent"));
            }
        }

        ImGui::Separator();

        // --- КАТЕГОРИЯ 2: MODELS ---
        if (ImGui::CollapsingHeader("Cognitive Models", ImGuiTreeNodeFlags_DefaultOpen)) {
            static int selectedModel = 0;

            auto& allAssets = model_manager.getAllAssets(); 

            ImGui::Combo("Type##Model", &selectedModel, [](void* data, int idx) {
                auto* vecPtr = static_cast<const std::vector<std::unique_ptr<SwarmIDE::Core::ModelAsset>>*>(data);
                return (*vecPtr)[idx]->name.c_str(); 
            }, (void*)&allAssets, static_cast<int>(allAssets.size()));
            
            if (ImGui::Button("Spawn Model")) {
                if (selectedModel >= 0 && selectedModel < allAssets.size()) {
                    auto& modelData = allAssets[selectedModel];
                    
                    command_bus.PushAndExecute(std::make_unique<Command::CmdCreateNode>("ConversationalAgent","ConversationalAgent"));
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
                // Пока нету
            }
        }

        ImGui::Separator();

        // --- КАТЕГОРИЯ 4: TOOLS ---
        if (ImGui::CollapsingHeader("External Tools", ImGuiTreeNodeFlags_DefaultOpen)) {
            static int selectedTool = 0;
            const char* tools[] = { "JiraSoftware" };
            ImGui::Combo("Type##Tool", &selectedTool, tools, IM_ARRAYSIZE(tools));
            
            if (ImGui::Button("Spawn Tool")) {
                // Пока нету
            }
        }

        ImGui::End();
        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(2);
    }

    void Menu::DrawTopBar(GLFWwindow* window) {

        bool isFpsCounterVisible = false;

        // Отрисовка верхнего главного меню
        if (ImGui::BeginMainMenuBar()) {

            // FILE
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("New Canvas"))       // { serialization_manager->getInstance().ClearGraph(); }


                if (ImGui::MenuItem("Open...", "Ctrl+O")) { 
                    auto openDialog = pfd::open_file(
                        "Open SwarmIDE Scene",
                        "", // Начальный путь пуст (откроется текущая папка)
                        { "SwarmIDE Project (*.json)", "*.json" }
                    );

                    auto selectedFiles = openDialog.result();
                    if(!selectedFiles.empty()) {
                        std::string selectedPath = selectedFiles[0];
                        //serialization_manager->getInstance().LoadGraph(selectedPath);
                    }
                }


                if (ImGui::MenuItem("Save", "Ctrl+S"))    { 

                    // 1. Получаем следующее свободное имя, например, "newSceneData1.json"
                    //std::string defaultFileName = serialization_manager->getInstance().GetNextAvailableFileName("newSceneData");

                    // 2. Открываем нативное окно "Сохранить файл"
                    auto saveDialog = pfd::save_file(
                        "Save SwarmIDE Scene",                    // Заголовок окна
                   //     defaultFileName,                          // Имя файла по умолчанию
                        { "SwarmIDE Project (*.json)", "*.json"}  // Фильтр расширений файлов
                    );

                    // 3. Если пользователь выбрал путь и нажал "Сохранить"
                    std::string selectedPath = saveDialog.result();
                    //if(!selectedPath.empty()) serialization_manager->getInstance().SaveGraph(selectedPath);
                }


                ImGui::Separator();
                if (ImGui::BeginMenu("Settings")) {
                    if (ImGui::BeginMenu("Models")) {  // Данный раздел позволяет узнать название и статус всех моделей и загрузить модели 
                        
                        //auto& allModels = model_manager->GetModels();                   // Получаем все модели
                        
                        /*
                        for (auto& modelData : allModels) {                             // Проходимся по всем моделям
                            modelData.isDownloaded = model_manager->isDownloadedModelForID(modelData.id); 
                            
                            if (!modelData.isDownloaded) {                              // Проверяем скачана ли модель
                                std::string menuLabel = "Download " + modelData.name;   
                                ImGui::TextDisabled("%s", menuLabel.c_str());           // Указываем на кнопке название модели


                                if (ImGui::MenuItem(menuLabel.c_str())) {               // Скачиваем модель, если кнопка нажата
                                    std::thread downloadThread([this, id = modelData.id]() { model_manager->downloadModelForID(id); });
                                    downloadThread.detach();
                                }
                            } 
                            else {
                                std::string statusLabel = modelData.name + " (Ready)"; // Если модель скачана, выводим соответсвующую кнопку
                                ImGui::TextDisabled("%s", statusLabel.c_str());
                            }
                        }
                            */
                       // DrawDownloadModel();                                           // Отображаем загрузку моделей.
                        ImGui::EndMenu();
                    }
                    ImGui::EndMenu();
                }

                ImGui::Separator();

                if (ImGui::MenuItem("Exit", "Alt+F4")) { glfwSetWindowShouldClose(window, true); }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View")) {
                if (ImGui::MenuItem("Reset Camera", "Ctrl+R")) { camera->reset(); }
                if (ImGui::MenuItem("Toggle Grid", "Ctrl+G")) { grid->toggleVisibility(); }
                ImGui::EndMenu();
            }
            
            if (ImGui::BeginMenu("Edit")) {
              //  if (ImGui::MenuItem("Clear Connections")) { connect_manager->removeAllConnections(); }
              //  if (ImGui::MenuItem("Clear Objects")) { serialization_manager->getInstance().ClearGraph(); }
               // if (ImGui::MenuItem("Clear All")) { connect_manager->removeAllConnections(); serialization_manager->getInstance().ClearGraph(); }
               // if (ImGui::MenuItem("FPS Counter")) { isFpsCounterVisible = !isFpsCounterVisible; }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Simulation")) {
             //   if (ImGui::MenuItem("Run Swarm", "F5")) { Запуск ядра CUDA  }
              //  if (ImGui::MenuItem("Stop", "Shift+F5")) { Остановка  }
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

    /*
    void Menu::DrawDownloadModel() {
       // auto models = model_manager->GetModels();

        // Размеры и позиция окна полосы загрузки
        ImVec2 windowSize = ImVec2(500.0f, 120.0f); // Уменьшил высоту, 200 много для одной полосы
        ImVec2 windowPos  = ImVec2(1000.0f, 800.0f);

        ImGuiWindowFlags flags = 
                    ImGuiWindowFlags_NoResize   |               
                    ImGuiWindowFlags_NoMove     |               
                    ImGuiWindowFlags_NoInputs   |               
                    ImGuiWindowFlags_NoCollapse |                      
                    ImGuiWindowFlags_NoTitleBar |               
                    ImGuiWindowFlags_NoBringToFrontOnFocus |    
                    ImGuiWindowFlags_NoFocusOnAppearing;        

        for (size_t i = 0; i < models.size(); ++i) {
            auto& model = models[i];

            if (model.isDownloading) {
                // Создаем уникальное имя окна для ImGui на основе индекса, чтобы избежать конфликтов ID
                std::string window_id = model.name + "##download_win_" + std::to_string(i);

                ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);
                ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always); 

                if (ImGui::Begin(window_id.c_str(), nullptr, flags)) {
                    // Рассчитываем размер самой полосы: во всю ширину окна минус отступы по 20px по бокам
                    // Высоту полосы сделаем фиксированной 30px

                    ImGui::Text(model.name.c_str());
                    ImVec2 progressBarSize = ImVec2(windowSize.x - 40.0f, 30.0f);
                    
                    // Отрисовываем полосу загрузки
                    DrawProgressBar(model.progress, progressBarSize);  
                }
                ImGui::End();

                // Если окон должно быть несколько, смещаем координату Y для следующего окна вниз
                windowPos.y += windowSize.y + 10.0f; 
            }
        }
    }

    void Menu::DrawProgressBar(float progress, ImVec2 size) {
        // 1. Нормализуем прогресс (ожидаем диапазон от 0.0f до 1.0f)
        if (progress < 0.0f) progress = 0.0f;
        if (progress > 1.0f) progress = 1.0f;

        // 2. Получаем текущую экранную позицию курсора внутри окна ImGui
        // Это гарантирует, что даже если окно сдвинется, рисунок сдвинется вместе с ним
        ImVec2 start_pos = ImGui::GetCursorScreenPos();

        // Смещаем курсор немного вниз и вправо для красоты (внутренний отступ от краев окна)
        start_pos.x += 20.0f;
        start_pos.y += 20.0f;

        // Резервируем место в окне под элемент (с учетом смещения)
        ImGui::Dummy(ImVec2(size.x + 20.0f, size.y + 20.0f));  

        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        // 3. Вычисляем крайние точки рамки полосы
        ImVec2 min_p = start_pos;
        ImVec2 max_p = ImVec2(start_pos.x + size.x, start_pos.y + size.y);

        ImU32 color_white  = IM_COL32(255, 255, 255, 255);
        ImU32 color_black  = IM_COL32(0, 0, 0, 255);
        ImU32 color_purple = IM_COL32(140, 20, 250, 255);

        // 4. Рисуем белый фон полосы
        draw_list->AddRectFilled(min_p, max_p, color_white);
        
        // 5. Рисуем фиолетовое заполнение (с учетом внутренних отступов)
        if (progress > 0.0f) {
            float padding = 3.0f; // Внутренний отступ фиолетовой полосы от черной рамки

            // Левый верхний угол заполнения
            ImVec2 fill_min = ImVec2(min_p.x + padding, min_p.y + padding);
            
            // Максимально доступная ширина для заполнения
            float max_fill_width = size.x - (padding * 2.0f);
            // Текущая ширина в зависимости от прогресса
            float current_fill_width = max_fill_width * progress;

            // Правый нижний угол заполнения
            ImVec2 fill_max = ImVec2(fill_min.x + current_fill_width, max_p.y - padding);

            // Рисуем фиолетовый прямоугольник только если ширина больше нуля
            if (current_fill_width > 0.0f) {
                draw_list->AddRectFilled(fill_min, fill_max, color_purple);
            }
        }

        // 6. Рисуем черную окантовку поверх всего (0.0f — без скругления углов)
        draw_list->AddRect(min_p, max_p, color_black, 0.0f, 0, 1.5f);
    }

    */
}
