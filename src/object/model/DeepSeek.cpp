#include "object/model/DeepSeek.hpp"
#include "ui/Shape/CircleShape.hpp"
#include "core/TextureManager.hpp"
#include "ui/Topology.hpp"
#include "imgui.h" 

namespace SwarmIDE::Object::Model {
    DeepSeek::DeepSeek(std::string name, glm::vec2 worldPos) : BaseModel(name + std::to_string(count), worldPos) { 
        shape = std::make_unique<SwarmIDE::UI::Shape::CircleShape>(worldPos);

        std::vector<SwarmIDE::UI::AnchorPin> pins = {
            {"Out DeepSeek", SwarmIDE::UI::PinType::ModelOutput, glm::vec2(0.5f, 0.0f), glm::vec2(0.0f, -1.0f), 1}
        };

        shape->SetLocalPins(pins);
        //shape->SetColor(Colors::ToImU32(Colors::BLACK));

        SwarmIDE::Core::TextureManager::getInstance().loadTexture("deepseek-512", "resources/icons/deepseek.png");
        this->iconKey = "deepseek-512";
    }

    void DeepSeek::update() {
        //std::cout << "DeepSeek " << count << std::endl;
    }

    std::optional<bool> DeepSeek::drawUI(Camera& camera, glm::vec2 windowSize) {
        if (!shape) return std::nullopt;

        // Синхронизируем позицию (если она изменилась в логике агента)
        shape->SetPosition(this->worldPos); 

        // Вызываем отрисовку фигуры, передавая контекст и лямбду с начинкой агента
        bool ifDelete = shape->Draw(name, camera, fonts, windowSize, [this]() {
            /*
            // Вот ТУТ пишется то, что специфично ТОЛЬКО для DeepSeek
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