#pragma once
#include "SwarmIDE.hpp"

namespace SwarmIDE::UI {

    /**
     * @enum ColorRole
     * @brief Перечисление ролей цвета.
     * @details Используется как ключ для получения конкретного цвета из активной темы для любого UI-компонента.
     */
    enum class ColorRole {
        // --- Сетка (OpenGL) ---
        GridBackground,                   /** @brief < Цвет фона бесконечной сетки. */
        GridLine,                         /** @brief < Цвет самих линий сетки. */
        
        // --- Главное меню (ImGui) ---
        MenuBackground,                   /** @brief < Основной цвет фона панелей меню. */
        MenuBorder,                       /** @brief < Цвет обводки панелей меню. */

        // --- Ноды (Трафареты / Shapes) ---
        NodeBorder,                       /** @brief < Цвет стандартной обводки ноды. */
        NodeHover,                        /** @brief < Цвет обводки или заливки при наведении курсора на ноду. */
        
        // --- Статусы работы нод (States) ---
        NodeStateIdle,                    /** @brief < Цвет ноды в состоянии простоя (ожидает запуска). */
        NodeStateWaiting,                 /** @brief < Цвет ноды, ожидающей данные от предыдущих узлов. */
        NodeStateExecuting,               /** @brief < Цвет ноды в процессе выполнения логики. */
        NodeStateSuccess,                 /** @brief < Цвет ноды при успешном завершении работы. */
        NodeStateFailed,                  /** @brief < Цвет ноды при возникновении ошибки. */

        // --- Пины (Входы / Выходы) ---
        PinDefault,                       /** @brief < Стандартный цвет пина. */
        PinHover,                         /** @brief < Цвет пина при наведении курсора или попытке соединения. */

        // --- Соединения (Кривые Безье) ---
        ConnectionLine                    /** @brief < Основной цвет линии, соединяющей два пина. */
    };

    /**
     * @enum StyleVarRole 
     * @brief Перечисление ролей числовых стилей.
     * @details Используется как ключ для получения параметров геометрии (скругления, толщины) из активной темы.
     */
    enum class StyleVarRole {
        // --- Сетка ---
        GridStep,                         /** @brief < Расстояние (шаг) между линиями сетки. */
        GridThickness,                    /** @brief < Толщина линий сетки. */

        // --- Меню ---
        MenuRounding,                     /** @brief < Радиус скругления углов у окон меню. */

        // --- Ноды ---
        NodeRounding,                     /** @brief < Радиус скругления углов у визуальных нод. */
        NodeBorderThickness,              /** @brief < Толщина рамки (обводки) ноды. */

        // --- Пины ---
        PinRadius,                        /** @brief < Радиус (или половина ширины) пина. */

        // --- Соединения ---
        ConnectionThickness,              /** @brief < Толщина линии соединения. */
        ConnectionStrength                /** @brief < Сила натяжения кривой Безье (насколько сильно линия изгибается). */
    };

    /**
     * @enum PinShapeType
     * @brief Доступные геометрические формы пинов.
     */
    enum class PinShapeType { 
        Circle,                           /** @brief < Пин круглой формы. */
        Square                            /** @brief < Пин квадратной формы. */
    };

    /**
     * @enum ConnectionStyleType
     * @brief Доступные стили отрисовки соединительных линий.
     */
    enum class ConnectionStyleType {
        Solid,                            /** @brief < Сплошная непрерывная линия. */
        Dashed,                           /** @brief < Пунктирная линия (штрихами). */
        Dotted                            /** @brief < Точечная линия. */
    };
}