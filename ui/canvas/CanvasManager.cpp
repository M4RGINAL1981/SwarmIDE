#include "SwarmIDE.hpp"
#include "ui/canvas/CanvasManager.hpp"
#include "core/graph/GraphManager.hpp"
#include "ui/command/CommandBus.hpp"
#include "graphics/Camera.hpp"
#include "graphics/Grid.hpp"
#include "core/engine/LoggingManager.hpp"

namespace SwarmIDE::UI {
    CanvasManager& CanvasManager::getInstance(){
        static CanvasManager instance;
        return instance;
    }

    CanvasManager::CanvasManager() : camera(Graphics::Camera()), grid(Graphics::Grid())  {
        Core::LoggingManager::Info("[CANVAS_MANAGER]: Constructor");
    }

    CanvasManager::~CanvasManager() {
        Core::LoggingManager::Info("[CANVAS_MANAGER]: Destructor");
        
        Core::LoggingManager::Info("[CANVAS_MANAGER]: ViewConnectionID:");
        for(const auto& [connectionID, viewConnection] : viewConnections) {
            Core::LoggingManager::Info(" - ", connectionID);
        }

        Core::LoggingManager::Info("[CANVAS_MANAGER]: ViewNodeID:");
        for(const auto& [nodeID, viewNode] : viewNodes) {
            Core::LoggingManager::Info(" - ", nodeID);
        }

        viewConnections.clear();
        viewNodes.clear();
    }

    void CanvasManager::AddViewNode(UUIDv4::UUID nodeID, glm::vec2 spawnPos) {
        if(viewNodes.contains(nodeID)) return;
        auto viewNode = std::make_unique<ViewNode>(nodeID, spawnPos);
        viewNodes[nodeID] = std::move(viewNode);
    }

    void CanvasManager::RemoveViewNode(UUIDv4::UUID nodeID) {
        removeViewConnectionsForNode(nodeID); 
        viewNodes.erase(nodeID);
    }                                 

    ViewNode* CanvasManager::findViewNodeByID(UUIDv4::UUID nodeID) {
        if(viewNodes.contains(nodeID)) return viewNodes[nodeID].get();
        return nullptr;
    }

    void CanvasManager::removeViewConnectionsForNode(UUIDv4::UUID nodeID) {
        auto& connection_manager = Core::ConnectionManager::getInstance();

        std::erase_if(viewConnections, [&](const auto& pair) {
            auto logicConnection = connection_manager.findConnection(pair.first);
            if(logicConnection && (logicConnection->fromNodeID == nodeID || logicConnection->toNodeID == nodeID)) {
                connection_manager.disconnect(pair.first);
                return true; // Удалит элемент из viewConnections
            }
            return false;
        });
    }

    void CanvasManager::AddViewConnection(UUIDv4::UUID connectionID) { 
        try {
            auto* conn = Core::ConnectionManager::getInstance().findConnection(connectionID);
            if(!conn) return;

            auto fromViewNode = findViewNodeByID(conn->fromNodeID);
            auto toViewNode = findViewNodeByID(conn->toNodeID);

            if(!fromViewNode || !toViewNode) {
                Core::LoggingManager::Warning("Skipping view connection creation because one or both nodes were not mapped to view nodes.");
                return;
            }

            auto fromPin = fromViewNode->findViewPin(conn->fromPinID);
            auto toPin = toViewNode->findViewPin(conn->toPinID);

            if(!fromPin || !toPin) {
                Core::LoggingManager::Warning("Skipping view connection creation because one or both pins were not found in the view layer.");
                return;
            }
            
            if(!viewConnections.contains(conn->getID())){
                Core::LoggingManager::Info("[CREATE]: ViewConnection is create. ViewConnectionID: ", conn->getID());
                viewConnections[conn->getID()] = std::make_unique<ViewConnection>(connectionID, fromPin, toPin);
            }
        }
        catch (const std::exception& ex) {
            Core::LoggingManager::Error("View connection creation failed: ", ex.what());
        }
        catch (...) {
            Core::LoggingManager::Error("View connection creation failed with unknown exception.");
        }
    }
    
    void CanvasManager::RemoveViewConnection(UUIDv4::UUID connectionID) {
        Core::LoggingManager::Info("[DELETE]: ViewConnection is delete");
        viewConnections.erase(connectionID);
    }

    ViewConnection* CanvasManager::findViewConnectionByID(UUIDv4::UUID connectionID) {
        if(viewConnections.contains(connectionID)) return viewConnections[connectionID].get();
        return nullptr;
    }

    void CanvasManager::SyncWithCoreGraph() {

        auto& graph_manager = Core::GraphManager::getInstance();
        const auto& logicNodes = graph_manager.getNodes();
        
        // Удаляем визуальные нodы, которых нет в логическом графе
        for (auto& [viewNodeID, viewNode] : viewNodes) {
            if (!graph_manager.findNodeByID(viewNodeID)) {
                removeViewConnectionsForNode(viewNodeID);
                viewNodes.erase(viewNodeID);
            }
        }
        // Добавляем визуальные ноды для всех логических нод, которых нет в viewNodes
        for (const auto& [nodeID, logicNode] : logicNodes) {
            if (!viewNodes.contains(nodeID)) AddViewNode(nodeID, glm::vec2(0.0f, 0.0f)); 
        }

        auto& connecction_manager = Core::ConnectionManager::getInstance();
        const auto& LogicConnections = connecction_manager.getAllConnections();

        // Удаляем визуальные соединения, которых нет в логическом графе
        for(auto& [viewConnectionID, viewConnection] : viewConnections) {
            if (!connecction_manager.findConnection(viewConnectionID)) {
                viewConnections.erase(viewConnectionID);
            }
        }
        // Добавляем визуальные соединения для всех логических соединений, которых нет в viewConnections
        for (const auto& [logicConnectionID, logicConnection] : LogicConnections) {
            if (!viewConnections.contains(logicConnectionID)) AddViewConnection(logicConnectionID);
        }
    }

    void CanvasManager::Draw() {
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        grid.draw(camera);

        // Получаем текущий зум
        float zoom = camera.zoomFactor;

        // Обновляем экранные координаты пинов до отрисовки соединений.
        for (auto& [id, viewNode] : viewNodes) {
            glm::vec2 worldPos = viewNode->GetWorldPos();
            glm::vec2 screenPos = camera.worldToScreen(worldPos, windowSize);
            for (auto& [pinID, viewPin] : viewNode->GetViewPins()) {
                viewPin->UpdatePos(screenPos, zoom);
            }
        }
        
        // Проходимся сначала по всем связям, чтобы они были отрисованы под нодами.
        for(auto& viewConn : viewConnections) viewConn.second->Draw(drawList, zoom);

        // Если событие мыши - проведение связи
        if (activeLinkContext.state == MouseEvents::DRAGGING_WIRE) {
            // Находим стартовую ноду
            ViewNode* startNode = findViewNodeByID(activeLinkContext.startNodeID);
            if (startNode) {
                // Находим стартовый пин
                ViewPin* startPin = startNode->findViewPin(activeLinkContext.startPinID);
                if (startPin) {
                    // Получаем экранные координаты пина - Начало связи
                    glm::vec2 startScreenPos = startPin->GetScreenPos();
                    glm::vec2 startNormalPin = startPin->GetNormal();
                    // Получаем координаты мыши - Конец связи
                    glm::vec2 endScreenPos = camera.worldToScreen(activeLinkContext.currentMouseWorldPos, windowSize);

                    auto& theme = ThemeManager::getInstance();
                    float strength = theme.GetStyleVar(StyleVarRole::ConnectionStrength);

                    // Отрисовываем кубическую кривую Безье
                    drawList->AddBezierCubic(

                        ImVec2(startScreenPos.x, startScreenPos.y),                                                                             // Начало связи
                        ImVec2(startScreenPos.x + startNormalPin.x * zoom * strength, startScreenPos.y + startNormalPin.y * zoom * strength),   // Первая направляющая/конечная точка
                        ImVec2(endScreenPos.x - startNormalPin.x * zoom * strength, endScreenPos.y - startNormalPin.y * zoom * strength),       // Вторая направляющая/Конечная точка
                        ImVec2(endScreenPos.x, endScreenPos.y),                                                                                 // Конец связи
                        /** @todo Передача цвета должна происходить через ThemeManger. Хотя не обязательно, раз это проведение, а не построение. */
                        IM_COL32(200, 200, 200, 255), 
                        // Вообще не понял - какого хуя тут умножение
                        2.0f * zoom
                    );
                }
            }
        }
        
        // Теперь проходимся по нодам и отрисовываем их
        for(auto& [id, viewNode] : viewNodes) {
            glm::vec2 worldPos = viewNode->GetWorldPos();
            glm::vec2 screenPos = camera.worldToScreen(worldPos, windowSize);

            viewNode->Draw(drawList, screenPos, zoom);
        }
    }

    void CanvasManager::HandleCanvasInputs(float zoomFactor) {
        ImGuiIO& io = ImGui::GetIO();
        // Получаем текущее положение мыши
        glm::vec2 mousePos = glm::vec2(io.MousePos.x, io.MousePos.y);
        // Сохраняем коодринаты мыши
        activeLinkContext.currentMouseWorldPos = camera.screenToWorld(mousePos, windowSize);

        // Объекты, что были выбраны пользоваелем (Визуальный пин и нода)
        ViewPin* hoveredViewPin = nullptr;
        // Будем фиксировать ID ноды, которой принадлежит пин
        UUIDv4::UUID hoveredPinParentNodeID; 
        ViewNode* hoveredViewNode = nullptr;
        
        // Проходимся по всем нодам и указываем, что они не выбраны
        for(auto& [viewNodeID, viewNode] : viewNodes) {
            viewNode->SetHovered(false);
            // Проходимся по всем пинам ноды и указываем, что они не выбраны
            for(auto& [viewPinID, viewPin] : viewNode->GetViewPins()) {
                viewPin->SetHovered(false);
            }
        }

        // Если событие мыши - Простой
        if(activeLinkContext.state == MouseEvents::IDLE) {   
            // Проходимся по всем визуальным нодам
            for(auto& [viewNodeID, viewNode] : viewNodes) {
                auto& viewPins = viewNode->GetViewPins();

                // Проходимся по всем визуальны пинам
                for(auto& [viewPinID, viewPin] : viewPins) {
                    if(viewPin->IsPointInside(mousePos, zoomFactor)) {
                        viewPin->SetHovered(true);
                        hoveredViewPin = viewPin.get();
                        // Запоминаем ID родительской нodы пина
                        hoveredPinParentNodeID = viewNodeID; 
                        break; 
                    }
                }

                // Если пользователь не выбрал пин, проверям выбрал ли пользователь ноду.
                if(hoveredViewPin == nullptr) {
                    glm::vec2 screenPos = camera.worldToScreen(viewNode->worldPos, windowSize);
                    if(viewNode->shape->isPointInside(mousePos, screenPos, viewNode->nodeSize, zoomFactor)) {
                        viewNode->isHovered = true;
                        hoveredViewNode = viewNode.get();
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
                }
            }
        }
        // Если событие мыши - перетаскивание ноды
        else if(activeLinkContext.state == MouseEvents::DRAGGING_NODE) {
            if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                // Находим смещение мыши (Дельту)
                glm::vec2 worldDelta = glm::vec2(io.MouseDelta.x, io.MouseDelta.y) / camera.zoomFactor;
                ViewNode* draggedNode = findViewNodeByID(activeLinkContext.grabberNodeID);
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

            // Проходимся циклом по всем визуальным нодам
            for(auto& [viewNodeID, viewNode] : viewNodes) {
                auto& viewPins = viewNode->GetViewPins();
                // Проходимся циклом по всем визуальным пинам
                for(auto& [viewPinID, viewPin] : viewPins) {
                    // Если пользователь указывает на новый пин (Второй). То есть хочет создать связь
                    if(viewPin->IsPointInside(mousePos, zoomFactor)) {
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
                        auto* startViewNode = findViewNodeByID(activeLinkContext.startNodeID);
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
            }
        }
    }
}