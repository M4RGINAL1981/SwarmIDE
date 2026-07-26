#pragma once
#include "SwarmIDE.hpp"
#include "core/graph/LogicPin.hpp"
#include "core/graph/GraphRegistry.hpp"
#include "core/engine/GenerateIDManager.hpp"
#include <optional>
#include <vector>
#include <memory>
#include <string>

namespace SwarmIDE::Nodes {

    /**
     * @enum State
     * @brief Текуший статус выполнения логического узла
     */
    enum State {
        Idle,                     /**< Простой: узел ожидает активации или триггера. */
        Waiting,                  /**< Ожидание: узел ждет данные от входящих пинов. */
        Executing,                /**< Выполнение: вычислительное ядро обрабатывает логику узла. */
        Success,                  /**< Успех: узел успешно выполнил задачу и передал данные дальше. */
        Failed                    /**< Провал: критическая ошибка при выполнении (например, таймаут API). */
    };
    
    /**
     * @class LogicNode
     * @brief Абстрактный базовый класс для всех функциональных узлов на холсте SwarmIDE.
     * 
     * @details Наследуется всеми сущностями системы: ИИ-агентами, моделями, базами данных, 
     * внешними интеграциями и инструментами. Управляет логическими пинами (входами/выходами) 
     * и своим пространственным положением на холсте.
     */
    class LogicNode {
    public:
        /**
         * @brief Конструктор узла с начальным положением в нулевых координатах.
         * @param[in]             name Имя отображаемого объекта на холсте.
         */
        LogicNode(const std::string& name) : name(name) { MakeNewID(); }; 
        
        virtual ~LogicNode() = default;
                                          
        /**
         * @brief Главный метод вычисления логики узла.
         * @details Переопределяется в конечных классах (например, в агентах). Вызывается 
         * вычислительным ядром асинхронно или в порядке обхода графа.
         */
        virtual void Execute() = 0;

        /**
         * @brief Выгрузка ноды в json.
         * @details Берёт все поля в ноде и загружает их в json.
         * 
         * @return                json 
         */
        virtual json serializeNode() = 0; 

        /**
         * @brief Загрузка ноды из json.
         * @details Берёт объект json, проходиться по всем его полям и загружает ноду.
         * 
         * @param[in]             nodeJson объект json, который хранит все поля ноды.
         */
        virtual void deserializeNode(json nodeJson) = 0;

        /**
         * @brief Возвращает строковый тип узла.
         * @return                std::string Уникальный идентификатор типа (например, "ConversationalAgent").
         */
        virtual std::string getTypeName() = 0;

        /**
         * @brief Добавляет новый пин.
         * @param[in] id          Идентификатор пина.
         * @param[in] dir         Направление пина (Вход / Выход).
         * @param[in] dataType    Тип данных, который ожидает или передает пин.
         * @param[in] index       Порядковый номер (для нод с несколькими одинаковыми пинами)
         * @param[in] maxConn     Максимальное количество одновременных связей (-1 для безлимита).
         * @param[in] value       Контейнер с текущим значением данных на пине.
         */
        void addPin(Core::PinID id, Core::Direction dir, Core::DataType dataType, int index = 0, int maxConn = 1, Core::DataVariant value = Core::DataVariant());

        /**
         * @brief Получить текущее значение входного пина.
         * @param[in] id          Идентификатор пина.
         * @param[in] dir         Направление пина (Вход / Выход).
         * @param[in] index       Порядковый номер пина (для нод с несколькими одинаковыми пинами).
         *  
         * @return                DataVariant Вариантный тип данных, содержащий текущее значение.
         * 
         * @see Core::DataVariant
         */
        Core::DataVariant getPinValue(Core::PinID id, Core::Direction dir, int index = 0);

        /**
         * @brief Записать значение во входной пин (обычно вызывается менеджером связей).
         * @param[in] value      Новые данные для записи.
         * @param[in] id         Идентификатор пина.
         * @param[in] dir        Направление пина (Вход / Выход).
         * @param[in] index      Порядковый номер пина (для нод с несколькими одинаковыми пинами).
         */
        void setPinValue(Core::DataVariant value, Core::PinID id, Core::Direction dir, int index = 0);

        /**
         * @brief Поиск указателя на входной пин по его ID.
         * @param[in] id         Идентификатор пина.
         * @param[in] dir        Направление пина (Вход / Выход).
         * @param[in] index      Порядковый номер пина (для нод с несколькими одинаковыми пинами).
         * 
         * @return               LogicPin* Указатель на пин или nullptr, если пин не найден.
         * 
         * @see Core::LogicPin
         */
        Core::LogicPin* findPin(Core::PinID id, Core::Direction dir, int index = 0);

        /**
         * @brief Проверяет, заполнены ли все входные пины данными, необходимыми для старта.
         * @return               true Если все обязательные входы содержат валидные данные.
         */
        bool isFullInputPin() const;

        /**
         * @brief Проверяет, заполнены ли все выходные пины данными, необходимыми для старта.
         * @return               true Если все обязательные выходы содержат валидные данные
         */
        bool isFullOutputPin() const;

        /**
         * @brief Возвращает пины логической ноды
         * @return std::vector<std::unique_ptr<Core::LogicPin>>& пины
         */
        std::vector<std::unique_ptr<Core::LogicPin>>& GetPins() { return pins; }

        /**
         * @brief Генерирует и присваивает узлу новый уникальный UUID.
         * @note Использует синглтон GenerateIDManager под капотом.
         */
        void MakeNewID() {  id = Core::GenerateIDManager::getInstance().GetNewID(); }

        /**
         * @brief Сброс состояния узла.
         * @details Полностью очищает данные на всех пинах и переводит статус в State::Waiting.
         */
        virtual void Reset();

        // =====================================================================
        // Геттеры и Сеттеры 
        // =====================================================================            

        UUIDv4::UUID GetID() { return id; }               
        void SetID(UUIDv4::UUID i) { id = i; }                      

        std::string GetName() { return name; }                      
        void SetName(const std::string& n) { name = n; }            

        State GetState() { return state; }                          
        void SetState(State st) { state = st; }                     

        // Запрет копирования и перемещения
        LogicNode(LogicNode&&) noexcept = default;
        LogicNode& operator=(LogicNode&&) noexcept = default;
        LogicNode(const LogicNode&) = delete;
        LogicNode& operator=(const LogicNode&) = delete;

    protected:
        std::string name;                                           /**< @brief Имя объекта на сцене. */
        UUIDv4::UUID id;                                            /**< @brief Уникальный UUID узла в рамках сессии графа. */

        std::vector<std::unique_ptr<Core::LogicPin>> pins;          /**< @brief Вектор умных указателей на порты. */
        State state = State::Idle;                                  /**< @brief Текущий статус выполнения ноды вычислительным ядром. */
    };
}