#include "SwarmIDE.hpp"
#include "nodes/memory/PostgreSQL.hpp"

namespace SwarmIDE::Nodes {

	PostgreSQL::PostgreSQL(std::string name) : BaseMemory(std::move(name)) {}

	void PostgreSQL::Execute() {}

	void PostgreSQL::ApplyDefaultSettings() { SetName("PostgreSQL"); }

	REGISTER_NODE(PostgreSQL, "PostgreSQL", "PostgreSQL", "STORAGE & MEMORY");
}