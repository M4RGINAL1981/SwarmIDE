#include "SwarmIDE.hpp"
#include "ui/AppController.hpp"
#include "ui/windows/ThemeManager.hpp"
#include "ui/command/CommandBus.hpp"
#include "ui/windows/Menu.hpp"

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

unsigned int SCR_WIDTH = 3840;
unsigned int SCR_HEIGHT = 2160;

namespace {
    void HandleShortcutCommands(SwarmIDE::UI::Command::CommandBus& command_bus) {
        ImGuiIO& io = ImGui::GetIO();

        // Проверяем, что зажат Ctrl (поддерживает и левый, и правый)
        bool ctrlPressed = io.KeyCtrl; 

        // Отлавливаем Ctrl + Z
        if (ctrlPressed && ImGui::IsKeyPressed(ImGuiKey_Z, false)) {
            // Вызываем вашу команду отмены действия
            command_bus.Undo();
        }

        // Отлавливаем Ctrl + Y
        if (ctrlPressed && ImGui::IsKeyPressed(ImGuiKey_Y, false)) {
            command_bus.Redo();
        }
    }
}

namespace SwarmIDE::UI {
    
    AppController::AppController() : window(nullptr), canvas_manager(UI::CanvasManager::getInstance()) {
        Core::LoggingManager::Info("[APP_CONTROLER]: AppController is begin constructor");

        if (!glfwInit()) {
            Core::LoggingManager::Error("[ERROR]: Failed to initialize GLFW");
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return;
        }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        this->window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "SwarmIDE v0.21", NULL, NULL);
        if (window == NULL) {
            
            Core::LoggingManager::Error("[ERROR]: Failed to create GLFW window");
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return;
        }
        glfwMakeContextCurrent(window);

        // --- НАСТРОЙКА КОЛЛБЭКОВ ---
        glfwSetWindowUserPointer(window, this);

        glfwSetMouseButtonCallback(window, [](GLFWwindow* w, int button, int action, int mods) {
            ImGuiIO& io = ImGui::GetIO();
            //if (io.WantCaptureMouse) return;

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
            UI::CanvasManager::getInstance().SetWindowSize(width, height);
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
            
            Core::LoggingManager::Error("[ERROR]: Failed to initialize GLAD");
            std::cerr << "Failed to initialize GLAD" << std::endl;
            return;
        }
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

        canvas_manager.grid.init();

        Core::LoggingManager::Info("[APP_CONTROLER]: AppController is end constructor");
    }
    
    void AppController::run() {

        auto& command_bus = Command::CommandBus::getInstance();
        
        auto cmd1 = std::make_unique<Command::CmdCreateNode>("TextNode", "Text Input 1");
        auto cmd2 = std::make_unique<Command::CmdCreateNode>("ModelNode", "DeepSeek-R1");
        auto cmd3 = std::make_unique<Command::CmdCreateNode>("ConversationalAgent", "Agent1");
        auto cmd4 = std::make_unique<Command::CmdCreateNode>("ConversationalAgent", "Agent2");
        auto cmd5 = std::make_unique<Command::CmdCreateNode>("MergeNode", "Merge");

        command_bus.PushAndExecute(std::move(cmd1));
        command_bus.PushAndExecute(std::move(cmd2));
        command_bus.PushAndExecute(std::move(cmd3));
        command_bus.PushAndExecute(std::move(cmd4));
        command_bus.PushAndExecute(std::move(cmd5));

        auto& camera = UI::CanvasManager::getInstance().GetCamera();
        auto& grid = UI::CanvasManager::getInstance().GetGrid();
        auto& menu = Menu::getInstance();
        menu.SetCamera(&camera);
        menu.SetGrid(&grid);

        Core::LoggingManager::Info("[APP_CONTROLER]: GameLoop is begin");
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();

            float currentFrame = (float)glfwGetTime();
            static float lastFrame = 0.0f;
            float deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;
            
            canvas_manager.Update(deltaTime);
            canvas_manager.HandleCanvasInputs(camera.zoomFactor);

            // Отлавливаем горячие клавиши
            HandleShortcutCommands(command_bus);

            // Очищаем экран ДО отрисовки чего-либо
            glm::vec3 colorBackground = ThemeManager::getInstance().GetColorVec3(ColorRole::GridBackground);
            glClearColor(colorBackground.r, colorBackground.g, colorBackground.b, 1.0F);
            glClear(GL_COLOR_BUFFER_BIT);

            // Готовим кадр ImGui
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
            ImGui::Begin("Canvas", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus);
            
            menu.DrawMenu();
            canvas_manager.Draw();
            
            ImGui::End();
            ImGui::PopStyleVar();

            ImGui::Render();

            // Рендерим сам ImGui поверх нашей OpenGL сцены
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            
            glfwSwapBuffers(window);
        }
        Core::LoggingManager::Info("[APP_CONTROLER]: GameLoop is end");

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(window);
        glfwTerminate();
    }
}