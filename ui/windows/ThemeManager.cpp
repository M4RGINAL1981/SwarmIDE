#include "SwarmIDE.hpp"
#include "ui/windows/ThemeManager.hpp"
#include "core/engine/LoggingManager.hpp"
#include <iostream>
#include <fstream>
#include <json/json.hpp> // Твой nlohmann::json

using json = nlohmann::json;

namespace SwarmIDE::UI {

    ThemeManager& ThemeManager::getInstance() {
        static ThemeManager instance;
        return instance;
    }

    ThemeManager::ThemeManager() {
        Core::LoggingManager::Info("[THEME_MANAGER]: Constructor");
        Core::LoggingManager::Info("[THEME MANAGER]: Loading themes from ", themesPath);

        std::ifstream modelsFile(themesPath);
        if (!modelsFile.is_open()) {
            Core::LoggingManager::Fatal("[ERROR]: ThemeManager could not find themes at: ", themesPath);
            return;
        }

        try {
            json root;
            modelsFile >> root;
            
            if(root.contains("themes")) {
                for (const auto& theme : root["themes"]) {
                    availableThemes.push_back(theme["ThemeName"]);
                }
                
                // Загружаем первую тему по умолчанию
                if (!availableThemes.empty()) {
                    SwitchTheme(availableThemes[0]);
                }
            }
        } catch (const std::exception& e) {
            Core::LoggingManager::Fatal("[ERROR]: JSON Parse Error in themes: ", e.what());
        }
        
        Core::LoggingManager::Info("[THEME MANAGER]: Themes loaded successfully.");
    }

    void ThemeManager::SwitchTheme(const std::string& themeName) {
        std::ifstream modelsFile(themesPath);
        json root;
        modelsFile >> root;

        for (const auto& theme : root["themes"]) {
            if (theme["ThemeName"] == themeName) {
                currentThemeName = themeName;
                
                // Парсим цвета Сетки
                if (theme.contains("Grid")) {
                    currentColorsHex[ColorRole::GridBackground] = theme["Grid"].value("BackgroundColor", "#1E1E1E");
                    currentColorsHex[ColorRole::GridLine] = theme["Grid"].value("LineColor", "#2A2A2A");
                    currentStyleVars[StyleVarRole::GridStep] = theme["Grid"].value("Step", 50.0f);
                    currentStyleVars[StyleVarRole::GridThickness] = theme["Grid"].value("Thickness", 1.5f);
                }

                // Парсим цвета Меню
                if (theme.contains("Menu")) {
                    currentColorsHex[ColorRole::MenuBackground] = theme["Menu"].value("BackgroundColor", "#252526");
                    currentColorsHex[ColorRole::MenuBorder] = theme["Menu"].value("BorderColor", "#333333");
                    currentStyleVars[StyleVarRole::MenuRounding] = theme["Menu"].value("Rounding", 6.0f);
                }

                // Парсим цвета Нод
                if (theme.contains("Node")) {
                    currentStyleVars[StyleVarRole::NodeRounding] = theme["Node"].value("Rounding", 8.0f);
                    currentStyleVars[StyleVarRole::NodeBorderThickness] = theme["Node"].value("BorderThickness", 1.0f);
                    currentColorsHex[ColorRole::NodeBorder] = theme["Node"].value("BorderColor", "#000000");
                    currentColorsHex[ColorRole::NodeHover] = theme["Node"].value("HoverColor", "#3F3F46");

                    if (theme["Node"].contains("States")) {
                        auto states = theme["Node"]["States"];
                        currentColorsHex[ColorRole::NodeStateIdle] = states.value("Idle", "#333333");
                        currentColorsHex[ColorRole::NodeStateWaiting] = states.value("Waiting", "#D7BA7D");
                        currentColorsHex[ColorRole::NodeStateExecuting] = states.value("Executing", "#007ACC");
                        currentColorsHex[ColorRole::NodeStateSuccess] = states.value("Success", "#4CAF50");
                        currentColorsHex[ColorRole::NodeStateFailed] = states.value("Failed", "#F44336");
                    }
                }

                // Парсим Уведомления
                if(theme.contains("Notification")) {
                    currentColorsHex[ColorRole::NotificationInfo] = theme["Notification"].value("NotificationInfo", "#00CCCC80");
                    currentColorsHex[ColorRole::NotificationWarning] = theme["Notification"].value("NotificationWarning", "#F3E60080");
                    currentColorsHex[ColorRole::NotificationError] = theme["Notification"].value("NotificationError", "#B5352C80");
                }

                // Парсим Пины
                if (theme.contains("Pin")) {
                    currentStyleVars[StyleVarRole::PinRadius] = theme["Pin"].value("Radius", 5.0f);
                    currentColorsHex[ColorRole::PinDefault] = theme["Pin"].value("ColorDefault", "#CCCCCC");
                    currentColorsHex[ColorRole::PinHover] = theme["Pin"].value("ColorHover", "#FFFFFF");
                    
                    std::string shape = theme["Pin"].value("Shape", "Circle");
                    currentPinShape = (shape == "Square") ? PinShapeType::Square : PinShapeType::Circle;
                }

                // Парсим Соединения
                if (theme.contains("Connection")) {
                    currentStyleVars[StyleVarRole::ConnectionThickness] = theme["Connection"].value("Thickness", 3.0f);
                    currentStyleVars[StyleVarRole::ConnectionStrength] = theme["Connection"].value("Strength", 25.0f);
                    currentColorsHex[ColorRole::ConnectionLine] = theme["Connection"].value("Color", "#A0A0A0");
                    
                    std::string style = theme["Connection"].value("Style", "Solid");
                    if (style == "Dashed") currentConnectionStyle = ConnectionStyleType::Dashed;
                    else if (style == "Dotted") currentConnectionStyle = ConnectionStyleType::Dotted;
                    else currentConnectionStyle = ConnectionStyleType::Solid;
                }
                
                Core::LoggingManager::Info("[THEME MANAGER]: Switched to theme: ", themeName);
                break;
            }
        }
    }

    ImU32 ThemeManager::ParseHexColor(const std::string& hexStr) {
        if (hexStr.empty() || hexStr[0] != '#') return IM_COL32(255, 0, 255, 255); // Ярко-розовый в случае ошибки
        
        std::string hex = hexStr.substr(1);
        if (hex.length() == 6) hex += "FF"; // Добавляем альфа-канал, если его нет
        
        unsigned long value = std::stoul(hex, nullptr, 16);
        return IM_COL32((value >> 24) & 0xFF, (value >> 16) & 0xFF, (value >> 8) & 0xFF, value & 0xFF);
    }

    glm::vec3 ThemeManager::ParseHexColorVec3(const std::string& hexStr) {
        if (hexStr.empty() || hexStr[0] != '#') return glm::vec3(1.0f, 0.0f, 1.0f);
        
        std::string hex = hexStr.substr(1);
        unsigned long value = std::stoul(hex, nullptr, 16);
        
        return glm::vec3(
            ((value >> 16) & 0xFF) / 255.0f,
            ((value >> 8) & 0xFF) / 255.0f,
            (value & 0xFF) / 255.0f
        );
    }

    ImU32 ThemeManager::GetColorU32(ColorRole role) {
        if (currentColorsHex.find(role) != currentColorsHex.end()) {
            return ParseHexColor(currentColorsHex[role]);
        }
        return IM_COL32(255, 0, 255, 255); // Ошибка
    }

    glm::vec3 ThemeManager::GetColorVec3(ColorRole role) {
        if (currentColorsHex.find(role) != currentColorsHex.end()) {
            return ParseHexColorVec3(currentColorsHex[role]);
        }
        return glm::vec3(1.0f, 0.0f, 1.0f);
    }

    float ThemeManager::GetStyleVar(StyleVarRole role) {
        if (currentStyleVars.find(role) != currentStyleVars.end()) {
            return currentStyleVars[role];
        }
        return 0.0f;
    }

    PinShapeType ThemeManager::GetPinShape() { return currentPinShape; }
    ConnectionStyleType ThemeManager::GetConnectionStyle() { return currentConnectionStyle; }

    void ThemeManager::ApplyGlobalStyle() {
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = GetStyleVar(StyleVarRole::MenuRounding);
        style.Colors[ImGuiCol_WindowBg] = ImGui::ColorConvertU32ToFloat4(GetColorU32(ColorRole::MenuBackground));
        style.Colors[ImGuiCol_Border] = ImGui::ColorConvertU32ToFloat4(GetColorU32(ColorRole::MenuBorder));
    }


    void ThemeManager::LoadFonts(ImGuiIO& io, float dpiScale) {
        const ImWchar* cyrillicRanges = io.Fonts->GetGlyphRangesCyrillic();

        float baseRegularSize = 18.0f;
        float baseBoldSize = 22.0f;

        fontRegular = io.Fonts->AddFontFromFileTTF("resources/fonts/ttf/JetBrainsMonoNL-Regular.ttf", baseRegularSize * dpiScale, nullptr, cyrillicRanges);
        fontBold = io.Fonts->AddFontFromFileTTF("resources/fonts/ttf/JetBrainsMonoNL-Bold.ttf", baseBoldSize * dpiScale, nullptr, cyrillicRanges);
        
        ImGui::GetStyle().ScaleAllSizes(dpiScale);
    }
}