#include "object/ai/PlanExecuteAgent.hpp"
#include "ui/Shape/SquareShape.hpp"
#include "ui/Shape/RectangleShape.hpp"
#include "imgui.h"

namespace SwarmIDE::Object::AI {

    PlanExecuteAgent::PlanExecuteAgent(std::string name, glm::vec2 worldPos) 
    : BaseAgent(name + std::to_string(count), worldPos) {
        shape = std::make_unique<SwarmIDE::UI::Shape::RectangleShape>(worldPos);

        std::vector<SwarmIDE::UI::AnchorPin> pins = {
            {"0", SwarmIDE::UI::PinType::ModelInput,  glm::vec2(0.25f, 1.0f), glm::vec2(-1.0f, 1.0f)}, 
            {"1", SwarmIDE::UI::PinType::MemoryInput, glm::vec2( 0.5f, 1.0f), glm::vec2( 0.0f, 1.0f)},
            {"2", SwarmIDE::UI::PinType::ToolInput,   glm::vec2(0.75f, 1.0f), glm::vec2( 1.0f, 1.0f)}, 
            {"3", SwarmIDE::UI::PinType::Output,      glm::vec2( 1.0f, 0.5f), glm::vec2( 1.0f, 0.0f)},  
            {"4", SwarmIDE::UI::PinType::Input,       glm::vec2( 0.0f, 0.5f), glm::vec2(-1.0f, 0.0f)}
        };

        shape->SetLocalPins(pins);
    }

    void PlanExecuteAgent::render() {

    }

    void PlanExecuteAgent::update() {
        //std::cout << "ConversationalAgent " << count << std::endl;
    }

    std::optional<bool> PlanExecuteAgent::drawUI(Camera& camera, glm::vec2 windowSize) {
        if (!shape) return std::nullopt;

        // Синхронизируем позицию (если она изменилась в логике агента)
        shape->SetPosition(this->worldPos); 

        // Вызываем отрисовку фигуры, передавая контекст и лямбду с начинкой агента
        bool ifDelete = shape->Draw(name, camera, fonts, windowSize, [this]() {
            // Вот ТУТ пишется то, что специфично ТОЛЬКО для PlanExecuteAgent
            ImGui::Text("Агент: %s", name.c_str());
            if (ImGui::Button("Отправить запрос")) {
                // Логика агента
            }
        }, std::nullopt);

        // Забираем обратно изменённую позицию (если пользователь перетащил окно мышкой)
        this->worldPos = shape->GetPosition();
        
        return ifDelete;
    }

}
