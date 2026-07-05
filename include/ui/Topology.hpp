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

        // Используем биты: 0001 (базовый), 0010 (модель), 0100 (память), 1000 (инструмент)
        // Дополнительный бит 0x10000 определяет Выход (Output), иначе Вход (Input)
        Input        = 0x01,
        Output       = 0x01 | 0x10, 
        
        ModelInput   = 0x02,
        ModelOutput  = 0x02 | 0x10,
        
        MemoryInput  = 0x04,
        MemoryOutput = 0x04 | 0x10,
        
        ToolInput    = 0x08,
        ToolOutput   = 0x08 | 0x10,

        Universal    = 0xFF 
    };

    // Конкретная точка контакта
    struct AnchorPin {  
        std::string id;           // Id пина
        PinType type;             // Тип пина
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