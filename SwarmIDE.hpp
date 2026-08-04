#pragma once

/**
 * @mainpage SwarmIDE — Среда оркестрации мультиагентных систем
 *
 * @section intro_sec Введение
 * SwarmIDE — это высокопроизводительная C++ среда для визуального проектирования 
 * и оркестрации мультиагентных систем. Проект является нативным, быстрым аналогом n8n,
 * написанным с использованием ImGui и OpenGL для максимальной плавности интерфейса.
 *
 * @section arch_sec Архитектура проекта
 * Кодовая база разделена на 4 изолированных слоя:
 * 
 * - **Core   (Вычислительное ядро)**:   @ref SwarmIDE::Core  — управление графом, сохранение, логика. Без UI.
 * - **Nodes           (Узлы графа)**:   @ref SwarmIDE::Nodes — логические блоки, агенты (Jira, Telegram, LLM).
 * - **UI & Graphics (Визуализация)**:   @ref SwarmIDE::UI    — отрисовка холста через ImGui/OpenGL.
 * - **OI      (Внешние интерфейсы)**:   @ref SwarmIDE::OI    — коннекторы к API (VK, Mail, Excel).
 *
 * @section install_sec Быстрый старт с CMake
 * 
 * Для сборки проекта выполните:
 * @code
 * mkdir build && cd build
 * cmake ..
 * cmake --build .
 * @endcode
 */
namespace SwarmIDE {

    // =========================================================================
    // 1. ВЫЧИСЛИТЕЛЬНОЕ ЯДРО 
    // Менеджеры, абстракции графа, сохранения, генерация ID. Никакого UI.
    // =========================================================================

    /**
     * @namespace Core
     * @brief В данном пространстве имён находятся вычислительное ядро.
     */
    namespace Core {

        // Управление графом
        class GraphManager;            
        class ConnectionManager;       
        class GraphRegistry;           
        class DataVariant;          
        struct LogicPin;           

        // Системы управления моделями

        class ModelManager;
        struct ModelAsset;
        
        // Системные менеджеры
        class LoggingManager;             
        class SerializationManager;      
        class GenerateIDManager;           
    }

    // =========================================================================
    // 2. УЗЛЫ ГРАФА
    // Вся логика, которая ставится на холст. Все наследники LogicNode.
    // =========================================================================

    /**
     * @namespace Nodes
     * @brief В данном пространстве имён находятся вся логика, которая ставиться на холст.
     */
    namespace Nodes {

        // Базовые узлы
        class LogicNode;  
        
        // Системные узлы (Специальные)
        class TextNode;                
        class MergeNode;               
        class ModelNode;               

        // ИИ-Агенты 
        class BaseAgent;               
        class ConversationalAgent;     
        class ToolsAgent;              
        class PlanExecuteAgent;        

        // Инструменты 
        class BaseTool;                
        class JiraSoftware;            

        // Память 
        class BaseMemory;              
        class PostgreSQL;
    }

    // =========================================================================
    // 3. ИНТЕРФЕЙС И ГРАФИКА 
    // Всё, что отвечает за отрисовку (ImGui + OpenGL).
    // =========================================================================

    /**
     * @namespace UI
     * @brief В данном пространстве имён находятся классы использующие ImGui.
     */
    namespace UI {

        // Менеджер отрисовки нод и подключений                    
        class AppController;                             /** @brief < Специальный класс(Синглтрон), который иницилизирует все другие класс, и владет GameLoop програмы. */
        class CanvasManager;                             /** @brief < Специальный класс(Синглтрон), который управляет: ViewNode, ViewConnection. */

        // UI-Нода и фигуры
        class ViewNode;                                  /** @brief < Класс, который отрисовывает ноду. */
        class ViewPin;                                   /** @brief < Класс, который отрисовывает связь между нодами.*/
        class ViewConnection;                            /** @brief < Класс, который отрисовывает пины ноды. */

        // Орисовка меню и системных компонент

        /** @brief < Специальный класс(Синглтрон), в котором отрисовывается Menu и TopBar.
         *  @details В Menu можно лишь создать объкты, 
         * а TopBar сохраняет/загружает топологию, запускает обход графа, меняет темы и другие вещи в прогремме. 
         */
        class Menu;

        class ThemeManager;                              /** @brief < Загружает из Themes.json всё параметры требуемые для отрисовки программы.*/

        /**
         * @namespace Shape
         * @brief В данном пространстве имён находятся все фигуры/трафареты для нод
         * @details Именно трафареты отрисовываются, по факту это просто форма подложки на экране
         */
        namespace Shape {
            class BaseShape;                             /** @brief < Базовый(Абстрактный) Класс фигуры. */
            class RectangleShape;                        /** @brief < Фигура-Прямоугольник. */ 
            class CircleShape;                           /** @brief < Фигура-Круг. */
            class DShape;                                /** @brief < Фигура-D (В форме буквы D). */
        }

        /**
         * @namespace Command 
         * @brief В данном пространстве имён находятся все команды
         * @details Если пользователь создал ноду например через Menu, 
         * он может использовать сочетание клавиш "Ctrl + Z" или "Ctrl + Y"
         * чтобы отменить или вернуть действие.
         */
        namespace Command { 
            class Cmd;                                   /** @brief < Базовый(Абстрактный) класс команды. */ 
            class CmdCreateConnection;                   /** @brief < Команда для создания соединения между нодами. */
            class CmdDeleteAllConnectionForPin;          /** @brief < Пока пустой файл. */ 
            class CmdCreateNode;                         /** @brief < Команда для создание ноды. */ 
            class CmdDeleteNode;                         /** @brief < Команда для удаления ноды. */ 
            class CmdMoveNode;                           /** @brief < Команда для перемещения ноды. */ 
            class CommandBus;                            /** @brief < Специальный класс(Синглтрон), что хранит стеки команд и вызывает Redo() и Undo(). */ 
        }
    }

    /**
     * @namespace Graphics 
     * @brief В данном пространстве имён находятся классы использующие OpenGL.
     */
    namespace Graphics {
        class Camera;                                    /** @brief < Класс камера. Нужен, чтобы можно было перемещаться по бесконечному полю. */ 
        class Grid;                                      /** @brief < Отрисовывает сетку на бесконечном поле в программе. */ 
        class TextureManager;                            /** @brief < Загружает иконки для нод. Испльзует stb */ 
    }

    // =========================================================================
    // 4. ВНЕШНИЕ ИНТЕГРАЦИИ (OI - Open Interfaces)
    // Клиенты и коннекторы к внешним сервисам.
    // =========================================================================

    /**
     * @namespace OI
     * @brief В данном пространстве имён находятся все классы при помощи которых по API можно выгружать всю информацию.
     * 
     * @warning ВНИМАНИЕ!!! Ни один из файлой не реализован.
     */
    namespace OI {
        class BaseApp;                 
        class Telegram;                
        class GMail;                   
        class Mail;                    
        class VK;                      
        class Word;                    
        class Excel;                   
        class PowerPoint;              
        class TextDocument;            
    }  
}