#pragma once
#include "SwarmIDE.hpp"
#include "core/ObjectManager.hpp" 
#include "object/ai/ConversationalAgent.hpp"
#include "object/ai/PlanExecuteAgent.hpp"
#include "object/ai/ToolsAgent.hpp"
#include "graphics/Camera.hpp"

namespace SwarmIDE::UI {
    class Menu {
    public:
        
        static Menu& getInstance(); 

        void Push(std::unique_ptr<SwarmIDE::Object::BaseObject> object);                    // Передать объект менеджеру объектов

        // Объевление менеджеров
        void SetObjectManager(SwarmIDE::Core::ObjectManager* obj_mgr) { object_manager = obj_mgr; }              
        void SetSerializationManager(SwarmIDE::Core::SerializationManager* ser_mgr) { serialization_manager = ser_mgr; }

        void DrawMenu(Camera& camera);                                                      // Отрисовка меню
        void DrawTopBar(GLFWwindow* window, Camera& camera);                                // Отрисовка функциональных клавиш

        void GetWinSize(glm::vec2 otherWinSize) { winSize = otherWinSize; }                 // Установка размера окна
        void SetFonts(const SwarmIDE::UI::Fonts& fontsAtlas) { this->fonts = fontsAtlas; }  // Установка шрифтов

    private:
        Menu() = default;

        SwarmIDE::Core::ObjectManager* object_manager = nullptr;
        SwarmIDE::Core::SerializationManager* serialization_manager = nullptr;

        glm::vec2 winSize;
        SwarmIDE::UI::Fonts fonts;
    };
}

// Boilerplate - это базовое хуйни приложение, которые качаешь с git. Пишишь одну команду и она иницилизирует файловую структуру.