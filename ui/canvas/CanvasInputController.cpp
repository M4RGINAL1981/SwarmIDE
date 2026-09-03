#include "SwarmIDE.hpp"
#include "ui/canvas/CanvasInputController.hpp"
#include "ui/canvas/CanvasManager.hpp"
#include "ui/command/CmdNodeOperations.hpp"
#include "ui/command/CmdBatch.hpp"
#include "ui/canvas/ContextMenuManager.hpp"

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
        activeLinkContext.currentMouseWorldPos = camera->screenToWorld(mousePos, canvas_manager->GetWindowSize());

        ViewPin* hoveredViewPin = nullptr;
        UUIDv4::UUID hoveredPinParentNodeID; 
        ViewNode* hoveredViewNode = nullptr;

        auto& viewNodes = canvas_manager->getViewNodes();

        // Снимаем подсвечивание наведения (Hover)
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

        if(activeLinkContext.state == MouseEvents::IDLE) {   
            for(auto& [viewNodeID, viewNode] : viewNodes) {
                auto& viewPins = viewNode->GetViewPins();

                for(auto& [viewPinID, viewPin] : viewPins) {
                    if(viewPin->IsPointInside(mousePos, camera->zoomFactor)) {
                        viewPin->SetHovered(true);
                        hoveredViewPin = viewPin.get();
                        hoveredPinParentNodeID = viewNodeID; 
                        lastHoveredPin = hoveredViewPin->GetPinID();
                        NodehasLastHoveredPin = viewNodeID;
                        break; 
                    }
                }

                if(hoveredViewPin == nullptr) {
                    glm::vec2 screenPos = camera->worldToScreen(viewNode->GetWorldPos(), canvas_manager->GetWindowSize());
                    if(viewNode->shape->isPointInside(mousePos, screenPos, viewNode->nodeSize, camera->zoomFactor)) {
                        viewNode->isHovered = true;
                        hoveredViewNode = viewNode.get();
                        lastHoveredNode = hoveredViewNode->nodeID;
                        break;
                    }
                }
            }

            // Клик по холсту (в пустое место) -> Старт рамки выделения
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && hoveredViewPin == nullptr && hoveredViewNode == nullptr) {
                bool isModifierPressed = io.KeyCtrl || io.KeyShift;

                if (!isModifierPressed) {
                    canvas_manager->ClearSelection();
                    for (auto& [id, node] : viewNodes) {
                        node->SetSelected(false);
                    }
                }

                activeLinkContext.state = MouseEvents::DRAGGING_SELECTION;
                selectionStartScreenPos = mousePos;
                selectionCurrentScreenPos = mousePos;
                initialSelectedNodesBeforeBox = canvas_manager->GetSelectedNodesID();
            }

            // Клик по пину
            else if(hoveredViewPin != nullptr) {
                if(ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                    activeLinkContext.state = MouseEvents::DRAGGING_WIRE;
                    activeLinkContext.startNodeID = hoveredPinParentNodeID;
                    activeLinkContext.startPinID = hoveredViewPin->GetPinID();
                }
            }
            // Клик по ноде
            else if(hoveredViewNode != nullptr) {
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                    bool isModifierPressed = io.KeyCtrl || io.KeyShift;

                    if (isModifierPressed) {
                        // Тоглим выделение конкретной ноды
                        if (canvas_manager->IsNodeSelected(hoveredViewNode->nodeID)) {
                            canvas_manager->DeselectNode(hoveredViewNode->nodeID);
                            hoveredViewNode->SetSelected(false);
                        } else {
                            hoveredViewNode->SetSelected(true);
                            canvas_manager->SelectNode(hoveredViewNode->nodeID);
                        }
                    } else {
                        // Если кликнули по ноде, которая не была выделена — делаем её единственной выделенной
                        if (!canvas_manager->IsNodeSelected(hoveredViewNode->nodeID)) {
                            canvas_manager->ClearSelection();
                            for (auto& [id, node] : viewNodes) {
                                node->SetSelected(false);
                            }
                            hoveredViewNode->SetSelected(true);
                            canvas_manager->SelectNode(hoveredViewNode->nodeID);
                        }
                    }

                    // Переходим в режим перетаскивания всех выбранных нод
                    activeLinkContext.state = MouseEvents::DRAGGING_NODE;
                    activeLinkContext.initialNodePositions.clear();

                    for (const auto& nodeID : canvas_manager->GetSelectedNodesID()) {
                        if (auto* node = canvas_manager->findViewNodeByID(nodeID)) {
                            activeLinkContext.initialNodePositions[nodeID] = node->GetWorldPos();
                        }
                    }
                }
            }
        }
        // Растягивание рамки выделения
        else if (activeLinkContext.state == MouseEvents::DRAGGING_SELECTION) {
            selectionCurrentScreenPos = mousePos;

            // Вычисляем прямоугольник выделения в экранных координатах
            glm::vec2 selectMinScreen = glm::min(selectionStartScreenPos, selectionCurrentScreenPos);
            glm::vec2 selectMaxScreen = glm::max(selectionStartScreenPos, selectionCurrentScreenPos);

            // Передаем данные в CanvasManager для отрисовки
            canvas_manager->SetSelectionRect(selectMinScreen, selectMaxScreen, true);

            // Переводим границы рамки в мировые координаты для проверки коллизий нод
            glm::vec2 w1 = camera->screenToWorld(selectMinScreen, canvas_manager->GetWindowSize());
            glm::vec2 w2 = camera->screenToWorld(selectMaxScreen, canvas_manager->GetWindowSize());
            glm::vec2 selectMinWorld = glm::min(w1, w2);
            glm::vec2 selectMaxWorld = glm::max(w1, w2);

            bool isModifierPressed = io.KeyCtrl || io.KeyShift;

            // Проверяем пересечение AABB прямоугольника рамки с каждой нодой
            for (auto& [nodeID, viewNode] : viewNodes) {
                glm::vec2 nodeMinWorld = viewNode->GetWorldPos();
                glm::vec2 nodeMaxWorld = nodeMinWorld + viewNode->nodeSize;

                bool intersects = !(nodeMaxWorld.x < selectMinWorld.x || nodeMinWorld.x > selectMaxWorld.x ||
                                    nodeMaxWorld.y < selectMinWorld.y || nodeMinWorld.y > selectMaxWorld.y);

                bool wasSelectedBefore = initialSelectedNodesBeforeBox.contains(nodeID);

                if (isModifierPressed) {
                    if (wasSelectedBefore || intersects) {
                        canvas_manager->SelectNode(nodeID);
                        viewNode->SetSelected(true);
                    } else {
                        canvas_manager->DeselectNode(nodeID);
                        viewNode->SetSelected(false);
                    }
                } else {
                    if (intersects) {
                        canvas_manager->SelectNode(nodeID);
                        viewNode->SetSelected(true);
                    } else {
                        canvas_manager->DeselectNode(nodeID);
                        viewNode->SetSelected(false);
                    }
                }
            }

            // Завершение выделения по отпусканию LKM
            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                canvas_manager->ClearSelectionRect();
                initialSelectedNodesBeforeBox.clear();
                activeLinkContext.Reset();
            }
        }
        // Завершение перетаскивания группы нод
        else if(activeLinkContext.state == MouseEvents::DRAGGING_NODE) {
            
            // ДОБАВЛЕНО: Визуально сдвигаем ноды при удерживании ЛКМ и движении
            if (ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.0f)) {
                // Конвертируем дельту экрана в мировую
                glm::vec2 dragDeltaWorld = glm::vec2(io.MouseDelta.x, io.MouseDelta.y) / camera->zoomFactor;
                
                for (const auto& nodeID : canvas_manager->GetSelectedNodesID()) {
                    if (auto* node = canvas_manager->findViewNodeByID(nodeID)) {
                        node->MoveBy(dragDeltaWorld);
                    }
                }
            }

            // Отпускание ЛКМ: фиксация позиции и создание команд
            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                auto batchCmd = std::make_unique<Command::CmdBatch>();

                for (const auto& [nodeID, oldPos] : activeLinkContext.initialNodePositions) {
                    if (auto* node = canvas_manager->findViewNodeByID(nodeID)) {
                        glm::vec2 finalPos = node->GetWorldPos();
                        if (finalPos != oldPos) {
                            glm::vec2 delta = finalPos - oldPos;
                            
                            // ВАЖНО: Отменяем визуальный сдвиг, так как PushAndExecute 
                            // вызовет Execute(), который сам сместит ноду через MoveBy(delta).
                            // Без этого нода улетит на двойное расстояние.
                            node->MoveBy(-delta);
                            
                            batchCmd->AddCommand(std::make_unique<Command::CmdMoveNode>(nodeID, delta));
                        }
                    }
                }

                if (!batchCmd->IsEmpty()) command_bus.PushAndExecute(std::move(batchCmd));
                activeLinkContext.Reset();
            }
        }
        // Завершение создания связи
        else if(activeLinkContext.state == MouseEvents::DRAGGING_WIRE) {
            canvas_manager->SetTempWire(activeLinkContext.startNodeID, activeLinkContext.startPinID, activeLinkContext.currentMouseWorldPos);

            for(auto& [viewNodeID, viewNode] : viewNodes) {
                auto& viewPins = viewNode->GetViewPins();
                for(auto& [viewPinID, viewPin] : viewPins) {
                    if(viewPin->IsPointInside(mousePos, camera->zoomFactor)) {
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
                                command_bus.PushAndExecute(std::move(cmd_createConnection));
                            }
                        }
                    }
                }
                catch (const std::exception& ex) {
                    Core::LoggingManager::Error("Connection creation path failed: ", ex.what());
                }

                activeLinkContext.Reset();
                canvas_manager->ClearTempWire();
            }
        }
    } // CanvasInputController::HandleMouseLeftClick(glm::vec2 mousePos)

    void CanvasInputController::HandleMouseRightClick(glm::vec2 mousePos) {
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
            std::optional<UUIDv4::UUID> hoveredNodeID = std::nullopt;
            
            // Ищем, не кликнули ли мы по какой-то ноде
            for (auto& [id, viewNode] : canvas_manager->getViewNodes()) {
                glm::vec2 screenPos = camera->worldToScreen(viewNode->GetWorldPos(), canvas_manager->GetWindowSize());
                if (viewNode->shape->isPointInside(mousePos, screenPos, viewNode->nodeSize, camera->zoomFactor)) {
                    hoveredNodeID = id;
                    break;
                }
            }
            
            // Вызываем меню. Если hoveredNodeID пустой, откроются настройки холста
            ContextMenuManager::getInstance().RequestMenu(mousePos, hoveredNodeID);
        }
    }

    void CanvasInputController::HandleKeyboardInputs() {
        ImGuiIO& io = ImGui::GetIO();
        if (io.WantTextInput) return; 

        bool ctrlPressed = io.KeyCtrl;

        if (ctrlPressed && ImGui::IsKeyPressed(ImGuiKey_Z, false)) command_bus.Undo();
        if (ctrlPressed && ImGui::IsKeyPressed(ImGuiKey_Y, false)) command_bus.Redo();

        const auto& selectedNodes = canvas_manager->GetSelectedNodesID();

        // Ctrl + D (Групповое дублирование со связями)
        if (ctrlPressed && ImGui::IsKeyPressed(ImGuiKey_D, false)) {
            if (!selectedNodes.empty()) {
                auto batchCmd = std::make_unique<Command::CmdBatch>();
                
                // 1. Словари для связи старых ID с новыми
                std::unordered_map<UUIDv4::UUID, UUIDv4::UUID> oldToNewNode;
                std::unordered_map<UUIDv4::UUID, UUIDv4::UUID> oldToNewPin;

                // 2. Копируем ноды (тут тебе потребуется метод в Core::GraphManager, который дублирует ноду и возвращает ее новую копию)
                for (const auto& oldNodeID : selectedNodes) {
                    // ПСЕВДОКОД: auto newNode = GraphManager.CloneNode(oldNodeID);
                    // ПСЕВДОКОД: oldToNewNode[oldNodeID] = newNode->GetID();
                    
                    // Запоминаем соответствие старых пинов новым (например, по их типу и направлению)
                    // ПСЕВДОКОД: oldToNewPin[oldPinID] = newPinID;
                    
                    // Сдвигаем новую ноду визуально, чтобы она не сливалась со старой
                    // batchCmd->AddCommand(MoveNode...);
                }

                // 3. Ищем связи между выделенными нодами
                auto& connManager = Core::ConnectionManager::getInstance();
                for (const auto& [connID, conn] : connManager.getAllConnections()) {
                    // Если ОБЕ ноды из этого соединения были скопированы (они есть в нашем словаре)
                    if (oldToNewNode.contains(conn.fromNodeID) && oldToNewNode.contains(conn.toNodeID)) {
                        
                        UUIDv4::UUID newFromNode = oldToNewNode[conn.fromNodeID];
                        UUIDv4::UUID newToNode = oldToNewNode[conn.toNodeID];
                        
                        UUIDv4::UUID newFromPin = oldToNewPin[conn.fromPinID];
                        UUIDv4::UUID newToPin = oldToNewPin[conn.toPinID];

                        // Создаем команду на прокладку провода между новыми нодами!
                        batchCmd->AddCommand(std::make_unique<Command::CmdCreateConnection>(
                            newFromNode, newFromPin, newToNode, newToPin
                        ));
                    }
                }

                if (!batchCmd->IsEmpty()) command_bus.PushAndExecute(std::move(batchCmd));
            }
        }

        if (ctrlPressed && ImGui::IsKeyPressed(ImGuiKey_A, false)) {
            for (auto& [id, viewNode] : canvas_manager->getViewNodes()) { 
                canvas_manager->SelectNode(id);
                viewNode->SetSelected(true);
            }
        }

        // Delete (Групповое удаление)
        if (ImGui::IsKeyPressed(ImGuiKey_Delete, false)) {
            if (!selectedNodes.empty()) {
                auto batchCmd = std::make_unique<Command::CmdBatch>();
                for (const auto& nodeID : selectedNodes) {
                    batchCmd->AddCommand(std::make_unique<Command::CmdDeleteNode>(nodeID));
                }
                command_bus.PushAndExecute(std::move(batchCmd));
                canvas_manager->ClearSelection();
            }
        }
    } // CanvasInputController::HandleKeyboardInputs()

} // SwarmIDE::UI