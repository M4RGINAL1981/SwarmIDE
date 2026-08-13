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

    /**
     * @class ThemeManager
     * @brief Класс, для загрузки стилей/тем приложения
     */
    class ThemeManager {
    public:
        /**
         * @brief Точка доступа к единственному экзэмпляру класса.
         * @return ThemeManager& 
         */
        static ThemeManager& getInstance();
        
        void SwitchTheme(const std::string& ThemeName);

        // Получение цветов
        ImU32 GetColorU32(ColorRole role);

        /**
         * @brief Get the Color Vec 3 object
         * 
         * @param role 
         * @return glm::vec3 
         */
        glm::vec3 GetColorVec3(ColorRole role); 

        /**
         * @brief Получить числовые параметры темы
         * @param role   Какой числовой параметр нужно вернуть
         * @return float Значение числового параметра
         */
        float GetStyleVar(StyleVarRole role);

        // Получение специфичных стилей

        /**
         * @brief Получить стиль пинов
         * @return PinShapeType 
         */
        PinShapeType GetPinShape();
        /**
         * @brief Получить стиль отрисовки соединений
         * @return ConnectionStyleType 
         */
        ConnectionStyleType GetConnectionStyle();

        /**
         * @brief Применить глобальные настройки темы для ImGui
         */
        void ApplyGlobalStyle();

        /**
         * @brief Применить глобальные шрифты темы для ImGui
         * @param[in] io 
         * @param[in] dpiScale
         */
        void LoadFonts(ImGuiIO& io, float dpiScale);

        /**
         * @brief Получить обычный шрифт
         * @return ImFont* обычный шрифт
         */
        ImFont* GetFontRegular() const { return fontRegular; }

        /**
         * @brief Получить жирный шрифт
         * @return ImFont* жирный шрифт
         */
        ImFont* GetFontBold() const { return fontBold; }

        /**
         * @brief Получить название всех тем.
         * @return const std::vector<std::string>& 
         */
        const std::vector<std::string>& GetAllThemeName() { return availableThemes; }

        /**
         * @brief Вернуть текущее название темы.
         * @return const st::string& 
         */
        const std::string& GetCurrentThemeName() { return currentThemeName; }

        // Запрещаем копирование и перемещение менеджера стилей/тем
        ThemeManager(const ThemeManager&) = delete;
        ThemeManager& operator=(const ThemeManager&) = delete;
        ThemeManager(ThemeManager&&) = delete;
        ThemeManager& operator=(ThemeManager&&) = delete;
    private:
        ThemeManager();
        ~ThemeManager() { Core::LoggingManager::Info("[THEME_MANAGER]: Destructor"); }

        ImU32 ParseHexColor(const std::string& hexStr);                                /** @brief < Вспомогательная функция для парсинга HEX в ImU32 */
        glm::vec3 ParseHexColorVec3(const std::string& hexStr);                        /** @brief < Вспомогательная функция для парсинга HEX в glm::vec3 */

        std::string themesPath = "resources/themes/Themes.json";                       /** @brief < Путь до json файла с темами. */
        std::string currentThemeName;                                                  /** @brief < Название текущий темы. */
        
        std::unordered_map<ColorRole, std::string> currentColorsHex;                   /** @brief < Текущие цвета темы. */
        std::unordered_map<StyleVarRole, float> currentStyleVars;                      /** @brief < Текущие числове параметры темы. */
        PinShapeType currentPinShape = PinShapeType::Circle;                           /** @brief < Текущая форма пинов темы. */
        ConnectionStyleType currentConnectionStyle = ConnectionStyleType::Solid;       /** @brief < Текущий стиль отрисовки соединений. */

        ImFont* fontRegular;                                                           /** @brief < Основной UI шрифт */
        ImFont* fontBold;                                                              /** @brief < Шрифт для заголовков / Нотификаций */

        std::vector<std::string> availableThemes;                                      /** @brief < Все строковые наименования тем. */
    };
}