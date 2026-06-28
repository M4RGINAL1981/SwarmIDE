#pragma once

namespace SwarmIDE {

    // Агенты
    namespace AI {    
        class BaseAgent; 
        class ConversationAgent;
        class ToolsAgent;
        class PlanExecuteAgent;
    }

    // Отрисовка окон и меню через ImGui
    namespace UI {
        class Menu;
        class Theme;
    }   

    // Cuda и вызов всей логики агентов
    namespace Core {
        class AgentManager;
        class AppController;
    } 

    // Подключение стороних приложений Telegram, Gmail и т.д.
    namespace OI {
        class BaseApp;
        class Telegram;
        class GMail;
        class Mail;
        class VK;
    }   
}