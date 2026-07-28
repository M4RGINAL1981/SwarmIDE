#pragma once
#include "SwarmIDE.hpp"
#include "ui/command/Cmd.hpp"
#include "core/graph/GraphManager.hpp"
#include "nodes/base/LogicNode.hpp"
#include <string>

namespace SwarmIDE::UI::Command {
    
    /**
     * @class CmdCreateNode
     * @brief Комманда для создания ноды
     * @see Core::GraphRegistry 
     * @see Core::GraphManager
     */
    class CmdCreateNode : public Cmd {
    public:

        /**
         * @brief Конструктор команды, для создания новой ноды
         * @details Данная команда, создаёт новый объект через GraphRegistry
         * и добаляет его в GraphManager, через метод std::unique_ptr<Nodes::LogicNode.
         * 
         * @param[in] nodeName Имя для создания новой ноды через GraphRegistry
         */
        CmdCreateNode(const std::string& typeName, const std::string& nodeName);

        /**
         * @brief Создаёт ноду
         */
        virtual void Execute() override;

        /**
         * @brief Удаляет ноду
         */
        virtual void Undo() override;

    private:
        std::string typeName;     /** @brief < Тип ноды */
        std::string nodeName;     /** @brief < Имя ноды */
        UUIDv4::UUID nodeID;      /** @brief < ID ноды */
    };

}