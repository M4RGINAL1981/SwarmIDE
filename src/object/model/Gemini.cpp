#include "object/model/Gemini.hpp"
#include "ui/Shape/CircleShape.hpp"
#include "core/TextureManager.hpp"
#include "ui/Topology.hpp"
#include "imgui.h" 

namespace SwarmIDE::Object::Model {
    Gemini::Gemini(std::string name, glm::vec2 worldPos) 
    : BaseModel(name + std::to_string(count), worldPos) { 
        shape = std::make_unique<SwarmIDE::UI::Shape::CircleShape>(worldPos);

        std::vector<SwarmIDE::UI::AnchorPin> pins = {
            {"Out Gemini", SwarmIDE::UI::PinType::ModelOutput, glm::vec2(0.5f, 0.0f), glm::vec2(0.0f, -1.0f)}
        };

        shape->SetLocalPins(pins);

        SwarmIDE::Core::TextureManager::getInstance().loadTexture("gemini-512", "resources/icons/gemini-512.png");
        this->iconKey = "gemini-512";
    }

    void Gemini::update() {
        //std::cout << "Gemini " << count << std::endl;
    }

    std::optional<bool> Gemini::drawUI(Camera& camera, glm::vec2 windowSize) {
        if (!shape) return std::nullopt;

        // Синхронизируем позицию (если она изменилась в логике агента)
        shape->SetPosition(this->worldPos); 

        // Вызываем отрисовку фигуры, передавая контекст и лямбду с начинкой агента
        bool ifDelete = shape->Draw(name, camera, fonts, windowSize, [this]() {
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
        
        return ifDelete;
    }

}