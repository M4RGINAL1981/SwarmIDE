#pragma once
#include <variant>
#include <string>
#include <typeinfo>
#include <stdexcept>
#include <json.hpp>

using namespace nlohmann;

namespace SwarmIDE::Core::Graph {

    class VariantTypeMismatchException : public std::runtime_error {
    /*
        Специальный класс, для вызова ошибки, при неправильном типе извлечения данных
    */
    public:
        VariantTypeMismatchException(const std::type_info& actual, const std::string& msg) 
            : std::runtime_error(msg), actual_type(actual) {}

        const char* get_actual_type_name() const { return actual_type.name(); }

    private:
        const std::type_info& actual_type;
    };

    class DataVariant {
    /*
        Контейнер для пересылки любых данных по связям графа.
    */        

    public:
        using VariantType = std::variant<std::monostate, int, float, std::string, nlohmann::json, void*>;

        DataVariant() : data(std::monostate{}) {};

        template<typename T>
        DataVariant(T value) : data(std::move(value)) {};

        template<typename T>
        T getValue() { // Извлечение значения
            if(!std::holds_alternative<T>(data)) {
                throw VariantTypeMismatchException(typeid(data), "Type mismatch in DataVariant");
            }
            return std::get<T>(data);
        }

        template<typename T>
        bool isType() { // Проверка типа значения
            if(!std::holds_alternative<T>(data)) return false;
            return true;
        }

        // Проверка на заполненость логического контейнера. Возвращает true - если тип std::monostate, иначе - false
        bool isEmpty() { 
            if(!std::holds_alternative<std::monostate>(data)) return false;
            return true;
        }

        // Очишает текущий логический контейнер
        void clear() {
            data = std::monostate{};
        }

    private:
        VariantType data;
    };

}