#pragma once
#include "SwarmIDE.hpp"
#include "nodes/base/LogicNode.hpp"

namespace SwarmIDE::Nodes {

    /**
     * @class TextNode
     * @brief Системный узел для хранения и передачи статического текста.
     * @details Данный узел используется в качестве источника данных для ИИ-агентов.
     * Он позволяет жестко задать контекст задачи
     * прямо на холсте и передать его через выходной пин.
     * 
     * @see SwarmIDE::Nodes::LogicNode
     * @see SwarmIDE::Nodes::BaseAgent
     */
    class TextNode : public LogicNode {
    public:
        /**
         * @brief Основной конструктор текстового узла.
         * @details Создает узел в нулевых координатах холста и инициализирует 
         * один стандартный выходной пин для передачи текста.
         * 
         * @param[in] nodeName Отображаемое имя узла на холсте (например, "Text").
         */
        TextNode(const std::string& nodeName);

        /**
         * @brief Конструктор с предустановленным контентом.
         * @param[in] nodeName Отображаемое имя узла.
         * @param[in] textContent Начальное текстовое содержимое.
         * 
         * @deprecated Данный конструктор не поддерживается в данной версии.
         * @warning НЕ ИСПОЛЬЗОВАТЬ в новой логике оркестрации.
         */
        TextNode(const std::string& nodeName, const std::string& textContent);

        virtual ~TextNode() = default;
        
        /**
         * @brief Активация логики узла.
         * @details Считывает внутреннюю переменную #text и записывает её значение
         * в выходной логический пин. Переводит статус узла в состояние State::Success.
         * 
         * @note Метод автоматически вызывается классом SwarmIDE::Core::GraphManager при обходе графа.
         */
        virtual void Execute() override;

        /**
         * @brief Возвращает уникальный строковый идентификатор типа узла.
         * @return std::string Строка "TextNode".
         * @see LogicNode::getTypeName()
         */
        virtual std::string getTypeName() override { return "TextNode"; }

        /**
         * @brief Возвращает текущее текстовое содержимое узла.
         * @return std::string Строка с текстом.
         */
        std::string getText() const { return text; }

        /**
         * @brief Устанавливает новое текстовое содержимое.
         * @details Обычно вызывается из графической оболочки (ImGui), когда пользователь
         * редактирует текстовое поле внутри узла на холсте.
         * 
         * @param[in] t Новая текстовая строка.
         */
        void setText(const std::string& t) { text = t; }

    private:
        std::string text = ""; /**< Внутреннее хранилище текста, передаваемого в агента. */
    };
}
