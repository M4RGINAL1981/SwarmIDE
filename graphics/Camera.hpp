#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <algorithm>

namespace SwarmIDE::Graphics {

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
        
        void zoom(int direction){
            currentZoomFactor = std::clamp(currentZoomFactor + direction, 0, static_cast<int>(screenSize.size() - 1));
            targetZoom = screenSize[currentZoomFactor];
            zoomFactor = targetZoom;
        }

        
        // Плавно догоняем цель каждый кадр
        void update(float deltaTime) {
            float interpolationSpeed = 15.0f; // Скорость доводки зума
            zoomFactor = glm::mix(zoomFactor, targetZoom, std::clamp(deltaTime * interpolationSpeed, 0.0f, 1.0f));
            targetZoom = screenSize[currentZoomFactor];
            zoomFactor = targetZoom;
        }
        

        int GetCurrentZoomIndex() { return currentZoomFactor; }

        void reset() {
            offset = glm::vec2(0.0f, 0.0f);
            targetZoom = 1.0f;
            zoomFactor = 1.0f;
        }

        glm::vec2 offset = glm::vec2(0.0f, 0.0f); 
        float zoomFactor = 1.0f;
        float targetZoom = 1.0f; // Целевой зум для плавности
        int currentZoomFactor = 1;
        std::vector<float> screenSize = { 0.5f, 1.0f, 1.5f, 2.0f, 2.5f, 3.0f, 
                                        3.5f, 4.0f, 4.5f, 5.0f, 5.5f, 6.0f,
                                        6.5f, 7.0f, 7.5f, 8.0f, 8.5f, 9.0f };
    };
}