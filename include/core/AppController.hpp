#include "core/AgentManager.hpp"
#include "graphics/Camera.hpp"
#include "graphics/Grid.hpp"
#include "SwarmIDE.hpp"

namespace SwarmIDE::Core {

    class AppController {
    public:
        AppController();

        void run();
    private:
       GLFWwindow* window;
        Camera camera;
        Grid grid;
        AgentManager& manager;

        glm::vec2 lastMousePos = glm::vec2(0.0f);
        bool isPanning = false;
    };
}