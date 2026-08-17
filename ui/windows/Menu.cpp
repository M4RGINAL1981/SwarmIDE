#include "SwarmIDE.hpp"
#include "ui/windows/Menu.hpp"
#include "imgui.h"
#include "ui/command/CmdNodeOperations.hpp"
#include "ui/command/CommandBus.hpp"
#include "core/models/ModelManager.hpp"
#include "ui/windows/ThemeManager.hpp"
#include "graphics/Camera.hpp"

namespace SwarmIDE::UI {

    Menu& Menu::getInstance() {
        static Menu instance;
        return instance;
    }

    void Menu::Draw(Graphics::Camera* camera) {
        auto& theme = ThemeManager::getInstance();
        auto& command_bus = Command::CommandBus::getInstance();
        auto& model_manager = Core::ModelManager::getInstance();

        // Установка глобальных цветов меню (оттенки серого)
        /** @todo В будущем сделать загрузку всех цветов из ThemeManager */
        ImGui::PushStyleColor(ImGuiCol_WindowBg, theme.GetColorU32(ColorRole::MenuBackground)); 
        ImGui::PushStyleColor(ImGuiCol_Border, theme.GetColorU32(ColorRole::MenuBorder)); 
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(210, 210, 210, 255));                      // Основной текст
        ImGui::PushStyleColor(ImGuiCol_Header, IM_COL32(50, 50, 50, 255));                       // Фон Selectable
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, IM_COL32(70, 70, 70, 255));                // Hover
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, IM_COL32(90, 90, 90, 255));                 // Click
        ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(30, 30, 30, 255));                      // Поиск фон

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, theme.GetStyleVar(StyleVarRole::MenuRounding));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));

        // Применение базового шрифта
        if (theme.GetFontRegular()) ImGui::PushFont(theme.GetFontRegular());

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove | 
                                        ImGuiWindowFlags_NoResize | 
                                        ImGuiWindowFlags_NoCollapse |
                                        ImGuiWindowFlags_NoTitleBar;

        float scale = 1.0f;                                    
        float currentTopBarHeight = 75.0f * scale;             
        float menuWidth = 350.0f * scale;                 
        float paddingX = 10.0f * scale;                        
        float paddingY = 10.0f * scale;                        
        float windowHeight = ImGui::GetWindowHeight();

        glm::vec2 windowSize = glm::vec2((float)ImGui::GetWindowWidth(), (float)ImGui::GetWindowHeight());
        glm::vec2 center = windowSize / 2.0f;
        glm::vec2 centerWorldPos = camera->screenToWorld(center, windowSize);

        ImGui::SetNextWindowPos(ImVec2(paddingX, currentTopBarHeight + paddingY), ImGuiCond_Always); 
        ImGui::SetNextWindowSize(ImVec2(menuWidth, windowHeight - currentTopBarHeight - paddingY), ImGuiCond_Always);

        ImGui::Begin("SwarmIDE Spawn Panel", nullptr, window_flags);

        // --- ПОИСК ---
        static char searchBuffer[128] = "";
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::InputTextWithHint("##SearchNodes", "Search nodes...", searchBuffer, IM_ARRAYSIZE(searchBuffer));
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0.0f, 5.0f));

        // Универсальная функция отрисовки категорий без стрелок и лишних кнопок
        auto DrawCategory = [&](const char* title, const std::vector<std::string>& items, auto spawnAction) {
            // Заголовок категории жирным, более темным серым шрифтом
            if (theme.GetFontBold()) ImGui::PushFont(theme.GetFontBold());
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "%s", title);
            if (theme.GetFontBold()) ImGui::PopFont();
            
            ImGui::Dummy(ImVec2(0.0f, 2.0f));

            for (const auto& item : items) {
                // Фильтрация по поиску
                if (strlen(searchBuffer) > 0) {
                    std::string itemLower = item;
                    std::string searchLower = searchBuffer;
                    std::transform(itemLower.begin(), itemLower.end(), itemLower.begin(), ::tolower);
                    std::transform(searchLower.begin(), searchLower.end(), searchLower.begin(), ::tolower);
                    if (itemLower.find(searchLower) == std::string::npos) continue;
                }

                // Спавн по клику на элемент
                if (ImGui::Selectable(item.c_str(), false, 0, ImVec2(0, 25.0f))) {
                    spawnAction(item);
                }
            }
            ImGui::Dummy(ImVec2(0.0f, 10.0f));
        };

        // --- КАТЕГОРИЯ 1: AGENTS ---
        DrawCategory("AGENTS", { "ConversationalAgent", "ToolsAgent", "PlanExecuteAgent" }, [&](const std::string& name) {
            command_bus.PushAndExecute(std::make_unique<Command::CmdCreateNode>(name, name, centerWorldPos));
        });
        ImGui::Separator();

        // --- КАТЕГОРИЯ 2: CORE ---
        DrawCategory("CORE", { "TextNode", "MergeNode"}, [&](const std::string& name) {
            command_bus.PushAndExecute(std::make_unique<Command::CmdCreateNode>(name, name, centerWorldPos));
        });
        ImGui::Separator();

        // --- КАТЕГОРИЯ 3: MODELS ---
        auto& allAssets = model_manager.getAllAssets();
        std::vector<std::string> modelNames;
        for (const auto& asset : allAssets) {
            modelNames.push_back(asset->name);
        }
        DrawCategory("COGNITIVE MODELS", modelNames, [&](const std::string& name) {
            command_bus.PushAndExecute(std::make_unique<Command::CmdCreateNode>("ModelNode", name, centerWorldPos));
        });
        ImGui::Separator();

        // --- КАТЕГОРИЯ 3: MEMORY ---
        DrawCategory("STORAGE & MEMORY", { "PostgreSQL" }, [](const std::string&) { /** @todo Нужно будет сделать в будушем */ });
        ImGui::Separator();

        // --- КАТЕГОРИЯ 4: TOOLS ---
        DrawCategory("EXTERNAL TOOLS", { "JiraSoftware" }, [](const std::string&) { /** @todo Нужно будет сделать в будушем */ });

        ImGui::End();

        if (theme.GetFontRegular()) ImGui::PopFont();

        ImGui::PopStyleVar(3);
        ImGui::PopStyleColor(7);
    }
}