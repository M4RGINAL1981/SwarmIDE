#pragma once
#include "SwarmIDE.hpp"
#include "ui/views/ViewPin.hpp"
#include "core/graph/ConnectionManager.hpp"
#include "ui/windows/ThemeManager.hpp"

namespace SwarmIDE::UI {

    /**
     * @brief Визуальное соединение
     * 
     */
    class ViewConnection {
    public:

        /**
         * @brief Конструктор визуального соединения
         * 
         * @param[in] sPin Указатель на выходной пин ноды-источника
         * @param[in] tPin Указатель на входной пин ноды-цели
         * @param[in] conn Указатель на структуру Core::Connection
         */
        ViewConnection(ViewPin* sPin, ViewPin* tPin, Core::Connection* conn) : sourcePin(sPin), targetPin(tPin), connection(conn) {};

        /**
         * @brief Деструктор визуального соеднинения
         */
        ~ViewConnection() = default;

        /**
         * @brief Отрисовка соединения кубической кривой Безье
         * @details Запрашивает экранные координаты у пинов, строит кубическую кривую Безье.
         * Если связь идет от работающей сейчас ноды, цвет линии может пульсировать синим.
         */
        void Draw(ImDrawList* drawList, float zoom);

        /**
         * @brief Вернуть логическое соединение
         * @return Core::Connection* 
         */
        Core::Connection* GetConnection() { return connection; }

    private:
        ViewPin* sourcePin = nullptr;                 /** @brief < Указатель на выходной пин ноды-источника */
        ViewPin* targetPin = nullptr;                 /** @brief < Указатель на входной пин ноды-цели */
        Core::Connection* connection = nullptr;       /** @brief < Информация о соединении */
    };

}