#pragma once 
#include "SwarmIDE.hpp"
#include "ui/command/Cmd.hpp"
#include "core/graph/ConnectionManager.hpp"
#include "uuid_v4/uuid_v4.h"
#include <vector>

namespace SwarmIDE::UI::Command {

    /**
     * @class CmdDeleteAllConnectionForPin
     * @brief Команда для удаления всех подключений с конкретным пином
     * 
     * @see SwarmIDE::Core::LogicPin
     * @see SwarmIDE::Nodes::LogicNode
     * @see SwarmIDE::Core::LogicConnection
     */
    class CmdDeleteAllConnectionForPin : public Cmd {
    public:
        CmdDeleteAllConnectionForPin(UUIDv4::UUID pinID) : pinID(pinID) {}

        /**
         * @brief Удаляет соединения с пином
         */
        virtual void Execute() override;
        

        /**
         * @brief Восстанавливает соединения с пином
         */
        virtual void Undo() override;

    private:    
        UUIDv4::UUID pinID;                                  /** @brief < ID пина у которого удаляются все соединения. */
        UUIDv4::UUID nodeIDforPin;                           /** @brief < ID ноды, которой принадлежит пин. */
        
        std::vector<Core::LogicConnection> connectionForPin; /** @brief < Соединения с пином */
    };

}