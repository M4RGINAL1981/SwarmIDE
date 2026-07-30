#pragma once
#include "SwarmIDE.hpp"
#include "nodes/memory/BaseMemory.hpp"

namespace SwarmIDE::Nodes {

    /**
     * @class PostgreSQL
     * @brief Данная нода подключает PostgreSQL для агента, чтобы он мог хранить и помнить свои данные
     */
    class PostgreSQL : public BaseMemory {
    public:
        PostgreSQL(std::string name);

        void Execute() override;
        std::string getTypeName() override { return "PostgreSQL"; };

        /**
         * @brief Установить стандартные настройки ноды
         */
        virtual void ApplyDefaultSettings() override;
    private:
    };
}