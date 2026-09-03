#pragma once 
#include "SwarmIDE.hpp"
#include "ui/command/Cmd.hpp"
#include "core/graph/DataVariant.hpp"

#include "uuid_v4/uuid_v4.h"

namespace SwarmIDE::UI::Command {

    class CmdSetProperty : public Cmd {
    public:
        CmdSetProperty(UUIDv4::UUID nodeID, std::string key, Core::DataVariant newValue);

        /**
         * @brief Установить новое значение для ноды
         */
        void Execute() override;

        /**
         * @brief Вернуть предыдушее значение для ноды
         */
        void Undo() override;

    private:

        UUIDv4::UUID nodeID;               /** @brief < ID ноды, чьё поле будет изменяться. */
        std::string key;                   /** @brief < Что именно за поле будет изменяться. */
        Core::DataVariant oldValue;        /** @brief < Старое значение для ноды. */
        Core::DataVariant newValue;        /** @brief < Новое значение для ноды. */
    };

}