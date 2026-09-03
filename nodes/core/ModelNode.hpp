#pragma once
#include "SwarmIDE.hpp"
#include "nodes/base/LogicNode.hpp"

namespace SwarmIDE::Nodes {

    /**
     * @class ModelNode
     * @brief Системный узел для хранения и передачи id модели.
     * @details Данный узел используется в качестве источника данных для ИИ-агентов.
     * Он позволяет жестко задать id модели прямо на холсте и передать его через
     * выходной пин
     * 
     * @see SwarmIDE::Nodes::LogicNode
     * @see SwarmIDE::Nodes::BaseAgent
     */
    class ModelNode : public LogicNode {
    public:
        /**
         * @brief Construct a new Model Node object
         * @details Создает узел в нулевых координатах холста и иницилизирует 
         * один стандартный выходной пин для передачи id модели
         * 
         * @param[in] nodeName Отображаемое имя узла на холсте (например, "DeepSeek").
         */
        ModelNode(const std::string& nodeName);                                         

        /**
         * @brief Конструктор с предустановленным контентом.
         * @param[in] nodeName Отображаемое имя узла.
         * @param[in] targetModelID Начальное значение ID модели.
         * 
         * @deprecated Данный конструктор не поддерживается в данной версии.
         * @warning НЕ ИСПОЛЬЗОВАТЬ в новой логике оркестрации.
         */
        ModelNode(const std::string& nodeName, const std::string& targetModelID);      
        
        virtual ~ModelNode() = default;                                            

        /**
         * @brief Активация логики узла.
         * @details Считывает внутреннюю переменную #modelID и записывает её значение
         * в выходной логический пин. Переводит статус узла в состояние State::Success.
         * 
         * @note Метод автоматически вызывается классом SwarmIDE::Core::GraphManager при обходе графа.
         */
        virtual void Execute() override;                                       

        /**
         * @brief Возвращает уникальный строковый идентификатор id модели.
         * @return std::string Строка "ModelNode".
         * @see LogicNode::getTypeName()
         */
        virtual std::string getTypeName() override { return "ModelNode"; }     

        /**
         * @brief Возвращает текущеe id модели.
         * @return std::string Строка c id модели.
         */
        std::string getTargetModelID() const { return modelID; }     

        /**
         * @brief Устанавливает новое id модели
         * @details Обычно вызывается при создании модели через боковое меню
         * 
         * @param[in] newModelID Новый id модели.
         */
        void setTargetModelID(const std::string& newModelID) { modelID = newModelID; }            

        /**
         * @brief 
         * @return std::vector<Core::PropertyDescriptor> 
         */
        virtual std::vector<Core::PropertyDescriptor> properties() override;

        /**
         * @brief Применить стандартные настройки для ноды
         */
        virtual void ApplyDefaultSettings() override;

    private:
        std::string modelID;  /**< Внутреннее хранилище id модели, передаваемое в агента. */       
    };
}