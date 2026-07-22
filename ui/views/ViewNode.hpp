#include "SwarmIDE.hpp"
#include "glm/glm.hpp"
#include "glfw/glfw3.h"
#include "glad/glad.h"

namespace SwarmIDE::UI {

    /**
     * @class ViewNode
     * @brief Класс для отрисовки ноды.
     * @details Используемый паттерн проектирования наблюдатель.
     * 
     * @see Node::LogicNode
     */
    class ViewNode {
    public:

    private:
        glm::vec2 worldPos;       /**< @brief Положение ноды на поле */
    };
}