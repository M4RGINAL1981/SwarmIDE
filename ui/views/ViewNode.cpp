#include "SwarmIDE.hpp"
#include "ui/views/ViewNode.hpp"
#include "ui/views/Shape/BaseShape.hpp"
#include "core/graph/GraphManager.hpp"
#include "core/engine/LoggingManager.hpp"

#include "ui/views/Shape/CircleShape.hpp"
#include "ui/views/Shape/RectangleShape.hpp"
#include "ui/views/Shape/DShape.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace SwarmIDE::UI {
    ViewNode::ViewNode(UUIDv4::UUID nodeID) {
        auto newNode = Core::GraphManager::getInstance().findNodeByID(nodeID);
        // Если логической ноды нет - ошибка
        if(!newNode) {
            Core::LoggingManager::Fatal("Node is not found. Node ID: ", nodeID);
            throw std::runtime_error("Node is not found");
        }
        this->nodeID = nodeID;
        worldPos = glm::vec2(0.0f, 0.0f);

        // Создание ViewPin
        auto& pins = newNode->GetPins();
        
        // 1. Создаем 4 корзины для сортировки
        std::vector<Core::LogicPin*> leftPins, rightPins, topPins, bottomPins;

        // 2. Раскидываем логические пины по правилам
        for(auto& [id, pin] : pins) {
            if(pin->direction == Core::Direction::Input) {
                if(pin->type == Core::PinType::DataStream) leftPins.push_back(pin.get());
                else bottomPins.push_back(pin.get());
            }
            else {
                if(pin->type == Core::PinType::DataStream) rightPins.push_back(pin.get());
                else topPins.push_back(pin.get());
            }
        }

        // 3. Вычисляем размеры ноды (nodeSize) на основе самых загруженных сторон
        float stepY = 50.0f; // Расстояние между пинами по вертикали
        float stepX = 50.0f; // Расстояние между пинами по горизонтали

        nodeSize = glm::vec2(
            stepX * (std::max(topPins.size(), bottomPins.size()) + 1), 
            stepY * (std::max(leftPins.size(), rightPins.size()) + 1)
        );

        // Минимальный размер, чтобы нода не схлопнулась, если пинов мало
        nodeSize.x = std::max(nodeSize.x, 100.0f);
        nodeSize.y = std::max(nodeSize.y, 100.0f);

        // 4. Генерация ViewPin с точными localPos

        // Левая сторона
        for(size_t i = 0; i < leftPins.size(); ++i) {
            auto pin = leftPins[i];
            // Распределяем равномерно по высоте левой грани
            float yPos = nodeSize.y / (leftPins.size() + 1) * (i + 1);
            glm::vec2 localPos(0.0f, yPos);
            glm::vec2 normal(-1.0f, 0.0f);
            
            viewPins[pin->GetID()] = (std::make_unique<ViewPin>(pin->GetID(), localPos, normal));
        }

        // Правая сторона
        for(size_t i = 0; i < rightPins.size(); ++i) {
            auto pin = rightPins[i];
            // Распределяем равномерно по высоте правой грани
            float yPos = nodeSize.y / (rightPins.size() + 1) * (i + 1);
            glm::vec2 localPos(nodeSize.x, yPos);
            glm::vec2 normal(1.0f, 0.0f);
            
            viewPins[pin->GetID()] = (std::make_unique<ViewPin>(pin->GetID(), localPos, normal));
        }

        // Вверхняя сторона
        for(size_t i = 0; i < topPins.size(); ++i) {
            auto pin = topPins[i];
            // Распределяем равномерно по ширине вверхней грани
            float xPos = nodeSize.x / (topPins.size() + 1) * (i + 1);
            glm::vec2 localPos(xPos, 0.0f);
            glm::vec2 normal(0.0f, -1.0f);
            
            viewPins[pin->GetID()] = (std::make_unique<ViewPin>(pin->GetID(), localPos, normal));
        }

        // Нижняя сторона
        for(size_t i = 0; i < bottomPins.size(); ++i) {
            auto pin = bottomPins[i];
            // Распределяем равномерно по ширине нижней грани
            float xPos = nodeSize.x / (bottomPins.size() + 1) * (i + 1);
            glm::vec2 localPos(xPos, nodeSize.y);
            glm::vec2 normal(0.0f, 1.0f);
            
            viewPins[pin->GetID()] = (std::make_unique<ViewPin>(pin->GetID(), localPos, normal));
        }

        // 5. Логика определения фигуры на основе топологии
        int totalPins = leftPins.size() + rightPins.size() + topPins.size() + bottomPins.size();

        if (totalPins == 1 && rightPins.size() == 1) {
            // 0 входов, 1 выход DataStream -> D-Образная фигура
            shape = std::make_unique<Shape::DShape>();
        }
        else if (totalPins == 1 && topPins.size() == 1) {
            // 0 входов, 1 выход не-DataStream -> Круг
            shape = std::make_unique<Shape::CircleShape>();
        }
        else {
            // Во всех остальных случаях -> Прямоугольник
            shape = std::make_unique<Shape::RectangleShape>();
        }

        // 6. Установка флагов по умолчанию
        isHovered = false;
        isSelected = false;
    }

    void ViewNode::Draw(ImDrawList* drawList, glm::vec2 screenPos, float zoom) {
        if (drawList == nullptr || shape == nullptr) return;

        // МАСШТАБИРУЕМ РАЗМЕР: Умножаем базовый размер ноды на зум камеры
        glm::vec2 scaledSize = nodeSize * zoom;

        state = Core::GraphManager::getInstance().findNodeByID(nodeID)->GetState();

        // Передаем scaledSize вместо статичного nodeSize
        shape->DrawBackground(drawList, screenPos, scaledSize, zoom, state, isHovered, isSelected);

        // Отрисовка пинов
        for(auto& [id, viewPin] : viewPins) {
            viewPin->UpdatePos(screenPos, zoom);
            viewPin->Draw(drawList, zoom);
        }   
    }

    ViewPin* ViewNode::findViewPin(UUIDv4::UUID pinID) {
        if(viewPins.contains(pinID)) return viewPins[pinID].get();
        return nullptr;
    }

    json ViewNode::serializeViewNode() const {
        json jsonNode;

        auto logicNode = Core::GraphManager::getInstance().findNodeByID(nodeID);

        jsonNode["worldPosX"] = worldPos.x;
        jsonNode["worldPosY"] = worldPos.y;

        jsonNode.update(logicNode->serializeNode());

        return jsonNode;
    }

    void ViewNode::deserializeViewNode(json jsonNode) {
        worldPos = glm::vec2(jsonNode["worldPosX"], jsonNode["worldPosY"]);
        nodeID = UUIDv4::UUID(jsonNode["id"].get<std::string>());

        // Заполняем данными логическую ноду
        auto logicNode = Core::GraphManager::getInstance().findNodeByID(nodeID);
        logicNode->deserializeNode(jsonNode);
    }
}