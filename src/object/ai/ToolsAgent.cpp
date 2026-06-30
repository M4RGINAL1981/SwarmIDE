#include "object/ai/ToolsAgent.hpp"
#include "ui/Shape/SquareShape.hpp"
#include "imgui.h"

namespace SwarmIDE::Object::AI {

    ToolsAgent::ToolsAgent(std::string name, glm::vec2 worldPos) 
    : BaseAgent(name + std::to_string(count), worldPos) {
        shape = std::make_unique<SwarmIDE::UI::Shape::SquareShape>(worldPos);
    }


    void ToolsAgent::render() {

    }

    void ToolsAgent::update() {
        //std::cout << "ConversationalAgent " << count << std::endl;
    }


    void ToolsAgent::drawUI(Camera& camera, glm::vec2 windowSize) {
        if (!shape) return;

        // Синхронизируем позицию (если она изменилась в логике агента)
        shape->SetPosition(this->worldPos); 

        // Вызываем отрисовку фигуры, передавая контекст и лямбду с начинкой агента
        shape->Draw(name, camera, fonts, windowSize, [this]() {
            // Вот ТУТ пишется то, что специфично ТОЛЬКО для ConversationalAgent
            ImGui::Text("Агент: %s", name.c_str());
            if (ImGui::Button("Отправить запрос")) {
                // Логика агента
            }
        });

        // Забираем обратно изменённую позицию (если пользователь перетащил окно мышкой)
        this->worldPos = shape->GetPosition();
    }

}