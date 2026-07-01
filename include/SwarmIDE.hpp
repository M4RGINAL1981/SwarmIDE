#pragma once

namespace SwarmIDE {

    namespace Object {
        class BaseObject;

            // Агенты
        namespace AI {      
            class BaseAgent;           // Базовый класс агентов
            class ConversationAgent;   //
            class ToolsAgent;          //
            class PlanExecuteAgent;    // 
        }

        namespace Tool {
            class BaseTool;
        }

        namespace Memory {
            class BaseMemory;
        }

        namespace Model {
            class BaseModel;
        }

    }

    
    // Отрисовка окон и меню через ImGui, а также связи через OpenGl
    namespace UI {
        class Menu;                // Меню, кнопки, создание связей, объектов, настройки и т.д.
        class Theme;               // Темы, окон. Закругление, цвета и т.д.
        class Topology;            // Описывает фундаментальные структуры для связей холста.
        class ConnectionDrawer;    // Рисует связи между агентами (Кривые Безье)

        namespace Shape {
            class BaseShape;
            class SquareShape;
            class RectangleShape;
            class CircleShape;
            class TriangleShape;
        }
    }   

    // Cuda, вызов всей логики агентов и иницилизация приложения.
    namespace Core {
        class AppController;        // Сердце приложения. Иницилизирует все библиотеки, создаёт объекты и т.д
        class ObjectManager;        // Хранит все объекты, запускает обход по ним.
        class ConnectionManager;    // Хранит все связи между объектам
        class TextureManger;        // Хранит текстуры(иконки) для объектов
    } 

    // Подключение стороних приложений Telegram, Gmail и т.д.
    namespace OI {
        class BaseApp;             // Класс базового прилжения
        class Telegram;            // Телеграмм
        class GMail;               // Почта gmail.com
        class Mail;                // Почта mail.ru
        class VK;                  // Вк
        class Word;                // Word документ
        class Excel;               // Excel документ
        class PowerPoint;
        class TextDocument;        // Текстовый документ
    }   
}