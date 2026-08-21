#include "SwarmIDE.hpp"
#include "ui/AppController.hpp"
#include "ui/windows/ThemeManager.hpp"
#include "ui/command/CommandBus.hpp"
#include "ui/command/ShortcutManager.hpp"
#include "ui/windows/WindowEventHandler.hpp"

#include "ui/windows/Menu.hpp"
#include "ui/windows/TopBar.hpp"
#include "ui/windows/NotificationMenu.hpp"
#include "ui/canvas/ContextMenuManager.hpp"

#include "stb_image.h"

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
 
unsigned int SCR_WIDTH = 1920;
unsigned int SCR_HEIGHT = 1080;

glm::vec2 windowSize = glm::vec2((float)SCR_WIDTH, (float)SCR_HEIGHT);

namespace SwarmIDE::UI {
    
    AppController::AppController() : window(nullptr), canvas_manager(CanvasManager::getInstance()), 
    canvas_input_controller(CanvasInputController::getInstance()), context_menu_manager(ContextMenuManager::getInstance()) {

        Core::LoggingManager::Info("[APP_CONTROLER]: AppController is begin constructor");

        if (!glfwInit()) { Core::LoggingManager::Error("[ERROR]: Failed to initialize GLFW"); return; }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE); // Отключаем стандартную белую рамку Windows

        this->window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "SwarmIDE v0.92", NULL, NULL);
        if (window == NULL) {
            Core::LoggingManager::Error("[ERROR]: Failed to create GLFW window");
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }

        GLFWimage window_icon;
        window_icon.pixels = stbi_load("SwarmIDE_icon_32_x_32.png", &window_icon.width, &window_icon.height, 0, 4);

        if (window_icon.pixels != nullptr) {
            glfwSetWindowIcon(window, 1, &window_icon);
            
            stbi_image_free(window_icon.pixels);
        } else {
            Core::LoggingManager::Warning("Failed to load window SwarmIDE_icon_32_x_32.png");
        }

        GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
        if (primaryMonitor) {
            const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
            int winWidth, winHeight;
            glfwGetWindowSize(window, &winWidth, &winHeight);
            
            int posX = (mode->width - winWidth) / 2;
            int posY = (mode->height - winHeight) / 2;
            
            glfwSetWindowPos(window, posX, posY);
        }

        glfwMakeContextCurrent(window);
        glfwSetWindowUserPointer(window, this);

        // --- КОЛЛБЭК НАЖАТИЯ КНОПОК МЫШИ ---
        glfwSetWindowUserPointer(window, this);
        glfwSetMouseButtonCallback(window, WindowEventHandler::MouseButtonCallback);
        glfwSetCursorPosCallback(window, WindowEventHandler::CursorPosCallback);
        glfwSetKeyCallback(window, WindowEventHandler::KeyCallback);
        glfwSetScrollCallback(window, WindowEventHandler::ScrollCallback);
        glfwSetFramebufferSizeCallback(window, WindowEventHandler::FramebufferSizeCallback);

        // --- ИНИЦИАЛИЗАЦИЯ IMGUI, DPI И ШРИФТОВ ---
        IMGUI_CHECKVERSION();ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        
        float xscale, yscale;glfwGetWindowContentScale(window, &xscale, &yscale);
        float dpiScale = (xscale > yscale) ? xscale : yscale;
        ThemeManager::getInstance().LoadFonts(io, dpiScale);

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 450");
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { Core::LoggingManager::Error("[ERROR]: Failed to initialize GLAD"); return; }
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
            
        canvas_manager.grid.init();
        canvas_input_controller.SetCamera(&canvas_manager.GetCamera());
        canvas_input_controller.SetCanvasManager(&canvas_manager);

        Core::LoggingManager::Info("[APP_CONTROLER]: AppController is end constructor");
    }

    
    void AppController::run() {

        auto& command_bus = Command::CommandBus::getInstance();                 // Иницилизация Менеджера комманд.
        context_menu_manager.SetCommandBus(&command_bus);                       // Установка связи с Менеджером комманд.

        auto& camera = UI::CanvasManager::getInstance().GetCamera();            // Иницилизация камеры.
        auto& grid = UI::CanvasManager::getInstance().GetGrid();                // Иницилизация сетки.

        auto& menu = Menu::getInstance();                                       // Иницилизация Меню создания нод.
        auto& top_bar = TopBar::getInstance();                                  // Иницилизация Верхней Меню-строки.
        auto& notification_menu = NotificationMenu::getInstance();              // Иницилизация Меню уведомлений.

        Core::LoggingManager::Info("[APP_CONTROLER]: GameLoop is begin");
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();

            float currentFrame = (float)glfwGetTime();
            static float lastFrame = 0.0f;
            float deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;
            
            canvas_manager.Update(deltaTime);                                   // Обновление камеры
            canvas_input_controller.ProcessInput();                             // Отлавливание взаимодействия с нодами
            Command::ShortcutManager::Process(window);                          // Отлавливание горячих клавиш

            // Очищаем экран ДО отрисовки чего-либо
            glm::vec3 colorBackground = ThemeManager::getInstance().GetColorVec3(ColorRole::GridBackground);
            glClearColor(colorBackground.r, colorBackground.g, colorBackground.b, 1.0F);
            glClear(GL_COLOR_BUFFER_BIT);

            // Получаем актуальные размеры окна каждый кадр
            int currentWidth, currentHeight;
            glfwGetWindowSize(window, &currentWidth, &currentHeight);
            glm::vec2 dynamicWindowSize((float)currentWidth, (float)currentHeight);

            canvas_manager.SetWindowSize(currentWidth, currentHeight);

            if (drag_window.needsResize) {
                glfwSetWindowSize(window, drag_window.targetWidth, drag_window.targetHeight);
                glfwSetWindowPos(window, drag_window.targetX, drag_window.targetY);
                drag_window.needsResize = false;
            }

            // Готовим кадр ImGui
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
            ImGui::Begin("Canvas", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus);

            menu.Draw(&camera);
            top_bar.Draw(window, &camera, &grid);
            notification_menu.Draw(deltaTime);
            ContextMenuManager::getInstance().Draw();

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
