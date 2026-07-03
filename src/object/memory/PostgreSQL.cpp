#include "object/memory/PostgreSQL.hpp"
#include "ui/Shape/CircleShape.hpp"
#include "core/TextureManager.hpp"
#include "imgui.h" 

namespace SwarmIDE::Object::Memory {
    PostgreSQL::PostgreSQL(std::string name, glm::vec2 worldPos) 
    : BaseMemory(name + std::to_string(count), worldPos) { 
        shape = std::make_unique<SwarmIDE::UI::Shape::CircleShape>(worldPos);
        SwarmIDE::Core::TextureManager::getInstance().loadTexture("postgres_icon", "resources/icons/postgres.png");
        this->iconKey = "postgres_icon";
    }

    void PostgreSQL::update() {
        //std::cout << "PostgreSQL " << count << std::endl;
    }

    std::optional<bool> PostgreSQL::drawUI(Camera& camera, glm::vec2 windowSize) {
        if (!shape) return std::nullopt;

        // Синхронизируем позицию (если она изменилась в логике агента)
        shape->SetPosition(this->worldPos); 

        // Вызываем отрисовку фигуры, передавая контекст и лямбду с начинкой агента
        bool ifDelete = shape->Draw(name, camera, fonts, windowSize, [this]() {
            /*
            // Вот ТУТ пишется то, что специфично ТОЛЬКО для PostgreSQL
            ImGui::Text("Память: %s", name.c_str());
            if (ImGui::Button("Отправить запрос")) {
                // Логика агента
            }
            */
        }, this->iconKey);

        // Забираем обратно изменённую позицию (если пользователь перетащил окно мышкой)
        this->worldPos = shape->GetPosition();
        
        return &ifDelete;
    }

}