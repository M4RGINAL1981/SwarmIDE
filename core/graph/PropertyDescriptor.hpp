#pragma once
#include "SwarmIDE.hpp"
#include <string>
#include <functional>
#include <vector>

namespace SwarmIDE::Core {

    /**
     * @class PropertyKind
     * @brief Перечисления указывающая на тип редактируемых полей.
     */
    enum class PropertyKind { Text, MultilineText, Int, Float, Bool, Enum, ModelRef };

    /**
     * @struct PropertyDescriptor
     * @brief Система описания свойств ноды.
     */
    struct PropertyDescriptor {
        std::string  key;                       // ключ в JSON и в инспекторе, напр. "systemInstruction"
        std::string  label;                     // подпись в UI
        PropertyKind kind;
        std::function<DataVariant()>            get;
        std::function<void(const DataVariant&)> set;
        std::vector<std::string> enumOptions;   // для Enum / ModelRef
        bool readOnly = false;
    };
}