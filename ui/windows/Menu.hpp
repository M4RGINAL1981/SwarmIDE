#pragma once
#include "SwarmIDE.hpp"
#include "core/graph/GraphManager.hpp"
#include "core/graph/ConnectionManager.hpp"
#include "core/engine/SerializationManager.hpp"
#include "core/models/ModelManager.hpp"
#include "nodes/base/LogicNode.hpp"
#include "graphics/Camera.hpp"
#include "graphics/Grid.hpp"

namespace SwarmIDE::UI {
    class Menu {
    public:
        
        static Menu& getInstance(); 

        void Push(std::unique_ptr<Nodes::LogicNode> object);                    // Передать объект менеджеру объектов

        // Объевление менеджеров
        void SetGraphManager(Core::GraphManager* grp_mgr) { graph_manager = grp_mgr; }              
        void SetSerializationManager(Core::SerializationManager* ser_mgr) { serialization_manager = ser_mgr; }
        void SetConnectionManager(Core::ConnectionManager* conn_mgr) { connect_manager = conn_mgr; }
        void SetModelManager(Core::ModelManager* mdl_mgr) { model_manager = mdl_mgr; }

        void SetGrid(Graphics::Grid* grid) { this->grid = grid; }                                     // Установка сетки
        void SetCamera(Graphics::Camera* camera) { this->camera = camera; }                           // Установки камеры

        void DrawMenu(Graphics::Camera& camera);                                                      // Отрисовка меню
        void DrawTopBar(GLFWwindow* window, Graphics::Camera& camera);                                // Отрисовка функциональных клавиш
        void DrawDownloadModel();                                                           // Отрисовка загрузки моделей
        void DrawProgressBar(float progress, ImVec2 size);                                  // Отрисовка полосы загрузки

        void GetWinSize(glm::vec2 otherWinSize) { winSize = otherWinSize; }                 // Установка размера окна
        //void SetFonts(const SwarmIDE::UI::Fonts& fontsAtlas) { this->fonts = fontsAtlas; }  // Установка шрифтов


        Menu (const Menu&) = delete;
        Menu& operator=(const Menu&) = delete; 
        Menu (Menu&&) = delete;
        Menu& operator=(Menu&&) = delete;

    private:
        Menu() = default;
        ~Menu() = default;

        Core::GraphManager* graph_manager = nullptr;
        Core::SerializationManager* serialization_manager = nullptr;
        Core::ConnectionManager* connect_manager = nullptr;
        Core::ModelManager* model_manager = nullptr;

        Graphics::Grid* grid = nullptr;
        Graphics::Camera* camera = nullptr;

        glm::vec2 winSize;
        //SwarmIDE::UI::Fonts fonts;
    };
}

// Boilerplate - это базовое хуйни приложение, которые качаешь с git. Пишишь одну команду и она иницилизирует файловую структуру.