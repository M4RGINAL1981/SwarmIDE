#include "core/ObjectManager.hpp"
#include "core/ConnectionManager.hpp"
#include "core/SerializationManager.hpp"
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
        ObjectManager& object_manager;
        ConnectionManager& connect_manager;
        SerializationManager& serialization_manager;
        
        SwarmIDE::UI::Fonts fonts;

        glm::vec2 lastMousePos = glm::vec2(0.0f);
        bool isPanning = false;
    };
}