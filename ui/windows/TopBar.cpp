#include "ui/windows/TopBar.hpp"
#include "ui/windows/ThemeManager.hpp"
#include "core/engine/SerializationManager.hpp"
#include "core/graph/GraphManager.hpp"
#include "imgui.h"

#ifdef APIENTRY
#undef APIENTRY
#endif
#include "pfd/portable-file-dialogs.h"

#include <thread>
#include <atomic>
#include <math.h>
#include <unordered_map>
#include <algorithm>

namespace {
    bool AnimatedWindowButton(const char* id_str, const char* label, ImVec4 targetColor, float width) {
        // Хранение прогресса анимации для каждой кнопки
        static std::unordered_map<ImGuiID, float> animations;
        
        ImGuiID id = ImGui::GetID(id_str);
        float& progress = animations[id];

        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImVec2 size = ImVec2(width, ImGui::GetFrameHeight());

        // Невидимая кнопка для обработки кликов и наведения
        bool pressed = ImGui::InvisibleButton(id_str, size);
        bool hovered = ImGui::IsItemHovered();

        // Линейная интерполяция прогресса анимации (скорость 8.0f)
        float deltaTime = ImGui::GetIO().DeltaTime;
        if (hovered) progress = min(progress + deltaTime * 8.0f, 1.0f);
        else         progress = max(progress - deltaTime * 8.0f, 0.0f);

        // Отрисовка фона при наведении
        if (progress > 0.0f) {
            ImVec4 bgColor = ImGui::GetStyleColorVec4(ImGuiCol_MenuBarBg);
            ImVec4 finalColor(
                bgColor.x + (targetColor.x - bgColor.x) * progress,
                bgColor.y + (targetColor.y - bgColor.y) * progress,
                bgColor.z + (targetColor.z - bgColor.z) * progress,
                targetColor.w * progress // Плавное изменение альфа-канала
            );
            ImGui::GetWindowDrawList()->AddRectFilled(
                pos, ImVec2(pos.x + size.x, pos.y + size.y), 
                ImGui::ColorConvertFloat4ToU32(finalColor)
            );
        }

        // Центрирование и отрисовка текста/иконки
        ImVec2 textSize = ImGui::CalcTextSize(label);
        ImVec2 textPos = ImVec2(pos.x + (size.x - textSize.x) * 0.5f, pos.y + (size.y - textSize.y) * 0.5f);
        ImGui::GetWindowDrawList()->AddText(textPos, ImGui::GetColorU32(ImGuiCol_Text), label);

        return pressed;
    }

    bool SimulationButton(const char* id_str, const char* label, bool isRunning, float width) {
        ImGuiID id = ImGui::GetID(id_str);
        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImVec2 size = ImVec2(width, ImGui::GetFrameHeight());

        bool pressed = ImGui::InvisibleButton(id_str, size);
        bool hovered = ImGui::IsItemHovered();

        ImU32 bgColor;
        if (isRunning) {
            bgColor = ImGui::ColorConvertFloat4ToU32(hovered ? ImVec4(0.9f, 0.1f, 0.1f, 0.8f) : ImVec4(0.1f, 0.8f, 0.1f, 0.8f));
        } else {
            bgColor = ImGui::ColorConvertFloat4ToU32(hovered ? ImVec4(1.0f, 1.0f, 1.0f, 0.2f) : ImGui::GetStyleColorVec4(ImGuiCol_MenuBarBg));
        }

        ImGui::GetWindowDrawList()->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), bgColor);

        ImVec2 textSize = ImGui::CalcTextSize(label);
        ImVec2 textPos = ImVec2(pos.x + (size.x - textSize.x) * 0.5f, pos.y + (size.y - textSize.y) * 0.5f);
        ImGui::GetWindowDrawList()->AddText(textPos, ImGui::GetColorU32(ImGuiCol_Text), label);

        return pressed && !isRunning;
    }
}

namespace SwarmIDE::UI {

    TopBar& TopBar::getInstance() {
        static TopBar instance;
        return instance;
    }

    void TopBar::Draw(GLFWwindow* window, Graphics::Camera* camera, Graphics::Grid* grid) {

        float scale = 1.0f;                                                                             // Текущий маштаб 
        float targetBarHeight = 75.0f * scale;                                                          // Высота верхнего меню
        float baseFontSize = 18.0f;                                                                     // Базовый размер шрифта
        float currentTextHeight = baseFontSize * scale;                                                 // Текущий размер шрифта
        float requiredPaddingY = (targetBarHeight - currentTextHeight) / 2.0f;                          // Обазятельный отступ от края меню до текста
        static bool isMaximized = false;                                                                // Текущее состояние окна

        ImGui::GetFont()->Scale = scale; 
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(ImGui::GetStyle().FramePadding.x, max(0.0f, requiredPaddingY)));

        /** @todo Нужно будет переделать визуальную состовляющую - сейчас калл. */
        if (ImGui::BeginMainMenuBar()) {

            // --- ВКЛАДКА FILE ---
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Save", "Ctrl + S")) { 
                    // Вызов проводника сохранения через pfd
                    auto destination = pfd::save_file(
                        "Сохранить топологию графа",
                        "topology.json",
                        { "Файлы топологии (*.json)", "*.json", "Все файлы (*)", "*" }
                    ).result();
                    
                    if (!destination.empty()) Core::SerializationManager::getInstance().SaveGraph(destination);
                }
                if (ImGui::MenuItem("Open...", "Ctrl + O")) { 
                    // Вызов проводника загрузки через pfd (возвращает std::vector путей)
                    auto selection = pfd::open_file(
                        "Загрузить топологию",
                        "",
                        { "Файлы топологии (*.json)", "*.json", "Все файлы (*)", "*" },
                        pfd::opt::none
                    ).result();
                    
                    if (!selection.empty()) Core::SerializationManager::getInstance().LoadGraph(selection[0]);
                }
                if (ImGui::MenuItem("Clear", "Ctrl + Delete")) Core::SerializationManager::getInstance().ClearGraph(); 
                ImGui::Separator();
                if (ImGui::MenuItem("Exit", "Alt+F4")) glfwSetWindowShouldClose(window, true); 
                ImGui::EndMenu();
            }

            // --- ВКЛАДКА VIEW ---
            if (ImGui::BeginMenu("View")) {
                if (ImGui::MenuItem("Reset Camera", "Ctrl+R")) { if(camera) camera->reset(); }
                if (ImGui::MenuItem("Toggle Grid", "Ctrl+G")) { if(grid) grid->toggleVisibility(); }
                
                ImGui::Separator();
                
                // ВЫБЕГАЮЩИЙ СПИСОК ТЕМ
                if (ImGui::BeginMenu("Themes")) {
                    auto& themeManager = ThemeManager::getInstance();
                    const auto& themes = themeManager.GetAllThemeName();
                    
                    for (const auto& themeName : themes) {
                        bool isSelected = (themeName == themeManager.GetCurrentThemeName());
                        
                        // Если пользователь нажал на тему, переключаем её
                        if (ImGui::MenuItem(themeName.c_str(), nullptr, isSelected)) {
                            themeManager.SwitchTheme(themeName);
                            // Применяем глобальные стили сразу после смены темы
                            themeManager.ApplyGlobalStyle(); 
                        }
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }

            // --- КНОПКА SIMULATION ---
            static std::atomic<bool> isSimulationRunning{false}; // Флаг статуса работы симуляции
            
            ImGui::Dummy(ImVec2(10.0f, 0.0f)); 
            if (SimulationButton("btn_simulation", "Simulation", isSimulationRunning, 150.0f)) {
                isSimulationRunning = true;
                std::thread([]() { 
                    Core::GraphManager::getInstance().StartExecutionAll(); 
                    isSimulationRunning = false; // Сброс статуса после завершения работы
                }).detach();
            }

            // --- КНОПКИ УПРАВЛЕНИЯ ОКНОМ ---

            ImVec4 hoverLight = ImVec4(1.0f, 1.0f, 1.0f, 0.2f);    // Светлый полупрозрачный
            ImVec4 hoverRed   = ImVec4(0.9f, 0.1f, 0.1f, 0.8f);    // Красный
            float btnWidth = 45.0f;
            
            ImGui::SetCursorPosX(ImGui::GetWindowWidth() - (btnWidth * 3.0f));
            // Убираем отступы между элементами для слитных кнопок
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0)); 

            // Кнопка Свернуть
            if (AnimatedWindowButton("btn_min", "—", hoverLight, btnWidth)) glfwIconifyWindow(window);
            
            ImGui::SameLine();
            
            // Кнопка Развернуть/Восстановить
            const char* maxIcon = isMaximized ? "<" : ">";                               /** @todo Нужно после разобраться и заменить значки на  🗗 и 🗖. */
            if (AnimatedWindowButton("btn_max", maxIcon, hoverLight, btnWidth)) {
                if (isMaximized) {
                    glfwRestoreWindow(window); 
                    glfwSetWindowPos(window, lastXPos, lastYPos);
                    glfwSetWindowSize(window, lastWidth, lastHeight);
                } 
                else {
                    // Получаем рабочую область монитора (без учета панели задач)
                    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
                    int xpos, ypos, width, height;
                    glfwGetMonitorWorkarea(monitor, &xpos, &ypos, &width, &height);
                    
                    // Сохраняем Текущее положение окна и его размер
                    glfwGetWindowPos(window, &lastXPos, &lastYPos);
                    glfwGetWindowSize(window, &lastWidth, &lastHeight);

                    // Вручную растягиваем окно на доступную область
                    glfwSetWindowPos(window, xpos, ypos);
                    glfwSetWindowSize(window, width, height - 1);
                }
                isMaximized = !isMaximized;
            }
            
            ImGui::SameLine();
            
            // Кнопка Закрыть
            if (AnimatedWindowButton("btn_close", "✕", hoverRed, btnWidth)) glfwSetWindowShouldClose(window, GLFW_TRUE);

            ImGui::PopStyleVar();
            ImGui::SameLine();

            ImGui::EndMainMenuBar();
        }
        
    
        ImGui::GetFont()->Scale = 1.0f;
        ImGui::PopStyleVar();
    }

}
    /*
    void Menu::DrawDownloadModel() {
       // auto models = model_manager->GetModels();

        // Размеры и позиция окна полосы загрузки
        ImVec2 windowSize = ImVec2(500.0f, 120.0f); // Уменьшил высоту, 200 много для одной полосы
        ImVec2 windowPos  = ImVec2(1000.0f, 800.0f);

        ImGuiWindowFlags flags = 
                    ImGuiWindowFlags_NoResize   |               
                    ImGuiWindowFlags_NoMove     |               
                    ImGuiWindowFlags_NoInputs   |               
                    ImGuiWindowFlags_NoCollapse |                      
                    ImGuiWindowFlags_NoTitleBar |               
                    ImGuiWindowFlags_NoBringToFrontOnFocus |    
                    ImGuiWindowFlags_NoFocusOnAppearing;        

        for (size_t i = 0; i < models.size(); ++i) {
            auto& model = models[i];

            if (model.isDownloading) {
                // Создаем уникальное имя окна для ImGui на основе индекса, чтобы избежать конфликтов ID
                std::string window_id = model.name + "##download_win_" + std::to_string(i);

                ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);
                ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always); 

                if (ImGui::Begin(window_id.c_str(), nullptr, flags)) {
                    // Рассчитываем размер самой полосы: во всю ширину окна минус отступы по 20px по бокам
                    // Высоту полосы сделаем фиксированной 30px

                    ImGui::Text(model.name.c_str());
                    ImVec2 progressBarSize = ImVec2(windowSize.x - 40.0f, 30.0f);
                    
                    // Отрисовываем полосу загрузки
                    DrawProgressBar(model.progress, progressBarSize);  
                }
                ImGui::End();

                // Если окон должно быть несколько, смещаем координату Y для следующего окна вниз
                windowPos.y += windowSize.y + 10.0f; 
            }
        }
    }

    void Menu::DrawProgressBar(float progress, ImVec2 size) {
        // 1. Нормализуем прогресс (ожидаем диапазон от 0.0f до 1.0f)
        if (progress < 0.0f) progress = 0.0f;
        if (progress > 1.0f) progress = 1.0f;

        // 2. Получаем текущую экранную позицию курсора внутри окна ImGui
        // Это гарантирует, что даже если окно сдвинется, рисунок сдвинется вместе с ним
        ImVec2 start_pos = ImGui::GetCursorScreenPos();

        // Смещаем курсор немного вниз и вправо для красоты (внутренний отступ от краев окна)
        start_pos.x += 20.0f;
        start_pos.y += 20.0f;

        // Резервируем место в окне под элемент (с учетом смещения)
        ImGui::Dummy(ImVec2(size.x + 20.0f, size.y + 20.0f));  

        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        // 3. Вычисляем крайние точки рамки полосы
        ImVec2 min_p = start_pos;
        ImVec2 max_p = ImVec2(start_pos.x + size.x, start_pos.y + size.y);

        ImU32 color_white  = IM_COL32(255, 255, 255, 255);
        ImU32 color_black  = IM_COL32(0, 0, 0, 255);
        ImU32 color_purple = IM_COL32(140, 20, 250, 255);

        // 4. Рисуем белый фон полосы
        draw_list->AddRectFilled(min_p, max_p, color_white);
        
        // 5. Рисуем фиолетовое заполнение (с учетом внутренних отступов)
        if (progress > 0.0f) {
            float padding = 3.0f; // Внутренний отступ фиолетовой полосы от черной рамки

            // Левый верхний угол заполнения
            ImVec2 fill_min = ImVec2(min_p.x + padding, min_p.y + padding);
            
            // Максимально доступная ширина для заполнения
            float max_fill_width = size.x - (padding * 2.0f);
            // Текущая ширина в зависимости от прогресса
            float current_fill_width = max_fill_width * progress;

            // Правый нижний угол заполнения
            ImVec2 fill_max = ImVec2(fill_min.x + current_fill_width, max_p.y - padding);

            // Рисуем фиолетовый прямоугольник только если ширина больше нуля
            if (current_fill_width > 0.0f) {
                draw_list->AddRectFilled(fill_min, fill_max, color_purple);
            }
        }

        // 6. Рисуем черную окантовку поверх всего (0.0f — без скругления углов)
        draw_list->AddRect(min_p, max_p, color_black, 0.0f, 0, 1.5f);
    }

    */