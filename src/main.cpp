#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "graphics/Camera.hpp"
#include "graphics/Grid.hpp"
#include "graphics/Colors.hpp"

Camera camera;

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    float factor = (yoffset > 0) ? 1.1f : 0.9f;
    camera.zoom(factor); // Зуммируем строго цель
}

int main() {
    int windowWidth = 3840;
    int windowHeight = 2160;

    if (!glfwInit()) return 1; 

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "SwarmIDE Core v0.1", NULL, NULL);
    if (!window) { glfwTerminate(); return 1; }
    
    glfwMakeContextCurrent(window);
    glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { 
        glfwTerminate(); 
        return 2; 
    }

    glfwSwapInterval(1); // V-Sync

    Grid grid(Colors::DEFAULT_GRAY, 100.0f);
    grid.SetWindowSize(windowWidth, windowHeight);
    grid.init();

    double mouseX = 0.0f;
    double mouseY = 0.0f;
    double lastMouseX = 0.0f;
    double lastMouseY = 0.0f;

    // Получаем стартовую позицию мыши перед циклом
    glfwGetCursorPos(window, &lastMouseX, &lastMouseY);

    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        // Расчет Delta Time для плавности
        float currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Обновляем плавный зум камеры
        camera.update(deltaTime);

        glfwGetCursorPos(window, &mouseX, &mouseY);

        glm::vec2 mousePos(mouseX, mouseY);
        glm::vec2 lastMousePos(lastMouseX, lastMouseY);

        // Смещение мыши
        glm::vec2 delta = mousePos - lastMousePos; 

        // Перетаскивание работает только при зажатии средней кнопки
        if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
            camera.pan(delta);
        }

        // Обновляем размеры
        int displayWidth, displayHeight;
        glfwGetFramebufferSize(window, &displayWidth, &displayHeight);
        glViewport(0, 0, displayWidth, displayHeight);

        int winWidth, winHeight;
        glfwGetWindowSize(window, &winWidth, &winHeight);
        grid.SetWindowSize(winWidth, winHeight); 

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        grid.draw(camera);
        
        // КРИТИЧЕСКИ ВАЖНО: сохраняем старые координаты мыши в КАНЦЕ кадра
        lastMouseX = mouseX;
        lastMouseY = mouseY;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}