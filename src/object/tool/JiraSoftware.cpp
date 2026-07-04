#include "object/tool/JiraSoftware.hpp"
#include "ui/Shape/CircleShape.hpp"
#include "core/TextureManager.hpp"
#include "imgui.h" 

namespace SwarmIDE::Object::Tool {
    JiraSoftware::JiraSoftware(std::string name, glm::vec2 worldPos) 
    : BaseTool(name + std::to_string(count), worldPos) { 
        shape = std::make_unique<SwarmIDE::UI::Shape::CircleShape>(worldPos);

        std::vector<SwarmIDE::UI::AnchorPin> pins = {
            {"Out JiraSoftware", SwarmIDE::UI::PinType::ToolOutput, glm::vec2(0.5f, 0.0f), glm::vec2(0.0f, -1.0f)}
        };

        shape->SetLocalPins(pins);

        SwarmIDE::Core::TextureManager::getInstance().loadTexture("openai", "resources/icons/openai.png");
        this->iconKey = "openai"; // Тестовая иконка
    }

    void JiraSoftware::update() {
        //std::cout << "JiraSoftware" << count << std::endl;
    }

    std::optional<bool> JiraSoftware::drawUI(Camera& camera, glm::vec2 windowSize) {
        if (!shape) return std::nullopt;

        // Синхронизируем позицию (если она изменилась в логике агента)
        shape->SetPosition(this->worldPos); 

        // Вызываем отрисовку фигуры, передавая контекст и лямбду с начинкой агента
        bool ifDelete = shape->Draw(name, camera, fonts, windowSize, [this]() {
            /*
            // Вот ТУТ пишется то, что специфично ТОЛЬКО для JiraSoftware
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