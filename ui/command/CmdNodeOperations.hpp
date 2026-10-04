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
     * @see UI::CanvasManager
     */
    class CmdCreateNode : public Cmd {
    public:

        /**
         * @brief Конструктор команды, для создания новой ноды
         * @details Данная команда, создаёт новый объект через GraphRegistry
         * и добаляет его в GraphManager, через метод std::unique_ptr<Nodes::LogicNode.
         * 
         * @param[in] typeName Тип ноды.
         * @param[in] nodeName Имя для создания новой ноды через GraphRegistry.
         * @param[in] worldPos Положение ноды на экране.
         */
        CmdCreateNode(const std::string& typeName, const glm::vec2& worldPos);
        UUIDv4::UUID GetNodeID() const { return nodeID; }

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
        UUIDv4::UUID nodeID;                                         /** @brief < ID ноды */
        glm::vec2 worldPos = glm::vec2(0.0f, 0.0f);                  /** @brief < Положение ноды на экране */
        std::unique_ptr<Nodes::LogicNode> cachedNode = nullptr;      /** @brief < Кэш для ноды */
    };

    /**
     * @class CmdDeleteNode
     * @brief Комманда для удаления ноды
     * @see Core::GraphManager
     * @see Core::ConnectionManager
     * @see UI::CanvasManager
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

    /**
     * @class CmdDuplicateNode
     * @brief Команда для дублирования ноды
     * 
     * @see Core::GraphManager
     * @see UI::CanvasManager
     */
    class CmdDuplicateNode : public Cmd {
    public:

        /**
         * @brief Конструктор команды, для удаления ноды
         * @details Данная команда, удаляет объект через GraphManager
         * 
         * @param[in] nodeID ID ноды, что нужно удалить
         */
        CmdDuplicateNode(std::optional<UUIDv4::UUID> nodeID);

        /**
         * @brief Дублирует ноду
         */
        virtual void Execute() override;

        /**
         * @brief Удаляет дублированную ноду
         */
        virtual void Undo() override;

    private:
        bool m_isFirstExecution = true;                              /** @brief Первй ли это запуск. */

        std::optional<UUIDv4::UUID> nodeID = std::nullopt;           /** @brief ID скопированной ноды. */
        UUIDv4::UUID newNodeID;                                      /** @brief ID для нового дубликата. */

        std::string nodeName = "";                                   /** @brief Имя скопированной ноды. */
        std::string nodeTypeName = "";                               /** @brief Тип скопированной ноды. */

        glm::vec2 worldPos = glm::vec2(0.0f, 0.0f);                  /** @brief Координаты скопированной ноды. */
        std::unique_ptr<Nodes::LogicNode> cachedNode = nullptr;      /** @brief Кеш нового дубликата. */
    };


    class CmdMoveNode : public Cmd {
    public:
        /**
         * @brief Конструктор команды для перемещения ноды
         * 
         * @param[in] nodeID ID ноды, которую перемещаем.
         * @param[in] delta  Дельта между старой и новой позицей ноды.
         */
        CmdMoveNode(UUIDv4::UUID nodeID, const glm::vec2& delta);

        /**
         * @brief Перемещает ноду в новую позицию
         */
        virtual void Execute() override;

        /**
         * @brief Возвращает ноду на старую позицию
         */
        virtual void Undo() override;
    private:
        UUIDv4::UUID nodeID;
        glm::vec2 delta;
    };

}// namespace SwarmIDE::UI::Command