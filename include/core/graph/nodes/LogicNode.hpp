#pragma once
#include "core/graph/LogicPin.hpp"
#include "core/graph/GraphRegistry.hpp"
#include "core/GenerateIDManager.hpp"
#include <glm/glm.hpp>
#include <optional>

namespace SwarmIDE::Core::Graph {

    enum State {
    /*
        Текущий статус ноды
    */
        Idle,           // Простой
        Waiting,        // Ожидание
        Executing,      // Выполнение
        Success,        // Успех
        Failed,         // Провал
    };
    
    class LogicNode {
    /*
        Абстрактный класс, от которого наследуются абсолютно все сущности:
        * Агенты
        * Модели
        * Память
        * Инструменты
        * Интеграции
    */
    public:

        LogicNode(const std::string& name) : name(name) { worldPos = glm::vec2(0.0f, 0.0f); MakeNewID(); }; 
        LogicNode(const std::string& name, glm::vec2 worldPos) : name(name), worldPos(worldPos) { MakeNewID(); };
        
        virtual ~LogicNode() = default;
                                          
        virtual void Execute() = 0;                                 // (Запуск ноды)
        virtual std::string getTypeName() = 0;                      // (Возвращает тип ноды)

        void addInputPin(std::string id, std::string type);         // Добавляет входной пин
        void addOutputPin(std::string id, std::string type);        // Добавляет выходной пин

        DataVariant getInputPinValue(std::string id);               // Считывает значение логического пина (Вход)
        DataVariant getOutputPinValue(std::string id);              // Считывает значение логического пина (Выход)

        void setInputPinValue(std::string id, DataVariant value);   // Устанавливает значение логического пина (Вход)
        void setOutputPinValue(std::string id, DataVariant value);  // Устанавливает значение логического пина (Выход)
        
        LogicPin* getLogicInputPinByID(std::string id);             // Получить входной логический пин
        LogicPin* getLogicOutputPinByID(std::string id);            // Получить выходной логический пин

        bool isFullInputPin() const;                                // Проверка на заполненость входных пинов
        bool isFullOutputPin() const;                               // Проверка на заполненость выходных пинов

        virtual void Reset();                                       // Очистить полностью все пины и поставить статус ожидание

        // Геттеры и Сеттеры 

        glm::vec2 GetWorldPos() { return worldPos; }                // Вернуть полжоение ноды
        void SetWorldPos(glm::vec2 p) { worldPos = p; }             // Установить положение ноды

        UUIDv4::UUID GetID() { return id; }                         // Вернуть id ноды 
        void SetID(UUIDv4::UUID i) { id = i; }                      // Установить id ноды

        void MakeNewID() {  id = GenerateIDManager::getInstance().GetNewID(); }

        std::string GetName() { return name; }                      // Вернуть имя ноды
        void SetName(const std::string& n) { name = n; }            // Установить имя ноды

        State GetState() { return state; }                          // Вернуть статус ноды
        void SetState(State st) { state = st; }                     // Установить статус ноды


        LogicNode(LogicNode&&) noexcept = default;
        LogicNode& operator=(LogicNode&&) noexcept = default;
        LogicNode(const LogicNode&) = delete;
        LogicNode& operator=(const LogicNode&) = delete;

    protected:
        std::string name;                                           // Имя объекта
        UUIDv4::UUID id;                                            // ID объекта
        glm::vec2 worldPos;                                         // Коодринаты Объекта

        std::vector<std::unique_ptr<LogicPin>> inputs;              // Массив входных портов 
        std::vector<std::unique_ptr<LogicPin>> outputs;             // Массив выходных портов

        State state = State::Idle;                                  // Текущиее статус ноды
    };
}