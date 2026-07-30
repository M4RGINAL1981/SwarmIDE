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

    void CanvasManager::AddViewNode(UUIDv4::UUID nodeID, glm::vec2 spawnPos) {
        if(viewNodes.contains(nodeID)) return;
        auto viewNode = std::make_unique<ViewNode>(nodeID, spawnPos);
        viewNodes[nodeID] = std::move(viewNode);
    }

    void CanvasManager::RemoveViewNode(UUIDv4::UUID nodeID) {
        removeViewConnectionsForNode(nodeID); 
        viewNodes.erase(nodeID);
    }
          
    void CanvasManager::removeAllViewNodes() {
        RemoveAllViewConnection();
        viewNodes.clear();
    }                                  

    ViewNode* CanvasManager::findViewNodeByID(UUIDv4::UUID nodeID) {
        if(viewNodes.contains(nodeID)) return viewNodes[nodeID].get();
        return nullptr;
    }

    void CanvasManager::removeViewConnectionsForNode(UUIDv4::UUID nodeID) {
        std::erase_if(viewLinks, [nodeID](const auto& viewLink) {
            auto conn = viewLink->GetConnection();
            return conn->fromNodeID == nodeID || conn->toNodeID == nodeID;
        });
    }

    void CanvasManager::AddViewConnection(Core::Connection* conn) { 

        auto fromViewNode = findViewNodeByID(conn->fromNodeID);
        auto toViewNode = findViewNodeByID(conn->toNodeID);

        if(!fromViewNode || !toViewNode) return;

        auto sourcePin = fromViewNode->findViewPin(conn->fromPinID, Core::Direction::Output, conn->fromIndex);
        auto targetPin = toViewNode->findViewPin(conn->toPinID, Core::Direction::Input, conn->toIndex);
        
        Core::LoggingManager::Info("[CREATE]: ViewConnection is create");
        viewLinks.push_back(std::make_unique<ViewConnection>(sourcePin, targetPin, conn));
    }
    
    void CanvasManager::RemoveViewConnection(Core::Connection* conn) {
        Core::LoggingManager::Info("[DELETE]: ViewConnection is delete");
        viewLinks.erase(
            std::remove_if(viewLinks.begin(), viewLinks.end(),
                [conn](const std::unique_ptr<ViewConnection>& vC) {
                    return vC->GetConnection() == conn;
                }), 
            viewLinks.end()
        );
    }

    ViewConnection* CanvasManager::findViewConnection(Core::Connection* conn) {
        if (!conn) return nullptr;
        
        for (auto& viewLink : viewLinks) {
            if (viewLink->GetConnection() == conn) {
                return viewLink.get();
            }
        }
        return nullptr;
    }

    void CanvasManager::SyncWithCoreGraph() {
        auto& coreGraph = Core::GraphManager::getInstance();
        const auto& coreNodes = coreGraph.getNodes();
        
        for (auto it = viewNodes.begin(); it != viewNodes.end(); ) {
            UUIDv4::UUID currentID = it->first;
            
            if (!coreGraph.findNodeByID(currentID)) {
                removeViewConnectionsForNode(currentID);
                it = viewNodes.erase(it);
            } else {
                ++it;
            }
        }

        for (const auto& coreNode : coreNodes) {
            UUIDv4::UUID id = coreNode->GetID();
            if (!viewNodes.contains(id)) {
                AddViewNode(id, glm::vec2(0.0f, 0.0f)); 
            }
        }

        const auto& coreConnections = Core::ConnectionManager::getInstance().getAllConnections();
        
        std::erase_if(viewLinks, [&coreConnections](const std::unique_ptr<ViewConnection>& viewLink) {
            auto* logicalConn = viewLink->GetConnection();
            auto& allConns = Core::ConnectionManager::getInstance().getAllConnections();
            auto it = std::find(allConns.begin(), allConns.end(), *logicalConn);
            return it == allConns.end(); 
        });
    }

    void CanvasManager::Draw() {
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        grid.draw(camera);

        // Получаем текущий зум
        float zoom = camera.zoomFactor; 
        
        // Проходимся сначала по всем связям, чтобы они были отрисованы под нодами.
        for(auto& viewConn : viewLinks) {
            viewConn->Draw(drawList, zoom);
        }

        // Если событие мыши - проведение связи
        if (activeLinkContext.state == MouseEvents::DRAGGING_WIRE) {
            // Находим стартовую ноду
            ViewNode* startNode = findViewNodeByID(activeLinkContext.startNodeID);
            if (startNode) {
                // Находим стартовый пин
                ViewPin* startPin = startNode->findViewPin(activeLinkContext.startPinID, activeLinkContext.startPinDir, activeLinkContext.startPinIndex);
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
        for(auto& [id, viewNode] : viewNodes) {
            viewNode->SetHovered(false);
            // Проходимся по всем пинам ноды и указываем, что они не выбраны
            for(auto& viewPin : viewNode->GetViewPins()) {
                viewPin->SetHovered(false);
            }
        }

        // Если событие мыши - Простой
        if(activeLinkContext.state == MouseEvents::IDLE) {   
            // Проходимся по всем визуальным нодам
            for(auto& [id, viewNode] : viewNodes) {
                auto& viewPins = viewNode->GetViewPins();

                // Проходимся по всем визуальны пинам
                for(auto& viewPin : viewPins) {
                    if(viewPin->IsPointInside(mousePos, zoomFactor)) {
                        viewPin->SetHovered(true);
                        hoveredViewPin = viewPin.get();
                        // Запоминаем ID родительской ноды пина
                        hoveredPinParentNodeID = id; 
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
                    // Сохраняем ID ноды выбраного пина
                    activeLinkContext.startNodeID = hoveredPinParentNodeID;
                    // Сохраняем данные о пине (Его ID, направление и индекс)
                    activeLinkContext.startPinID = hoveredViewPin->GetPinID();
                    activeLinkContext.startPinDir = hoveredViewPin->GetDirection();
                    activeLinkContext.startPinIndex = hoveredViewPin->GetIndex();
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
            for(auto& [id, viewNode] : viewNodes) {
                auto& viewPins = viewNode->GetViewPins();
                // Проходимся циклом по всем визуальным пинам
                for(auto& viewPin : viewPins) {
                    // Если пользователь указывает на новый пин (Второй). То есть хочет создать связь
                    if(viewPin->IsPointInside(mousePos, zoomFactor)) {
                        // Сохраняем второй теперь уже выбранный пин
                        viewPin->SetHovered(true);
                        hoveredViewPin = viewPin.get();
                        hoveredPinParentNodeID = id;
                        break;
                    }
                }
                if(hoveredViewPin) break;
            }

            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                // Если выбраный пин (Второй) существует, и начальный пин (Первый) не равен выбранному - строим связь  
                if (hoveredViewPin != nullptr && hoveredViewPin->GetDirection() != activeLinkContext.startPinDir) {
                    // Данные из первой ноды
                    UUIDv4::UUID fromNodeID = (activeLinkContext.startPinDir == Core::Direction::Output) ? activeLinkContext.startNodeID : hoveredPinParentNodeID;
                    Core::PinID fromPinID = (activeLinkContext.startPinDir == Core::Direction::Output) ? activeLinkContext.startPinID : hoveredViewPin->GetPinID();
                    int fromIndex = (activeLinkContext.startPinDir == Core::Direction::Output) ? activeLinkContext.startPinIndex : hoveredViewPin->GetIndex();

                    // Данные из второй ноды
                    UUIDv4::UUID toNodeID = (activeLinkContext.startPinDir == Core::Direction::Input) ? activeLinkContext.startNodeID : hoveredPinParentNodeID;
                    Core::PinID toPinID = (activeLinkContext.startPinDir == Core::Direction::Input) ? activeLinkContext.startPinID : hoveredViewPin->GetPinID();
                    int toIndex = (activeLinkContext.startPinDir == Core::Direction::Input) ? activeLinkContext.startPinIndex : hoveredViewPin->GetIndex();

                    auto coreConn = std::make_unique<Command::CmdCreateConnection>(fromNodeID, fromPinID, fromIndex, toNodeID, toPinID, toIndex);
                    Command::CommandBus::getInstance().PushAndExecute(std::move(coreConn));
                }
                // Сбрасываем состояние
                activeLinkContext.Reset();
            }
        }
    }
}