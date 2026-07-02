#include "object/model/Gemini.hpp"
#include "ui/Shape/CircleShape.hpp"
#include "core/TextureManager.hpp"
#include "imgui.h" 

namespace SwarmIDE::Object::Model {
    Gemini::Gemini(std::string name, glm::vec2 worldPos) 
    : BaseModel(name + std::to_string(count), worldPos) { 
        shape = std::make_unique<SwarmIDE::UI::Shape::CircleShape>(worldPos);
        SwarmIDE::Core::TextureManager::getInstance().loadTexture("gemini-512", "resources/icons/gemini-512.png");
        this->iconKey = "gemini-512";
    }

    void Gemini::update() {
        //std::cout << "Gemini " << count << std::endl;
    }

    void Gemini::drawUI(Camera& camera, glm::vec2 windowSize) {
        if (!shape) return;

        // Синхронизируем позицию (если она изменилась в логике агента)
        shape->SetPosition(this->worldPos); 

        // Вызываем отрисовку фигуры, передавая контекст и лямбду с начинкой агента
        shape->Draw(name, camera, fonts, windowSize, [this]() {
            /*
            // Вот ТУТ пишется то, что специфично ТОЛЬКО для Gemini
            ImGui::Text("Память: %s", name.c_str());
            if (ImGui::Button("Отправить запрос")) {
                // Логика агента
            }
            */
        }, this->iconKey);

        // Забираем обратно изменённую позицию (если пользователь перетащил окно мышкой)
        this->worldPos = shape->GetPosition();
    }

}