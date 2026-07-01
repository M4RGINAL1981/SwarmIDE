#include "object/memory/PostgreSQL.hpp"
#include "ui/Shape/CircleShape.hpp"
#include "imgui.h" 

namespace SwarmIDE::Object::Memory {
    PostgreSQL::PostgreSQL(std::string name, glm::vec2 worldPos) 
    : BaseMemory(name + std::to_string(count), worldPos) { 
        shape = std::make_unique<SwarmIDE::UI::Shape::CircleShape>(worldPos);
    }

    void PostgreSQL::update() {
        //std::cout << "PostgreSQL " << count << std::endl;
    }

    void PostgreSQL::drawUI(Camera& camera, glm::vec2 windowSize) {
        if (!shape) return;

        // Синхронизируем позицию (если она изменилась в логике агента)
        shape->SetPosition(this->worldPos); 

        // Вызываем отрисовку фигуры, передавая контекст и лямбду с начинкой агента
        shape->Draw(name, camera, fonts, windowSize, [this]() {
            // Вот ТУТ пишется то, что специфично ТОЛЬКО для PostgreSQL
            ImGui::Text("Память: %s", name.c_str());
            if (ImGui::Button("Отправить запрос")) {
                // Логика агента
            }
        });

        // Забираем обратно изменённую позицию (если пользователь перетащил окно мышкой)
        this->worldPos = shape->GetPosition();
    }

}