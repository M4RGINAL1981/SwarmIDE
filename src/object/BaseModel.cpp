#include "object/BaseModel.hpp"
#include "ui/Shape/CircleShape.hpp"
#include "core/TextureManager.hpp"
#include "ui/Topology.hpp"
#include "pfd/portable-file-dialogs.h"
#include "imgui.h" 
#include "exception"

namespace SwarmIDE::Object {
    BaseModel::BaseModel(std::string name, glm::vec2 worldPos, SwarmIDE::Core::Model* model) : BaseObject(name + std::to_string(count), worldPos) { 
        
        if(model == nullptr) {
            std::cerr << "Model is nullptr!!!" << "\n";
            std::terminate();
        }
        this->model = model;
        
        shape = std::make_unique<SwarmIDE::UI::Shape::CircleShape>(worldPos);

        std::vector<SwarmIDE::UI::AnchorPin> pins = {
            {"Out Model", SwarmIDE::UI::PinType::ModelOutput, glm::vec2(0.5f, 0.0f), glm::vec2(0.0f, -1.0f), 1}
        };

        shape->SetLocalPins(pins);
        //shape->SetColor(Colors::ToImU32(Colors::BLACK));

        SwarmIDE::Core::TextureManager::getInstance().loadTexture(model->name, model->iconPath);
        this->iconKey = model->name;
    }

    void BaseModel::update() {
        //std::cout << "BaseModel " << count << std::endl;
    }

    std::optional<bool> BaseModel::drawUI(Camera& camera, glm::vec2 windowSize) {
        if (!shape) return std::nullopt;

        // Синхронизируем позицию (если она изменилась в логике агента)
        shape->SetPosition(this->worldPos); 

        // Вызываем отрисовку фигуры, передавая контекст и лямбду с начинкой агента
        bool ifDelete = shape->Draw(name, camera, fonts, windowSize, [this]() {
            /*
            // Вот ТУТ пишется то, что специфично ТОЛЬКО для BaseModel
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