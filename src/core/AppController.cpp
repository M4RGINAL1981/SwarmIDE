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
    
    AppController::AppController() : window(nullptr), camera(), grid(), object_manager(ObjectManager::getInstance()), connect_manager(ConnectionManager::getInstance()) {

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
        SwarmIDE::UI::Menu::getInstance().SetManager(&object_manager);
        SwarmIDE::UI::Menu::getInstance().SetFonts(fonts);

        SwarmIDE::UI::ConnectionDrawer::getInstance().SetObjectManager(&object_manager);
        SwarmIDE::UI::ConnectionDrawer::getInstance().SetConnectionManager(&connect_manager);

    }

    void AppController::run() {

        // Базовое создание объектов и их связей
        std::unique_ptr<SwarmIDE::Object::AI::BaseAgent> newAgent1 = nullptr; 
        std::unique_ptr<SwarmIDE::Object::AI::BaseAgent> newAgent2 = nullptr;

        const glm::vec2 WorldPos1(0.0f, 0.0f);
        const glm::vec2 WorldPos2(200.0f, 350.0f);

        newAgent1 = std::make_unique<SwarmIDE::Object::AI::ConversationalAgent>("New Conversational", WorldPos1);
        newAgent2 = std::make_unique<SwarmIDE::Object::AI::ConversationalAgent>("New Conversational", WorldPos2);

        newAgent1->GetFonts(this->fonts);
        newAgent2->GetFonts(this->fonts);

        object_manager.addObject(std::move(newAgent1));
        object_manager.addObject(std::move(newAgent2));

        int Agent1ID = object_manager.objects[0]->GetID();
        int Agent2ID = object_manager.objects[1]->GetID();

        // Проверяем, сколько пинов создалось внутри фигур
        auto pins1 = object_manager.objects[0]->GetShape()->GetLocalPins();
        auto pins2 = object_manager.objects[1]->GetShape()->GetLocalPins();

        std::cout << "[DEBUG] Agent 1 pins count: " << pins1.size() << std::endl;
        std::cout << "[DEBUG] Agent 2 pins count: " << pins2.size() << std::endl;

        if (pins1.empty() || pins2.size() < 2) {
            std::cerr << "[ERROR] Недостаточно пинов для создания связи! Падение предотвращено." << std::endl;
            // Временно закомментируем коннект, чтобы программа запустилась и открылось окно
            // SwarmIDE::UI::AnchorPin pin1 = pins1[0];
            // SwarmIDE::UI::AnchorPin pin2 = pins2[1];
            // ConnectionManager::getInstance().connect(Agent1ID, Agent2ID, pin1, pin2);
        } 
        else {
            SwarmIDE::UI::AnchorPin pin1 = pins1[0];
            SwarmIDE::UI::AnchorPin pin2 = pins2[1]; // Убедись, что тебе нужен именно второй пин (индекс 1)
            ConnectionManager::getInstance().connect(Agent1ID, Agent2ID, pin1, pin2);
        }



        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();

            // 1. Расчет времени и камеры (делаем в самом начале шага)
            float currentFrame = (float)glfwGetTime();
            static float lastFrame = 0.0f;
            float deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;
            
            //camera.update(deltaTime);
            object_manager.updateAllObject(); // <-- Перенесли сюда! Логика обновляется до закрытия кадра UI

            // 2. Подготовка кадров ImGui
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // 3. Отрисовка всей UI-логики (ВНУТРИ FRAME SCOPE)
            SwarmIDE::UI::Menu::getInstance().DrawMenu(camera); 
            
            object_manager.drawUIAllObject(camera, winSize2); 

            // РИСУЕМ СВЯЗИ ТУТ (они уйдут на задний план под окна):
            SwarmIDE::UI::ConnectionDrawer::DrawLink(camera, winSize2); 
            SwarmIDE::UI::ConnectionDrawer::DrawPins(camera, winSize2); 

            ImGui::Render(); // <--- Теперь закрываем кадр со спокойной душой!

            // 4. Отрисовка графической сцены (Очистка экрана и сетка)
            glClearColor(Colors::BACKGROUND.r, Colors::BACKGROUND.g, Colors::BACKGROUND.b, 1.0F);
            glClear(GL_COLOR_BUFFER_BIT);

            grid.draw(camera);
            // SwarmIDE::UI::ConnectionDrawer::DrawLink(...) <- ОТСЮДА СТРОЧКУ УДАЛИЛИ

            // 5. Вывод ImGui поверх OpenGL сцены
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            
            glfwSwapBuffers(window);
        }

        std::cout << "All created Agents: ";
        std::cout << object_manager.getCountObject() << std::endl;
    }
}