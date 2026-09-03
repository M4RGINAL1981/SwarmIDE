#include "SwarmIDE.hpp"
#include "ui/windows/WindowEventHandler.hpp"
#include "ui/AppController.hpp"
#include "ui/canvas/CanvasManager.hpp"
#include "imgui.h"

namespace SwarmIDE::UI {

    void WindowEventHandler::MouseButtonCallback(GLFWwindow* w, int button, int action, int mods) {
        ImGuiIO& io = ImGui::GetIO();
        auto* app = static_cast<AppController*>(glfwGetWindowUserPointer(w));
        
        if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
            if (action == GLFW_PRESS) app->isPanning = true;
            if (action == GLFW_RELEASE) app->isPanning = false;
            return;
        }

        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            if (action == GLFW_PRESS) {
                double mx, my;
                glfwGetCursorPos(w, &mx, &my);

                int winWidth, winHeight;
                glfwGetWindowSize(w, &winWidth, &winHeight);

                const float baseHeight = 2160.0f; 
                float scale = (float)winHeight / baseHeight;
                if (scale < 0.4f) scale = 0.4f;   
                float dynamicTopBarHeight = 75.0f * scale;

                glfwGetWindowPos(w, &app->drag_window.windowStartX, &app->drag_window.windowStartY);
                app->drag_window.windowStartWidth = winWidth;
                app->drag_window.windowStartHeight = winHeight;
                app->drag_window.dragStartX = app->drag_window.windowStartX + mx;
                app->drag_window.dragStartY = app->drag_window.windowStartY + my;

                if (app->drag_window.resizeEdgeFlags != AppController::ResizeFlags::NONE) {
                    app->drag_window.isResizingWindow = true;
                } else if (my >= 0 && my <= dynamicTopBarHeight) {
                    if (!io.WantCaptureMouse || (io.WantCaptureMouse && !ImGui::IsAnyItemHovered())) {
                        app->drag_window.isDraggingWindow = true;
                    }
                }
            } else if (action == GLFW_RELEASE) {
                app->drag_window.Reset();
            }
        }
    }

    void WindowEventHandler::CursorPosCallback(GLFWwindow* w, double xpos, double ypos) {
        auto* app = static_cast<AppController*>(glfwGetWindowUserPointer(w));

        // Восстанавливаем логику панорамирования (Middle Mouse)
        if (app->isPanning) {
            double dx = xpos - app->lastMousePos.x;
            double dy = ypos - app->lastMousePos.y;
            CanvasManager::getInstance().GetCamera().pan(glm::vec2(dx, dy));
        }
        app->lastMousePos = glm::vec2(xpos, ypos);

        int width, height, winX, winY;
        glfwGetWindowSize(w, &width, &height);
        glfwGetWindowPos(w, &winX, &winY);

        double currentScrX = winX + xpos;
        double currentScrY = winY + ypos;

        // Изменение размера окна
        if (app->drag_window.isResizingWindow) {
            double dx = currentScrX - app->drag_window.dragStartX;
            double dy = currentScrY - app->drag_window.dragStartY;

            int newWidth = app->drag_window.windowStartWidth;
            int newHeight = app->drag_window.windowStartHeight;
            int newX = app->drag_window.windowStartX;
            int newY = app->drag_window.windowStartY;

            if (app->drag_window.resizeEdgeFlags & AppController::ResizeFlags::RIGHT)  newWidth += (int)dx;
            if (app->drag_window.resizeEdgeFlags & AppController::ResizeFlags::BOTTOM) newHeight += (int)dy;
            if (app->drag_window.resizeEdgeFlags & AppController::ResizeFlags::LEFT)   { newWidth -= (int)dx; newX += (int)dx; }
            if (app->drag_window.resizeEdgeFlags & AppController::ResizeFlags::TOP)    { newHeight -= (int)dy; newY += (int)dy; }

            const int MIN_WIDTH = 800;
            const int MIN_HEIGHT = 600;

            if (newWidth < MIN_WIDTH) {
                if (app->drag_window.resizeEdgeFlags & AppController::ResizeFlags::LEFT)
                    newX = app->drag_window.windowStartX + (app->drag_window.windowStartWidth - MIN_WIDTH);
                newWidth = MIN_WIDTH;
            }
            if (newHeight < MIN_HEIGHT) {
                if (app->drag_window.resizeEdgeFlags & AppController::ResizeFlags::TOP)
                    newY = app->drag_window.windowStartY + (app->drag_window.windowStartHeight - MIN_HEIGHT);
                newHeight = MIN_HEIGHT;
            }

            app->drag_window.targetWidth = newWidth;
            app->drag_window.targetHeight = newHeight;
            app->drag_window.targetX = newX;
            app->drag_window.targetY = newY;
            app->drag_window.needsResize = true;
            return;
        }

        // Плавное перетаскивание окна
        if (app->drag_window.isDraggingWindow) {
            double dx = currentScrX - app->drag_window.dragStartX;
            double dy = currentScrY - app->drag_window.dragStartY;
            glfwSetWindowPos(w, app->drag_window.windowStartX + (int)dx, app->drag_window.windowStartY + (int)dy);
            return;
        }

        ImGuiIO& io = ImGui::GetIO();
        app->drag_window.resizeEdgeFlags = AppController::ResizeFlags::NONE;
        const int border = 8; 

        if (xpos <= border)          app->drag_window.resizeEdgeFlags |= AppController::ResizeFlags::LEFT;
        if (xpos >= width - border)  app->drag_window.resizeEdgeFlags |= AppController::ResizeFlags::RIGHT;
        if (ypos <= border)          app->drag_window.resizeEdgeFlags |= AppController::ResizeFlags::TOP;
        if (ypos >= height - border) app->drag_window.resizeEdgeFlags |= AppController::ResizeFlags::BOTTOM;

        GLFWcursor* cursor = nullptr;
        uint8_t flags = app->drag_window.resizeEdgeFlags;

        // ИСПРАВЛЕНО: Теперь берем готовые сохраненные курсоры
        if ((flags & AppController::ResizeFlags::LEFT && flags & AppController::ResizeFlags::TOP) || 
            (flags & AppController::ResizeFlags::RIGHT && flags & AppController::ResizeFlags::BOTTOM)) {
            cursor = app->drag_window.cursorNWSE;
        }
        else if ((flags & AppController::ResizeFlags::LEFT && flags & AppController::ResizeFlags::BOTTOM) || 
                (flags & AppController::ResizeFlags::RIGHT && flags & AppController::ResizeFlags::TOP)) {
            cursor = app->drag_window.cursorNESW;
        }
        else if (flags & AppController::ResizeFlags::LEFT || flags & AppController::ResizeFlags::RIGHT) {
            cursor = app->drag_window.cursorHResize;
        }
        else if (flags & AppController::ResizeFlags::TOP || flags & AppController::ResizeFlags::BOTTOM) {
            cursor = app->drag_window.cursorVResize;
        }

        if (cursor) {
            glfwSetCursor(w, cursor);
            // Запрещаем ImGui менять курсор, пока мы на границе окна
            io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange; 
        } else {
            // Разрешаем ImGui управлять курсором внутри окна
            io.ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange;
        }
    }

    void WindowEventHandler::KeyCallback(GLFWwindow* w, int key, int scancode, int action, int mods) {
        auto& camera = CanvasManager::getInstance().GetCamera();
        if (action == GLFW_PRESS || action == GLFW_REPEAT) {
            if (key == GLFW_KEY_UP)    camera.pan(glm::vec2(0.0f, -10.0f));
            if (key == GLFW_KEY_DOWN)  camera.pan(glm::vec2(0.0f, 10.0f));
            if (key == GLFW_KEY_LEFT)  camera.pan(glm::vec2(10.0f, 0.0f));
            if (key == GLFW_KEY_RIGHT) camera.pan(glm::vec2(-10.0f, 0.0f));
        }
        if (mods & GLFW_MOD_CONTROL && action == GLFW_PRESS) {
            if (key == GLFW_KEY_EQUAL) camera.zoom(1);
            if (key == GLFW_KEY_MINUS) camera.zoom(-1);
        }
    }

    void WindowEventHandler::ScrollCallback(GLFWwindow* w, double xoffset, double yoffset) {
        auto& camera = CanvasManager::getInstance().GetCamera();
        if (glfwGetKey(w, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || 
            glfwGetKey(w, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS) {
            if (yoffset > 0.0) camera.zoom(1); 
            else if (yoffset < 0.0) camera.zoom(-1); 
        } 
    }

    void WindowEventHandler::FramebufferSizeCallback(GLFWwindow* w, int width, int height) {
        glViewport(0, 0, width, height);
        CanvasManager::getInstance().GetGrid().SetWindowSize(width, height); 
        CanvasManager::getInstance().SetWindowSize(width, height);
    }
}