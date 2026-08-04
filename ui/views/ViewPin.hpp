#pragma once 
#include "SwarmIDE.hpp"
#include "core/graph/LogicPin.hpp"
#include "glm/glm.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace SwarmIDE::UI {

    /**
     * @class ViewPin
     * @brief Визуальный компонент точки контакта ноды.
     * 
     * @see SwarmIDE::Core::LogicPin
     */
    class ViewPin {
    public:
        /**
         * @brief Конструктор визуального пина.
         * 
         * @param id        ID пина (совпадает с ID логического пина).
         * @param localPos  Локальные координаты визуального пина, относительно фигуры
         * @param normal    Вектор нормали пина
         */
        ViewPin(UUIDv4::UUID id, glm::vec2 localPos, glm::vec2 normal);

        /**
         * @brief Деструктор визуального пина.
         */
        ~ViewPin() = default;

        /**
         * @brief Проверяет, совпадает ли ID пина с переданным.
         * @param[in] id пина для проверки на совпадение.
         * @return true Если ID пина совпадает с переданным.
         * @return false Если ID пина не совпадает с переданным.
         */
        bool ComplianceСheck(UUIDv4::UUID id) { return this->id == id; }

        /**
         * @brief Пересчитывание мировые координаты, отткаливаясь от коодинат своего родителя (BaseShape / ViewNode)
         * 
         * @param NodeScreenPos 
         * @param zoom 
         */
        void UpdatePos(glm::vec2 NodeScreenPos, float zoom);

        /**
         * @brief Отрисовка пина.
         * @param[in] drawList список для отрисовки
         * @param[in] zoom Текущий зум
         */
        void Draw(ImDrawList* drawList, float zoom);

        /**
         * @brief  Возвращает идентификатор пина
         * @return UUIDv4::UUID Идентификатор пина
         */
        UUIDv4::UUID GetPinID() { return id; }
        /**
         * @brief Устанавливает новый идентификатор пина
         * @param id Новй идентификатор пина
         */
        void SetPinID(UUIDv4::UUID id) { this->id = id; }
        
        /**
         * @brief Возвращает локальные координаты пина
         * @return glm::vec2 Локальные координаты пина
         */
        glm::vec2 GetLocalPos() { return localPos; }
        /**
         * @brief Устанавливает новые локальные координаты пина
         * @param localPos Новые локальные координаты пина
         */
        void SetLocalPos(glm::vec2 localPos) { this->localPos = localPos; }

        /**
         * @brief Вернуть экраные координаты пина
         * @return glm::vec2 
         */
        glm::vec2 GetScreenPos() { return cachedScreenPos; }
        /**
         * @brief Установить экранные координаты пина
         * @param screenPos 
         */
        void SetScreenPos(glm::vec2 screenPos) { cachedScreenPos = screenPos; }

        /**
         * @brief Возвращает нормаль пина
         * 
         * @return glm::vec2 
         */
        glm::vec2 GetNormal() { return normal; }
        /**
         * @brief Устанавливает нормаль для пина 
         * @param n 
         */
        void SetNormal(glm::vec2 n) { normal = n; }

        /**
         * @brief Наведена ли мышь на пин.
         * 
         * @return true  Мышь указывает на пин.
         * @return false Мышь не указывает на пин.
         */
        bool GetHovered() { return is_hovered; }
        /**
         * @brief Установить переменную о наведении мыши на пин
         * @param[in] ih Переменная о наведении мыши на пин
         */
        void SetHovered(bool ih) { is_hovered = ih; }

        /**
         * @brief Проверка на то, что наведена ли мышь на пин или нет
         * 
         * @param mousePos текущие положение мыши
         * @param zoomFactor Текущий зум
         * @return true Мышь наведена на пин
         * @return false Мышь не наведена на пин
         */
        bool IsPointInside(glm::vec2 mousePos, float zoomFactor);

    private:
        UUIDv4::UUID id;                      /** @brief < Уникальный идентификатор пина. */
        glm::vec2 localPos;                   /** @brief < Смещение относительно верхнего левого угла родительской ноды. */
        glm::vec2 normal;                     /** @brief < Вектор нормали */
        glm::vec2 cachedScreenPos;            /** @brief < Мировые экранные координаты пина (рассчитываются каждый кадр с учетом зума холста). */
        bool is_hovered;                      /** @brief < Флаг, указыывающий, наведена ли сейчас мышь на пин или нет. */
    };
}