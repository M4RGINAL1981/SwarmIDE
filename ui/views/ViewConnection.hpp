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
         * @param[in] id ID соединения (совпадает с ID логического соединения)
         * @param[in] sPin Указатель на выходной пин ноды-источника
         * @param[in] tPin Указатель на входной пин ноды-цели
         */
        ViewConnection(UUIDv4::UUID id, ViewPin* sPin, ViewPin* tPin) : connectionID(id), sourcePin(sPin), targetPin(tPin) {}

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
         * @brief Возвращает ID соединения
         * @return UUIDv4::UUID ID соединения
         */
        UUIDv4::UUID GetID() const { return connectionID; }
    private:

        UUIDv4::UUID connectionID;                    /** @brief < ID соединения */
        ViewPin* sourcePin = nullptr;                 /** @brief < Указатель на выходной пин ноды-источника */
        ViewPin* targetPin = nullptr;                 /** @brief < Указатель на входной пин ноды-цели */
    };

}