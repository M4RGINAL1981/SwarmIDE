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
    // 1. ВЫЧИСЛИТЕЛЬНОЕ ЯДРО (Core)
    // Менеджеры, абстракции графа, сохранения, генерация ID. Никакого UI.
    // =========================================================================
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
    // 2. УЗЛЫ ГРАФА (Nodes)
    // Вся логика, которая ставится на холст. Все наследники LogicNode.
    // =========================================================================
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
    // 3. ИНТЕРФЕЙС И ГРАФИКА (UI)
    // Всё, что отвечает за отрисовку (ImGui + OpenGL).
    // =========================================================================

    // ImGui
    namespace UI {

        // Менеджер отрисовки нод и подключений                    
        class AppController;  
        class CanvasManager;

        // UI-Нода и фигуры
        class ViewNode;      
        class ViewPin;
        class ViewConnection;

        class BaseShape;               
        class RectangleShape;          
        class CircleShape;   
        class DShape;

        // Орисовка меню и системных компонент
        class Menu;
        class Theme;

        namespace Shape {
            class BaseShape;
            class CircleShape;
            class DShape;
            class RectangleShape;
        }

        namespace Command {
            class Cmd;
            class CmdCreateConnection;
            class CmdCreateNode;
            class MoveNode;
            class CommandBus;
        }
    }   

    // OpenGL
    namespace Graphics {

        // Камера, сетка, цвета и менеджер текстур (Иконок)
        class Camera;
        class Grid;
        class TextureManager;
    }

    // =========================================================================
    // 4. ВНЕШНИЕ ИНТЕГРАЦИИ (OI - Open Interfaces)
    // Клиенты и коннекторы к внешним сервисам.
    // =========================================================================
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