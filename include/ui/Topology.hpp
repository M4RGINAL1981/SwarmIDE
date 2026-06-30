#pragma once
#include "SwarmIDE.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <string>

namespace SwarmIDE::UI {

    // Тип пина
    enum class PinType {  
        Input,              // Вход            (Принимает результат работы другого агента)
        Output,             // Выход           (Отправляет результат работы текущего агента)
        Universal,          // Универсальный   (Этот пин одновремено является и Input, и Output)
        ResourceInput,      // Вход Ресурсов   (Принимает из инструментария)
        ResourceOutput,     // Выход Ресурвоу  (Отправляет в инструментарий)
    };

    // Конкретная точка контакта
    struct AnchorPin {  
        std::string id;           // Id пина
        PinType type;             // Тип пина
        glm::vec2 localPos;       // Смешение от центра/угла фигуры
        glm::vec2 localNormal;    // Вектор направления вылета линни
    };

    // Пин пересчитаный в глобальные координаты
    struct TransformedPin{        
        glm::vec2 globalPos;      // Смешение от центра/угла фигуры (глобальный)
        glm::vec2 globalNormal;   // Вектор направлени вылета линии (глобальный)
    };
}