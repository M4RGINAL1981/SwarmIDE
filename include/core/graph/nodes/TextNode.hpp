#pragma once
#include "core/graph/nodes/LogicNode.hpp"

namespace SwarmIDE::Core::Graph {

    class TextNode : public LogicNode {
    public:
        TextNode(const std::string& nodeName);                                     // Конструктор
        TextNode(const std::string& nodeName, const std::string& textContent);     // Конструктор (НE ИСПОЛЬЗОВАТЬ)
        virtual ~TextNode() = default;                                             // Деструктор 
        
        virtual void Execute() override;                                           // Логика ноды
        virtual std::string getTypeName() override { return "TextNode"; }          // Вернуть тип ноды

        std::string getText() const { return text; }                               // Вернуть текст
        void setText(const std::string& t) { text = t; }                           // Установить текст

    private:
        std::string text = "";                                                     // Текст, который можно передать напрямую в агента
    };
}