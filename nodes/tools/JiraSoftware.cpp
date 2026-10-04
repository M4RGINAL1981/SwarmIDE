#include "SwarmIDE.hpp"
#include "nodes/tools/JiraSoftware.hpp"

namespace SwarmIDE::Nodes {

	JiraSoftware::JiraSoftware(std::string name) : BaseTool(std::move(name)) {}

	void JiraSoftware::Execute() {}

	void JiraSoftware::ApplyDefaultSettings() { SetName("JiraSoftware"); }

	REGISTER_NODE(JiraSoftware, "JiraSoftware", "Jira Software", "EXTERNAL TOOLS");
}