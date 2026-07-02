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
        void SetManager(SwarmIDE::Core::ObjectManager* mgr) { manager = mgr; }              // Объявить менеджера-объектов
        void DrawMenu(Camera& camera);                                                      // Отрисовка меню
        void GetWinSize(glm::vec2 otherWinSize) { winSize = otherWinSize; }                 // Установка размера окна
        void SetFonts(const SwarmIDE::UI::Fonts& fontsAtlas) { this->fonts = fontsAtlas; }  // Установка шрифтов

    private:
        Menu() = default;
        SwarmIDE::Core::ObjectManager* manager = nullptr;
        glm::vec2 winSize;
        SwarmIDE::UI::Fonts fonts;
    };
}

// Boilerplate - это базовое хуйни приложение, которые качаешь с git. Пишишь одну команду и она иницилизирует файловую структуру.