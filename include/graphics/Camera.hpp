#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <algorithm>

class Camera {
public:
    // Трансформация координат (центрированная относительно экрана)
    glm::vec2 worldToScreen(glm::vec2 worldPos, glm::vec2 windowSize) { 
        return ((worldPos + offset) * zoomFactor) + (windowSize * 0.5f); 
    }
    glm::vec2 screenToWorld(glm::vec2 screenPos, glm::vec2 windowSize) { 
        return ((screenPos - (windowSize * 0.5f)) / zoomFactor) - offset; 
    }

    // Перетаскивание
    void pan(glm::vec2 delta){ 
        offset += (delta / zoomFactor); 
    }

    // Меняем только ЦЕЛЬ зума
    void zoom(float factor){
        targetZoom = std::clamp(targetZoom * factor, minZoom, maxZoom);
    }

    // Плавно догоняем цель каждый кадр
    void update(float deltaTime) {
        float interpolationSpeed = 15.0f; // Скорость доводки зума
        zoomFactor = glm::mix(zoomFactor, targetZoom, std::clamp(deltaTime * interpolationSpeed, 0.0f, 1.0f));
    }

    glm::vec2 offset = glm::vec2(0.0f, 0.0f); 
    float zoomFactor = 1.0f;
    float targetZoom = 1.0f; // Целевой зум для плавности
    float minZoom = 0.05f;
    float maxZoom = 20.0f;
};