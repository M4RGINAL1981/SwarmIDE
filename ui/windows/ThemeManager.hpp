#pragma once
#include "SwarmIDE.hpp"
#include "ui/windows/ThemeTypes.hpp"
#include "core/engine/LoggingManager.hpp"
#include "imgui/imgui.h"
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <unordered_map>

namespace SwarmIDE::UI {

    class ThemeManager {
    public:
        static ThemeManager& getInstance();
        
        void SwitchTheme(const std::string& ThemeName);

        // Получение цветов
        ImU32 GetColorU32(ColorRole role);
        glm::vec3 GetColorVec3(ColorRole role); // Специально для OpenGL Grid!

        // Получение числовых параметров
        float GetStyleVar(StyleVarRole role);

        // Получение специфичных стилей
        PinShapeType GetPinShape();
        ConnectionStyleType GetConnectionStyle();

        void ApplyGlobalStyle();

        ThemeManager(const ThemeManager&) = delete;
        ThemeManager& operator=(const ThemeManager&) = delete;

    private:
        ThemeManager();
        ~ThemeManager() { Core::LoggingManager::Info("[THEME_MANAGER]: Destructor"); }

        // Вспомогательная функция для парсинга HEX в ImU32
        ImU32 ParseHexColor(const std::string& hexStr);
        // Вспомогательная функция для парсинга HEX в glm::vec3
        glm::vec3 ParseHexColorVec3(const std::string& hexStr);

        std::string themesPath = "resources/themes/Themes.json";
        std::string currentThemeName;
        
        // Словари для хранения загруженной темы
        std::unordered_map<ColorRole, std::string> currentColorsHex;
        std::unordered_map<StyleVarRole, float> currentStyleVars;
        
        PinShapeType currentPinShape = PinShapeType::Circle;
        ConnectionStyleType currentConnectionStyle = ConnectionStyleType::Solid;

        std::vector<std::string> availableThemes;
    };
}