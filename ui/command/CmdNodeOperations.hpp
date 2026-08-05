#pragma once
#include "SwarmIDE.hpp"
#include "ui/command/Cmd.hpp"
#include "core/graph/GraphManager.hpp"
#include "nodes/base/LogicNode.hpp"
#include "glm/glm.hpp"
#include <string>
#include <vector>
#include <optional>

namespace SwarmIDE::UI::Command {
    
    /**
     * @class CmdCreateNode
     * @brief Комманда для создания ноды
     * @see Core::GraphRegistry 
     * @see Core::GraphManager
     * 
     * @todo При удалении ноды - нужно удалять и все подключения, что есть у ноды. А при отмене опять их создавать.
     */
    class CmdCreateNode : public Cmd {
    public:

        /**
         * @brief Конструктор команды, для создания новой ноды
         * @details Данная команда, создаёт новый объект через GraphRegistry
         * и добаляет его в GraphManager, через метод std::unique_ptr<Nodes::LogicNode.
         * 
         * @param[in] typeName Тип ноды
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
        bool m_isFirstExecution = true;                              /** @brief < Первый ли это запуск */
        
        std::string typeName;                                        /** @brief < Тип ноды */
        std::string nodeName;                                        /** @brief < Имя ноды */
        UUIDv4::UUID nodeID;                                         /** @brief < ID ноды */
        glm::vec2 worldPos = glm::vec2(0.0f, 0.0f);                  /** @brief < Положение ноды на экране */
        std::unique_ptr<Nodes::LogicNode> cachedNode = nullptr;      /** @brief < Кэш для ноды */
    };

    /**
     * @class CmdDeleteNode
     * @brief Комманда для удаления ноды
     * @see Core::GraphManager
     * @see Core::ConnectionManager
     * 
     * @todo При удалении ноды - нужно удалять и все подключения, что есть у ноды. А при отмене опять их создавать.
     */
    class CmdDeleteNode : public Cmd {
    public:

        /**
         * @brief Конструктор команды, для удаления ноды
         * @details Данная команда, удаляет объект через GraphManager
         * 
         * @param[in] nodeID ID ноды, что нужно удалить
         */
        CmdDeleteNode(std::optional<UUIDv4::UUID> nodeID);

        /**
         * @brief Удаляет ноду
         */
        virtual void Execute() override;

        /**
         * @brief Создаёт ноду
         */
        virtual void Undo() override;

    private:
        
        std::optional<UUIDv4::UUID> nodeID = std::nullopt;           /** @brief < ID ноды */
        glm::vec2 worldPos = glm::vec2(0.0f, 0.0f);                  /** @brief < Положение ноды на экране */
        std::unique_ptr<Nodes::LogicNode> cachedNode = nullptr;      /** @brief < Кэш для ноды */

        std::vector<Core::LogicConnection> connectionForNode;        /** @brief < Все соединения, что были у ноды */
    };

}