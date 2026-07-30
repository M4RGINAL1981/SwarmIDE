#pragma once
#include "SwarmIDE.hpp"
#include "ui/command/Cmd.hpp"
#include "nodes/base/LogicNode.hpp"
#include "glm/glm.hpp"

namespace SwarmIDE::UI::Command {

    /**
     * @class CmdMoveNode
     * @brief Команда для перемещения ноды
     * 
     * @see SwarmIDE::Nodes::LogicNode
     * @see SwarmIDE::UI::CanvasManager
     */
    class CmdMoveNode : public Cmd {
    public:

        /**
         * @brief Конструктор команды для перемещния ноды
         * 
         * @param[in] id 
         * @param[in] oldP
         * @param[in] newP
         */
        CmdMoveNode(const UUIDv4::UUID& id, const glm::vec2& oldP, glm::vec2& newP) 
            : nodeID(id), oldPos(oldP), newPos(newP) {};

        /**
         * @brief Перемещение ноды на координаты
         * 
         */
        virtual void Execute() override;

        /**
         * @brief Перемещение ноды обратно
         * 
         */
        virtual void Undo() override;

    private:
        UUIDv4::UUID nodeID;      /** @brief < ID ноды */
        glm::vec2 oldPos;         /** @brief < Старая позиция ноды */
        glm::vec2 newPos;         /** @brief < Новая позиция ноды */
    };

}