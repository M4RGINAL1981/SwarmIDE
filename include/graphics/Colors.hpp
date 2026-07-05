#pragma once
#include <glm/glm.hpp>
#include <imgui.h>

namespace Colors {
    inline constexpr glm::vec3 RED{1.0f, 0.0f, 0.0f};
    inline constexpr glm::vec3 GREEN{0.0f, 1.0f, 0.0f};
    inline constexpr glm::vec3 BLUE{0.0f, 0.0f, 1.0f};
    inline constexpr glm::vec3 WHITE{1.0f, 1.0f, 1.0f};
    inline constexpr glm::vec3 BLACK{0.0f, 0.0f, 0.0f};
    inline constexpr glm::vec3 PURPLE{1.0f, 0.0f, 1.0f};
    inline constexpr glm::vec3 LIGHT_GRAY{0.8f, 0.8f, 0.8f};
    inline constexpr glm::vec3 DARK_GRAY{0.2f, 0.2f, 0.2f};
    inline constexpr glm::vec3 DEFAULT_GRAY{0.5f, 0.5f, 0.5f};
    inline constexpr glm::vec3 BACKGROUND{0.1f, 0.1f, 0.1f}; 
    inline constexpr glm::vec3 GRANITE(0.18f, 0.20f, 0.23f);

    // Функция-конвертер в glm::vec4
    inline glm::vec4 ToImGlmVec4(const glm::vec3& color, float alpha = 1.0f) {
        return glm::vec4(color.x, color.y, color.z, alpha);
    }

    // Функция-конвертер в ImVec4
    inline ImVec4 ToImVec4(const glm::vec3& color, float alpha = 1.0f) {
        return ImVec4(color.x, color.y, color.z, alpha);
    }

    // Функция-конвертер в ImU32 
    inline ImU32 ToImU32(const glm::vec3& color, float alpha = 1.0f) {
        return ImGui::ColorConvertFloat4ToU32(ImVec4(color.x, color.y, color.z, alpha));
    }
}
