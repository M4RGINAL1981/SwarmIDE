#include "SwarmIDE.hpp"
#include "ui/AppController.hpp"
#include "ui/windows/ThemeManager.hpp"
#include "ui/command/CmdCreateNode.hpp"

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

unsigned int SCR_WIDTH = 3840;
unsigned int SCR_HEIGHT = 2160;

namespace SwarmIDE::UI {
    
    AppController::AppController() : window(nullptr), canvas_manager(UI::CanvasManager::getInstance()) {

        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return;
        }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        this->window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "SwarmIDE v1.0", NULL, NULL);
        if (window == NULL) {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return;
        }
        glfwMakeContextCurrent(window);

        // --- НАСТРОЙКА КОЛЛБЭКОВ ---
        glfwSetWindowUserPointer(window, this);

        glfwSetMouseButtonCallback(window, [](GLFWwindow* w, int button, int action, int mods) {
            ImGuiIO& io = ImGui::GetIO();
            if (io.WantCaptureMouse) return;

            auto* app = static_cast<AppController*>(glfwGetWindowUserPointer(w));
            if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
                if (action == GLFW_PRESS) app->isPanning = true;
                if (action == GLFW_RELEASE) app->isPanning = false;
            }
        });

        glfwSetCursorPosCallback(window, [](GLFWwindow* w, double xpos, double ypos) {
            auto* app = static_cast<AppController*>(glfwGetWindowUserPointer(w));
            glm::vec2 currentPos = glm::vec2((float)xpos, (float)ypos);
            if (app->isPanning) {
                // Панорамируем камеру через синглтон холста
                UI::CanvasManager::getInstance().GetCamera().pan(currentPos - app->lastMousePos);
            }
            app->lastMousePos = currentPos;
        });

        // Объединенный коллбэк клавиатуры
        glfwSetKeyCallback(window, [](GLFWwindow* w, int key, int scancode, int action, int mods) {
            auto* app = static_cast<AppController*>(glfwGetWindowUserPointer(w));
            auto& camera = UI::CanvasManager::getInstance().GetCamera();

            // Стрелочки для перемещения (работает при зажатии - GLFW_REPEAT)
            if (action == GLFW_PRESS || action == GLFW_REPEAT) {
                if (key == GLFW_KEY_UP)    camera.pan(glm::vec2(0.0f, -10.0f));
                if (key == GLFW_KEY_DOWN)  camera.pan(glm::vec2(0.0f, 10.0f));
                if (key == GLFW_KEY_LEFT)  camera.pan(glm::vec2(10.0f, 0.0f));
                if (key == GLFW_KEY_RIGHT) camera.pan(glm::vec2(-10.0f, 0.0f));
            }
            
            // Масштабирование через Ctrl + / -
            if (mods & GLFW_MOD_CONTROL && action == GLFW_PRESS) {
                if (key == GLFW_KEY_EQUAL) camera.zoom(1);
                if (key == GLFW_KEY_MINUS) camera.zoom(-1);
            }
        });

        glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
            auto* app = static_cast<AppController*>(glfwGetWindowUserPointer(window));
            auto& camera = UI::CanvasManager::getInstance().GetCamera();

            if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || 
                glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS) {
                if (yoffset > 0.0) camera.zoom(1); 
                else if (yoffset < 0.0) camera.zoom(-1); 
            } 
        });

        glfwSetFramebufferSizeCallback(window, [](GLFWwindow* w, int width, int height) {
            glViewport(0, 0, width, height);
            UI::CanvasManager::getInstance().GetGrid().SetWindowSize(width, height); 
        });

        // --- ИНИЦИАЛИЗАЦИЯ ImGui ---
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        const ImWchar* cyrillicRanges = io.Fonts->GetGlyphRangesCyrillic();
        io.Fonts->AddFontFromFileTTF("resources/fonts/ttf/JetBrainsMonoNL-Regular.ttf", 24.0f, nullptr, cyrillicRanges);

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 450");

        // --- ИНИЦИАЛИЗАЦИЯ OpenGL (GLAD) ---
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "Failed to initialize GLAD" << std::endl;
            return;
        }
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    }
    
    void AppController::run() {
        // Создаем тестовые ноды (Имена переменных должны отличаться)
        Command::CmdCreateNode cmd1("TextNode", "Text Input 1");
        cmd1.Execute();
        Command::CmdCreateNode cmd2("ModelNode", "DeepSeek-R1");
        cmd2.Execute();

        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();

            // 1. Расчет дельты времени
            float currentFrame = (float)glfwGetTime();
            static float lastFrame = 0.0f;
            float deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            // Обновляем логику холста (плавный зум камеры)
            canvas_manager.Update(deltaTime);

            // 2. Подготовка кадров ImGui
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // 3. Отрисовка UI-логики (Полноэкранное окно-подложка)
            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0)); // Убираем отступы
            ImGui::Begin("Canvas", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus);
            
            canvas_manager.Draw();
            
            ImGui::End();
            ImGui::PopStyleVar();

            ImGui::Render();

            // 4. Очистка экрана фоновым цветом сетки
            glm::vec3 colorBackground = ThemeManager::getInstance().GetColorVec3(ColorRole::GridBackground);
            glClearColor(colorBackground.r, colorBackground.g, colorBackground.b, 1.0F);
            glClear(GL_COLOR_BUFFER_BIT);

            // 5. Вывод ImGui поверх OpenGL сцены
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            
            glfwSwapBuffers(window);
        }

        // Очистка ресурсов при выходе
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(window);
        glfwTerminate();
    }
}