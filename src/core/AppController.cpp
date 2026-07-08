#include "core/AppController.hpp"
#include "ui/Menu.hpp"
#include "ui/ConnectionDrawer.hpp"
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "ui/Theme.hpp"
#include "graphics/Colors.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

unsigned int SCR_WIDTH = 3840;
unsigned int SCR_HEIGHT = 2160;

ImVec2 winSize((float)SCR_WIDTH, (float)SCR_HEIGHT);
glm::vec2 winSize2((float)SCR_WIDTH, (float)SCR_HEIGHT);

namespace SwarmIDE::Core {
    
    AppController::AppController() : window(nullptr), camera(), grid(), object_manager(ObjectManager::getInstance()), connect_manager(ConnectionManager::getInstance()), serialization_manager(SerializationManager::getInstance()), model_manager(ModelManager::getInstance()) {

        // 1. GLFW и создание окна
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return;
        }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        this->window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL and ImGui", NULL, NULL);
        if (window == NULL) {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return;
        }
        glfwMakeContextCurrent(window);

        // 2. СНАЧАЛА СТАВИМ НАШИ КОЛБЭКИ ВВОДА
        glfwSetWindowUserPointer(window, this);

        glfwSetMouseButtonCallback(window, [](GLFWwindow* w, int button, int action, int mods) {
            ImGuiIO& io = ImGui::GetIO();
            if (io.WantCaptureMouse) return; // Если клик по меню - выходим

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
                app->camera.pan(currentPos - app->lastMousePos);
            }
            app->lastMousePos = currentPos;
        });
        glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
            auto* app = static_cast<AppController*>(glfwGetWindowUserPointer(window));
            
            if(mods & GLFW_MOD_CONTROL){
                if(key == GLFW_KEY_EQUAL && action == GLFW_PRESS){
                    app->camera.zoom(1);
                }
                if(key == GLFW_KEY_MINUS && action == GLFW_PRESS){
                    app->camera.zoom(-1);
                }
            }
        });
        glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
            auto* app = static_cast<AppController*>(glfwGetWindowUserPointer(window));

            // Проверяем, зажат ли Ctrl (левый или правый)
            if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || 
                glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS) {
                if (yoffset > 0.0) { app->camera.zoom(1); } 
                else if (yoffset < 0.0) { app->camera.zoom(-1); } } 
        });
        glfwSetFramebufferSizeCallback(window, [](GLFWwindow* w, int width, int height) {
            auto* app = static_cast<AppController*>(glfwGetWindowUserPointer(w));
            glViewport(0, 0, width, height);
            app->grid.SetWindowSize(width, height); 
        });

        // 3. ПОТОМ ИНИЦИАЛИЗИРУЕМ ImGui
        // Теперь ImGui сохранит наши колбэки (благодаря true) и встроит их в свою цепочку
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();

        // io.FontGlobalScale = 3.0f;
        
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        const ImWchar* cyrillicRanges = io.Fonts->GetGlyphRangesCyrillic();

        io.Fonts->AddFontFromFileTTF("resources/fonts/ttf/JetBrainsMonoNL-Regular.ttf", 48.0f, nullptr, cyrillicRanges);

        SwarmIDE::UI::Theme::BuildFontAtlas(fonts.regularFont, io, "resources/fonts/ttf/JetBrainsMono-Regular.ttf", cyrillicRanges);
        SwarmIDE::UI::Theme::BuildFontAtlas(fonts.boldFont,    io, "resources/fonts/ttf/JetBrainsMono-Bold.ttf",    cyrillicRanges);
        SwarmIDE::UI::Theme::BuildFontAtlas(fonts.regularFont, io, "resources/fonts/ttf/JetBrainsMono-Italic.ttf",  cyrillicRanges);

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 450");

        // 4. Инициализация OpenGL ресурсов
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "Failed to initialize GLAD" << std::endl;
            return;
        }
        
        grid.init();
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

        // 5. Связи
        SwarmIDE::UI::Menu::getInstance().SetObjectManager(&object_manager);
        SwarmIDE::UI::Menu::getInstance().SetSerializationManager(&serialization_manager);
        SwarmIDE::UI::Menu::getInstance().SetConnectionManager(&connect_manager);
        SwarmIDE::UI::Menu::getInstance().SetModelManager(&model_manager);

        SwarmIDE::UI::Menu::getInstance().SetFonts(fonts);
        SwarmIDE::Core::SerializationManager::getInstance().SetFonts(fonts);

        SwarmIDE::UI::ConnectionDrawer::getInstance().SetObjectManager(&object_manager);
        SwarmIDE::UI::ConnectionDrawer::getInstance().SetConnectionManager(&connect_manager);
    }

    void AppController::run() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();

            // 1. Расчет времени и камеры (делаем в самом начале шага)
            float currentFrame = (float)glfwGetTime();
            static float lastFrame = 0.0f;
            float deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;
            
            //camera.update(deltaTime);
            object_manager.updateAllObject(); 

            // 2. Подготовка кадров ImGui
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // 3. Отрисовка всей UI-логики

            object_manager.drawUIAllObject(camera, winSize2); 

            SwarmIDE::UI::ConnectionDrawer::DrawLink(camera, winSize2); 
            SwarmIDE::UI::ConnectionDrawer::DrawPins(camera, winSize2); 

            SwarmIDE::UI::Menu::getInstance().DrawMenu(camera); 
            SwarmIDE::UI::Menu::getInstance().DrawTopBar(window, camera);

            ImGui::Render();

            // 4. Отрисовка графической сцены (Очистка экрана и сетка)
            glClearColor(Colors::BACKGROUND.r, Colors::BACKGROUND.g, Colors::BACKGROUND.b, 1.0F);
            glClear(GL_COLOR_BUFFER_BIT);

            grid.draw(camera);

            // 5. Вывод ImGui поверх OpenGL сцены
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            
            glfwSwapBuffers(window);
        }

        std::cout << "All created Agents: ";
        std::cout << object_manager.getCountObject() << std::endl;
    }
}