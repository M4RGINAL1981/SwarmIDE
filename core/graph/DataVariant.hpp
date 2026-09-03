#pragma once
#include "SwarmIDE.hpp"
#include <variant>
#include <string>
#include <typeinfo>
#include <stdexcept>
#include <json.hpp>

using namespace nlohmann;

namespace SwarmIDE::Core {

    /**
     * @class VariantTypeMismatchException
     * @brief Исключение, выбрасываемое при попытке извлечь из DataVariant данные неверного типа.
     * @details Служит для безопасной отладки связей в графе. Помогает сразу обнаружить,
     * если, например, узел-агент ConversationalAgent пытается прочитать `std::string`, а ModelNode передала `int`.
     */
    class VariantTypeMismatchException : public std::runtime_error {
    public:
        /**
         * @brief Конструктор исключения.
         * @param[in] actual Информация о типе данных, который реально сейчас находится в контейнере.
         * @param[in] msg Текстовое описание ошибки для логов вычислительного ядра.
         */
        VariantTypeMismatchException(const std::type_info& actual, const std::string& msg) 
            : std::runtime_error(msg), actual_type(actual) {}

        /**
         * @brief Возвращает имя типа данных, который фактически находится в контейнере на момент ошибки.
         * @return const char* Системное имя типа (результат выполнения `type_info::name()`).
         */
        const char* get_actual_type_name() const { return actual_type.name(); }

    private:
        const std::type_info& actual_type; /**< Ссылка на информацию о реальном типе данных внутри контейнера. */
    };

    /**
     * @class DataVariant
     * @brief Универсальный контейнер для пересылки любых типов данных по связям графа.
     * @details Является оберткой над `std::variant`. Позволяет динамически передавать между пинами 
     * различных узлов примитивные типы, сложные структуры JSON (через nlohmann::json).
     * 
     * @see SwarmIDE::Core::LogicPin
     * @see VariantTypeMismatchException
     */
    class DataVariant {
    public:
        /**
         * @brief Набор поддерживаемых типов данных вычислительным ядром SwarmIDE.
         * - `std::monostate` — Пустой контейнер (данные еще не пришли).
         * - `int` — Целые числа (счетчики, итераторы).
         * - `float` — Числа с плавающей точкой (координаты, веса моделей).
         * - `std::string` — Строки (тексты промптов, ответы агентов).
         * - `nlohmann::json` — Сложные структуры (ответы от Jira, Telegram, конфигурации).
         * - `void*` — Сырой указатель для передачи специфичных системных объектов.
         */
        using VariantType = std::variant<std::monostate, int, float, bool, std::string, nlohmann::json, void*>;

        /**
         * @brief Конструктор по умолчанию. Инициализирует контейнер в состоянии "пусто" (std::monostate).
         */
        DataVariant() : data(std::monostate{}) {};

        /**
         * @brief Универсальный шаблонный конструктор.
         * @tparam T Тип записываемых данных (должен входить в список VariantType).
         * @param[in] value Фактическое значение для сохранения в контейнер.
         */
        template<typename T>
        DataVariant(T value) : data(std::move(value)) {};

        /**
         * @brief Безопасное извлечение значения из контейнера.
         * @tparam T Ожидаемый тип данных, который нужно достать.
         * @return T Извлеченное значение исходного типа.
         * 
         * @throws VariantTypeMismatchException Если запрашиваемый тип @tparam T не совпадает с тем, что реально хранится в контейнере.
         * @see isType()
         */
        template<typename T>
        T getValue() const { // Извлечение значения
            if(!std::holds_alternative<T>(data)) {
                throw VariantTypeMismatchException(typeid(data), "Type mismatch in DataVariant");
            }
            return std::get<T>(data);
        }

        /**
         * @brief Проверка, хранится ли сейчас в контейнере конкретный тип данных.
         * @tparam T Проверяемый тип данных.
         * @return true Если в контейнере лежит именно этот тип.
         * @return false Если тип отличается.
         */
        template<typename T>
        bool isType() const { // Проверка типа значения
            if(!std::holds_alternative<T>(data)) return false;
            return true;
        }

        /**
         * @brief Проверка на заполненность логического контейнера.
         * @return true Если контейнер пуст (`std::monostate`).
         * @return false Если внутри лежат валидные данные любого другого типа.
         */
        bool isEmpty() const { 
            if(!std::holds_alternative<std::monostate>(data)) return false;
            return true;
        }

        /**
         * @brief Полностью очищает текущий логический контейнер, возвращая его в состояние `std::monostate`.
         */
        void clear() {
            data = std::monostate{};
        }

    private:
        VariantType data; /**< Внутреннее хранилище данных на базе std::variant. */
    };

}