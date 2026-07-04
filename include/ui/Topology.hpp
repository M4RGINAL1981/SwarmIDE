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
        Input,              // Вход  Агента           (Пин принимает результат работы другого агента)
        Output,             // Выход Агента           (Пин отправляет результат работы текущего агента)
        ModelInput,         // Вход  Модели           (Пин принимает модели)
        ModelOutput,        // Выход Модели           (Пин отправляет модель в агента)
        MemoryInput,        // Вход  Памяти           (Пин принимает память)
        MemoryOutput,       // Выход Памяти           (Пин отправляет память в агента)
        ToolInput,          // Вход  Инструментов     (Пин принимает инструмент)
        ToolOutput,         // Выход Инструментов     (Пин отправляет инструмент в агента)

        Universal,          // Универсальный          (Этот пин одновремено является любым пином)
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