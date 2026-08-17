#include "SwarmIDE.hpp"
#include "ui/command/ShortcutManager.hpp"
#include "ui/canvas/CanvasManager.hpp"
#include "core/graph/GraphManager.hpp"
#include "imgui.h"

#ifdef APIENTRY
#undef APIENTRY
#endif
#include "pfd/portable-file-dialogs.h"

#include <thread>

namespace SwarmIDE::UI::Command {

    void ShortcutManager::Process(GLFWwindow* window) {
        ImGuiIO& io = ImGui::GetIO();

        if(io.WantTextInput) return;

        auto& camera = CanvasManager::getInstance().GetCamera();
        auto& grid = CanvasManager::getInstance().GetGrid();

        /** @brief Alt + F4. Закрытие приложения. */
        if(io.KeyAlt && ImGui::IsKeyPressed(ImGuiKey_F4, false)) glfwSetWindowShouldClose(window, true);

        /** @brief Ctrl + Enter. Запук обхода графа. */
        if(io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Enter, false)) std::thread([]() { Core::GraphManager::getInstance().StartExecutionAll(); }).detach();

        /** @brief Ctrl + R. Сброс положения камеры. */
        if(io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_R, false)) camera.reset();

        /** @brief Ctrl + G. Переключение видимости сетки. */
        if(io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_G, false)) grid.toggleVisibility();

        /** @brief Ctrl + S. Сохранение топологии. */
        if(io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S, false)) {
            std::string nextAvailableName = Core::SerializationManager::getInstance().GetNextAvailableFileName("topology");
            auto destination = pfd::save_file(
                "Сохранить топологию графа",
                nextAvailableName,
                { "Файлы топологии (*.json)", "*.json", "Все файлы (*)", "*" }).result();  
                if (!destination.empty()) Core::SerializationManager::getInstance().SaveGraph(destination);
        }

        /** @brief Ctrl + O. Загрузка топологии. */
        if(io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_O, false)) {
            auto selection = pfd::open_file(
                "Загрузить топологию",
                "",
                { "Файлы топологии (*.json)", "*.json", "Все файлы (*)", "*" },
                pfd::opt::none
            ).result();
                    
            if (!selection.empty()) Core::SerializationManager::getInstance().LoadGraph(selection[0]);
        }

        /** @brief Ctrl + Delete. Очистка топологии. */
        if(io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Delete, false)) Core::SerializationManager::getInstance().ClearGraph(); 
    }
}