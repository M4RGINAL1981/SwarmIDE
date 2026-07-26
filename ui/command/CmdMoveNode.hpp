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
         * @param[in] nodeID 
         * @param[in] pos
         */
        CmdMoveNode(const UUIDv4::UUID& nodeID, const glm::vec2& pos);

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
        UUIDv4::UUID nodeID;
        glm::vec2 pos;
    };

}