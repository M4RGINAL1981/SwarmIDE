#include "SwarmIDE.hpp"
#include "ui/canvas/CanvasInputController.hpp"
#include "ui/canvas/CanvasManager.hpp"

namespace SwarmIDE::UI {

    CanvasInputController& CanvasInputController::getInstance() {
        static CanvasInputController instance;
        return instance;
    }

    void CanvasInputController::ProcessInput() {
        if (!canvas_manager || !camera) return;
        
        ImGuiIO& io = ImGui::GetIO();
        glm::vec2 mousePos = glm::vec2(io.MousePos.x, io.MousePos.y);

        // Обработка кликов и клавиатуры
        HandleMouseLeftClick(mousePos);
        HandleMouseRightClick(mousePos);
        HandleKeyboardInputs();
    }

    void CanvasInputController::HandleMouseLeftClick(glm::vec2 mousePos) {
        ImGuiIO& io = ImGui::GetIO();
        // Сохраняем коодринаты мыши
        activeLinkContext.currentMouseWorldPos = camera->screenToWorld(mousePos, canvas_manager->GetWindowSize());

        // Объекты, что были выбраны пользоваелем (Визуальный пин и нода)
        ViewPin* hoveredViewPin = nullptr;
        // Будем фиксировать ID ноды, которой принадлежит пин
        UUIDv4::UUID hoveredPinParentNodeID; 
        ViewNode* hoveredViewNode = nullptr;

        auto& viewNodes = canvas_manager->getViewNodes();

        // Указываем, что прошлые ноды и пины должны перестать быть наведены.
        if (lastHoveredPin != std::nullopt && NodehasLastHoveredPin != std::nullopt) {
            if (auto* pastNode = canvas_manager->findViewNodeByID(NodehasLastHoveredPin.value())) {
                if (auto* pastPin = pastNode->findViewPin(lastHoveredPin.value())) {
                    pastPin->SetHovered(false);
                }
            }
        }

        if (lastHoveredNode != std::nullopt) {
            if (auto* pastNode = canvas_manager->findViewNodeByID(lastHoveredNode.value())) {
                pastNode->SetHovered(false);
            }
        }

        // Если событие мыши - Простой
        if(activeLinkContext.state == MouseEvents::IDLE) {   
            // Проходимся по всем визуальным нодам
            for(auto& [viewNodeID, viewNode] : viewNodes) {
                auto& viewPins = viewNode->GetViewPins();

                // Проходимся по всем визуальны пинам
                for(auto& [viewPinID, viewPin] : viewPins) {
                    if(viewPin->IsPointInside(mousePos, camera->zoomFactor)) {
                        viewPin->SetHovered(true);
                        hoveredViewPin = viewPin.get();
                        // Запоминаем ID родительской нodы пина
                        hoveredPinParentNodeID = viewNodeID; 
                        
                        // Запоминаем ID визуального пина, для снятия выделения и ID ноды, которая имеет данный пин.
                        lastHoveredPin = hoveredViewPin->GetPinID();
                        NodehasLastHoveredPin = viewNodeID;
                        break; 
                    }
                }

                // Если пользователь не выбрал пин, проверям выбрал ли пользователь ноду.
                if(hoveredViewPin == nullptr) {
                    glm::vec2 screenPos = camera->worldToScreen(viewNode->worldPos, canvas_manager->GetWindowSize());
                    if(viewNode->shape->isPointInside(mousePos, screenPos, viewNode->nodeSize, camera->zoomFactor)) {
                        viewNode->isHovered = true;
                        hoveredViewNode = viewNode.get();
                        lastHoveredNode = hoveredViewNode->nodeID;
                        break;
                    }
                }
            }

            // Если пользователь указывает на пин
            if(hoveredViewPin != nullptr) {
                // Если пользователь кликнул на пин
                if(ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                    // Указываем событие мыши, как проведение связи
                    activeLinkContext.state = MouseEvents::DRAGGING_WIRE;
                    activeLinkContext.startNodeID = hoveredPinParentNodeID;
                    activeLinkContext.startPinID = hoveredViewPin->GetPinID();
                }
            }
            // Если пользователь выбрал ноду
            else if(hoveredViewNode != nullptr) {
                // Если пользователь кликнул на ноду
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                    // Указываем событие мыши, как перетаскивание ноды
                    activeLinkContext.state = MouseEvents::DRAGGING_NODE;
                    activeLinkContext.grabberNodeID = hoveredViewNode->nodeID;
                    activeLinkContext.initialNodePos = hoveredViewNode->GetWorldPos();

                    // Сохраняем ID ноды, чтобы пользователь мог удалить её
                    selectedNodeID = hoveredViewNode->nodeID;
                }
            }
        }
        // Если событие мыши - перетаскивание ноды
        else if(activeLinkContext.state == MouseEvents::DRAGGING_NODE) {
            if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                // Находим смещение мыши (Дельту)
                glm::vec2 worldDelta = glm::vec2(io.MouseDelta.x, io.MouseDelta.y) / camera->zoomFactor;
                ViewNode* draggedNode = canvas_manager->findViewNodeByID(activeLinkContext.grabberNodeID);

                if (draggedNode) {
                    /** @todo Здесь должна быть команда по перемещению ноды. CmdMoveNode */

                    // Смешаем ноду на дельту
                    draggedNode->MoveBy(worldDelta);
                }
            }
            
            // Если пользователь отпустил мышь - Reset (Сброс события мыши в IDLE)
            if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                activeLinkContext.Reset();
            }
        }
        // Если собитие мыши - проведение связи
        else if(activeLinkContext.state == MouseEvents::DRAGGING_WIRE) {
            canvas_manager->SetTempWire(activeLinkContext.startNodeID, activeLinkContext.startPinID, activeLinkContext.currentMouseWorldPos);

            // Проходимся циклом по всем визуальным нодам
            for(auto& [viewNodeID, viewNode] : viewNodes) {
                auto& viewPins = viewNode->GetViewPins();
                // Проходимся циклом по всем визуальным пинам
                for(auto& [viewPinID, viewPin] : viewPins) {
                    // Если пользователь указывает на новый пин (Второй). То есть хочет создать связь
                    if(viewPin->IsPointInside(mousePos, camera->zoomFactor)) {
                        // Сохраняем второй теперь уже выбранный пин
                        viewPin->SetHovered(true);
                        hoveredViewPin = viewPin.get();
                        hoveredPinParentNodeID = viewNodeID;
                        break;
                    }
                }
                if(hoveredViewPin) break;
            }

            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                try {
                    // Если выбранный пин (второй) существует, и начальный пин (первый) не равен выбранному — строим связь.
                    if (hoveredViewPin != nullptr && hoveredViewPin->GetPinID() != activeLinkContext.startPinID) {
                        auto* startViewNode = canvas_manager->findViewNodeByID(activeLinkContext.startNodeID);
                        auto* startViewPin = (startViewNode != nullptr) ? startViewNode->findViewPin(activeLinkContext.startPinID) : nullptr;

                        auto* logicStartPin = (startViewPin != nullptr)
                            ? Core::GraphManager::getInstance().findPinGlobaly(activeLinkContext.startPinID)
                            : nullptr;
                        auto* logicHoveredPin = Core::GraphManager::getInstance().findPinGlobaly(hoveredViewPin->GetPinID());

                        if (logicStartPin != nullptr && logicHoveredPin != nullptr) {

                            if (logicStartPin->nodeID == logicHoveredPin->nodeID) {
                                Core::LoggingManager::Warning("Connection dropped: source and target pin belong to the same node.");
                            }
                            else if (logicStartPin->direction == logicHoveredPin->direction) {
                                Core::LoggingManager::Warning("Connection dropped: pins have the same direction.");
                            }
                            else if (logicStartPin->dataType != logicHoveredPin->dataType || logicStartPin->type != logicHoveredPin->type) {
                                Core::LoggingManager::Warning("Connection dropped: pin data/type mismatch.");
                            }
                            else if (logicStartPin->type != logicHoveredPin->type) {
                                Core::LoggingManager::Warning("Connection dropped: pin types do not match.");
                            }
                            else if (logicStartPin->maxConnections != -1 && logicStartPin->GetCurrentConnections() >= logicStartPin->maxConnections) {
                                Core::LoggingManager::Warning("Connection dropped: source pin has reached its maximum connections.");
                            }
                            else if (logicHoveredPin->maxConnections != -1 && logicHoveredPin->GetCurrentConnections() >= logicHoveredPin->maxConnections) {
                                Core::LoggingManager::Warning("Connection dropped: target pin has reached its maximum connections.");
                            }
                            else {
                                UUIDv4::UUID fromNodeID = (logicHoveredPin->direction == Core::Direction::Input) ? activeLinkContext.startNodeID : hoveredPinParentNodeID;
                                UUIDv4::UUID fromPinID = (logicHoveredPin->direction == Core::Direction::Input) ? activeLinkContext.startPinID : hoveredViewPin->GetPinID();

                                UUIDv4::UUID toNodeID = (logicHoveredPin->direction == Core::Direction::Output) ? activeLinkContext.startNodeID : hoveredPinParentNodeID;
                                UUIDv4::UUID toPinID = (logicHoveredPin->direction == Core::Direction::Output) ? activeLinkContext.startPinID : hoveredViewPin->GetPinID();

                                auto cmd_createConnection = std::make_unique<Command::CmdCreateConnection>(fromNodeID, fromPinID, toNodeID, toPinID);
                                Command::CommandBus::getInstance().PushAndExecute(std::move(cmd_createConnection));
                            }
                        }
                        else {
                            Core::LoggingManager::Warning("Connection dropped: unable to resolve logical pins for the selected view pins.");
                        }
                    }
                }
                catch (const std::exception& ex) {
                    Core::LoggingManager::Error("Connection creation path failed: ", ex.what());
                }
                catch (...) {
                    Core::LoggingManager::Error("Connection creation path failed with unknown exception.");
                }
                // Сбрасываем состояние
                activeLinkContext.Reset();
                canvas_manager->ClearTempWire();
            }
        }
    }

    void CanvasInputController::HandleMouseRightClick(glm::vec2 mousePos) {
        /** @todo Нужно будет доделать метод, сейчас тут нет реализации, так как нет контекстного меню. */
    }

    void CanvasInputController::HandleKeyboardInputs() {
        ImGuiIO& io = ImGui::GetIO();

        // Проверяем, что зажат Ctrl (поддерживает и левый, и правый)
        bool ctrlPressed = io.KeyCtrl; 

        // Отлавливаем Ctrl + Z
        if (ctrlPressed && ImGui::IsKeyPressed(ImGuiKey_Z, false)) {
            // Вызываем вашу команду отмены действия
            command_bus.Undo();
        }

        // Отлавливаем Ctrl + Y
        if (ctrlPressed && ImGui::IsKeyPressed(ImGuiKey_Y, false)) {
            command_bus.Redo();
        }

        if(ImGui::IsKeyPressed(ImGuiKey_Delete, false)) {
            if(selectedNodeID != std::nullopt) {
                auto cmd_deleteNode = std::make_unique<Command::CmdDeleteNode>(selectedNodeID);
                command_bus.PushAndExecute(std::move(cmd_deleteNode));

                // Сбрасываем ноду, чтобы не удалять одну и ту же по нескольку раз
                selectedNodeID = std::nullopt;
            }
        }
    }
}