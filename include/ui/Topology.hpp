#pragma once
#include "SwarmIDE.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <string>

namespace SwarmIDE::UI {

    // Конкретная точка контакта
    struct AnchorPin {  
        std::string id;           // Id пина
        // PinType type;          Тип пина (Нужно было вырезать)
        glm::vec2 localPos;       // Смешение от центра/угла фигуры
        glm::vec2 localNormal;    // Вектор направления вылета линни

        int maxConnections;       // Максимальное кол-во связей с данный пином
        int connectionCount = 0;
    };

    // Пин пересчитаный в глобальные координаты
    struct TransformedPin{        
        glm::vec2 globalPos;      // Смешение от центра/угла фигуры (глобальный)
        glm::vec2 globalNormal;   // Вектор направлени вылета линии (глобальный)
    };
}