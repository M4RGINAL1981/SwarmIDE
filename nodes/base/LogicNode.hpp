#pragma once
#include "SwarmIDE.hpp"
#include "core/graph/LogicPin.hpp"
#include "core/graph/GraphRegistry.hpp"
#include "core/engine/GenerateIDManager.hpp"
#include <glm/glm.hpp>
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
        Idle,           /**< Простой: узел ожидает активации или триггера. */
        Waiting,        /**< Ожидание: узел ждет данные от входящих пинов. */
        Executing,      /**< Выполнение: вычислительное ядро обрабатывает логику узла. */
        Success,        /**< Успех: узел успешно выполнил задачу и передал данные дальше. */
        Failed          /**< Провал: критическая ошибка при выполнении (например, таймаут API). */
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
         * @param[in] name Имя отображаемого объекта на холсте.
         */
        LogicNode(const std::string& name) : name(name) { worldPos = glm::vec2(0.0f, 0.0f); MakeNewID(); }; 
        
        /**
         * @brief Конструктор узла с указанием точных координат на холсте.
         * @param[in] name Имя отображаемого объекта.
         * @param[in] worldPos Позиция в 2D-пространстве сцены OpenGL.
         */
        LogicNode(const std::string& name, glm::vec2 worldPos) : name(name), worldPos(worldPos) { MakeNewID(); };
        
        virtual ~LogicNode() = default;
                                          
        /**
         * @brief Главный метод вычисления логики узла.
         * @details Переопределяется в конечных классах (например, в агентах). Вызывается 
         * вычислительным ядром асинхронно или в порядке обхода графа.
         */
        virtual void Execute() = 0;

        /**
         * @brief Возвращает строковый тип узла.
         * @return std::string Уникальный идентификатор типа (например, "ConversationalAgent").
         */
        virtual std::string getTypeName() = 0;

        /**
         * @brief Добавляет новый входной пин к узлу.
         * @param[in] id Уникальный строковый идентификатор пина внутри этой ноды.
         * @param[in] type Строковое имя типа данных, принимаемого пином (например, "json").
         */
        void addInputPin(std::string id, std::string type);

        /**
         * @brief Добавляет новый выходной пин к узлу.
         * @param[in] id Уникальный строковый идентификатор пина внутри этой ноды.
         * @param[in] type Строковое имя типа данных, который пин отдает наружу.
         */
        void addOutputPin(std::string id, std::string type);

        /**
         * @brief Получить текущее значение входного пина.
         * @param[in] id Идентификатор пина.
         * @return DataVariant Вариантный тип данных, содержащий текущее значение.
         */
        Core::DataVariant getInputPinValue(std::string id);

        /**
         * @brief Получить текущее значение выходного пина.
         * @param[in] id Идентификатор пина.
         * @return DataVariant Вариантный тип данных, содержащий текущее значение.
         */
        Core::DataVariant getOutputPinValue(std::string id);

        /**
         * @brief Записать значение во входной пин (обычно вызывается менеджером связей).
         * @param[in] id Идентификатор пина.
         * @param[in] value Новые данные для записи.
         */
        void setInputPinValue(std::string id, Core::DataVariant value);

        /**
         * @brief Записать значение в выходной пин (обычно вызывается менеджером связей).
         * @param[in] id Идентификатор пина.
         * @param[in] value Новые данные для записи.
         */
        void setOutputPinValue(std::string id, Core::DataVariant value);
        
        /**
         * @brief Поиск указателя на входной пин по его ID.
         * @param[in] id Идентификатор пина.
         * @return LogicPin* Указатель на пин или nullptr, если пин не найден.
         */
        Core::LogicPin* getLogicInputPinByID(std::string id);

        /**
         * @brief Поиск указателя на выходной пин по его ID.
         * @param[in] id Идентификатор пина.
         * @return LogicPin* Указатель на пин или nullptr, если пин не найден.
         */
        Core::LogicPin* getLogicOutputPinByID(std::string id);

        /**
         * @brief Проверяет, заполнены ли все входные пины данными, необходимыми для старта.
         * @return true Если все обязательные входы содержат валидные данные.
         */
        bool isFullInputPin() const;

        /**
         * @brief Проверяет, заполнены ли все выходные пины данными, необходимыми для старта.
         * @return true Если все обязательные выходы содержат валидные данные.
         */
        bool isFullOutputPin() const;

        /**
         * @brief Сброс состояния узла.
         * @details Полностью очищает данные на всех пинах и переводит статус в State::Waiting.
         */
        virtual void Reset();

        // =====================================================================
        // Геттеры и Сеттеры 
        // =====================================================================

        glm::vec2 GetWorldPos() { return worldPos; }                
        void SetWorldPos(glm::vec2 p) { worldPos = p; }             

        UUIDv4::UUID GetID() { return id; }                         
        void SetID(UUIDv4::UUID i) { id = i; }                      

        /**
         * @brief Генерирует и присваивает узлу новый уникальный UUID.
         * @note Использует синглтон GenerateIDManager под капотом.
         */
        void MakeNewID() {  id = Core::GenerateIDManager::getInstance().GetNewID(); }

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
        std::string name;                                           /**< Имя объекта на сцене. */
        UUIDv4::UUID id;                                            /**< Уникальный UUID узла в рамках сессии графа. */
        glm::vec2 worldPos;                                         /**< 2D Координаты центра объекта в мировом пространстве. */

        std::vector<std::unique_ptr<Core::LogicPin>> inputs;        /**< Вектор умных указателей на входные порты. */
        std::vector<std::unique_ptr<Core::LogicPin>> outputs;       /**< Вектор умных указателей на выходные порты. */

        State state = State::Idle;                                  /**< Текущий статус выполнения ноды вычислительным ядром. */
    };
}