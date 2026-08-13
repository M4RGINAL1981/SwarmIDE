#include "SwarmIDE.hpp"
#include "ui/command/ShortcutManager.hpp"
#include "ui/canvas/CanvasManager.hpp"
#include "core/graph/GraphManager.hpp"
#include "imgui.h"
#include <thread>

namespace SwarmIDE::UI::Command {

    void ShortcutManager::Process(GLFWwindow* window) {
        ImGuiIO& io = ImGui::GetIO();

        if(io.WantCaptureKeyboard) return;

        auto& camera = CanvasManager::getInstance().GetCamera();
        auto& grid = CanvasManager::getInstance().GetGrid();

        // Alt + F4
        if(io.KeyAlt && ImGui::IsKeyPressed(ImGuiKey_F4, false)) glfwSetWindowShouldClose(window, true);

        // Ctrl + Enter
        if(io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Enter, false)) std::thread([]() { Core::GraphManager::getInstance().StartExecutionAll(); }).detach();

        // Ctrl + R
        if(io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_R, false)) camera.reset();

        // Ctrl + G
        if(io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_G, false)) grid.toggleVisibility();
    }
}