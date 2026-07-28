#include "SwarmIDE.hpp"
#include "ui/canvas/CanvasManager.hpp"
#include "core/graph/GraphManager.hpp"
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
        // Удаляем все визуальные соединения с визуальной нодой
        removeViewConnectionsForNode(nodeID); 
        // Удаляем визуальную ноду
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

        viewLinks.push_back(std::make_unique<ViewConnection>(sourcePin, targetPin, conn));
    }
    
    void CanvasManager::RemoveViewConnection(Core::Connection* conn) {
        viewLinks.erase(
            std::remove_if(viewLinks.begin(), viewLinks.end(),
                [conn](const std::unique_ptr<ViewConnection>& vC) {
                    return vC->GetConnection() == conn;
                }), 
            viewLinks.end()
        );
    }

    void CanvasManager::SyncWithCoreGraph() {
        auto& coreGraph = Core::GraphManager::getInstance();
        const auto& coreNodes = coreGraph.getNodes();
        
        // 1. УДАЛЕНИЕ: Убираем из UI ноды, которых больше нет в Core
        for (auto it = viewNodes.begin(); it != viewNodes.end(); ) {
            UUIDv4::UUID currentID = it->first;
            
            if (!coreGraph.findNodeByID(currentID)) {
                removeViewConnectionsForNode(currentID);
                it = viewNodes.erase(it);
            } else {
                ++it;
            }
        }

        // 2. ДОБАВЛЕНИЕ: Создаем визуальные ноды для новых логических нод
        for (const auto& coreNode : coreNodes) {
            UUIDv4::UUID id = coreNode->GetID();
            if (!viewNodes.contains(id)) {
                // Используем корректный метод получения позиции камеры (например, GetCameraCenter или аналогичный)
                // Либо передаем дефолтную позицию / позицию из ноды
                AddViewNode(id, glm::vec2(0.0f, 0.0f)); 
            }
        }

        // 3. СИНХРОНИЗАЦИЯ СВЯЗЕЙ
        const auto& coreConnections = Core::ConnectionManager::getInstance().getAllConnections();
        
        std::erase_if(viewLinks, [&coreConnections](const std::unique_ptr<ViewConnection>& viewLink) {
            auto* logicalConn = viewLink->GetConnection();
            // Проверяем наличие связи через поиск в векторе логических связей ConnectionManager
            auto& allConns = Core::ConnectionManager::getInstance().getAllConnections();
            auto it = std::find(allConns.begin(), allConns.end(), *logicalConn);
            return it == allConns.end(); 
        });
    }

    void CanvasManager::Draw() {

        ImDrawList* drawList = ImGui::GetWindowDrawList();

        grid.draw(camera);

        float zoom = camera.zoomFactor; 
        
        for(auto& conn : viewLinks) {
            conn->Draw(drawList, zoom);
        }
        
        for(auto& [id, viewNode] : viewNodes) {
            glm::vec2 worldPos = viewNode->GetWorldPos();
            glm::vec2 screenPos = camera.worldToScreen(worldPos, windowSize);

            viewNode->Draw(drawList, screenPos, zoom);
        }
    }

    ViewConnection* CanvasManager::findViewConnection(Core::Connection* conn) {
        if (!conn) return nullptr;
        
        // Ищем визуальное соединение, которое ссылается на этот логический connection
        for (auto& viewLink : viewLinks) {
            if (viewLink->GetConnection() == conn) {
                return viewLink.get();
            }
        }
        return nullptr;
    }

    void CanvasManager::HandleCanvasInputs(float zoomFactor) {
        ImGuiIO& io = ImGui::GetIO();
        glm::vec2 mousePos = glm::vec2(io.MousePos.x, io.MousePos.y);

        ViewPin* hoveredViewPin = nullptr;
        ViewNode* hoveredViewNode = nullptr;

        // Сбрасываем флаги наведения перед каждым кадром
        for(auto& [id, viewNode] : viewNodes) {
            viewNode->isHovered = false;
            for(auto& viewPin : viewNode->GetViewPins()) {
                viewPin->SetHovered(false);
            }
        }

        // Если ничего не делаем — ищем, на что наведена мышь
        if(activeLinkContext.state == MouseEvents::IDLE) { 
            for(auto& [id, viewNode] : viewNodes) {
                auto& viewPins = viewNode->GetViewPins();
                for(auto& viewPin : viewPins) {
                    if(viewPin->IsPointInside(mousePos, zoomFactor)) {
                        viewPin->SetHovered(true);
                        hoveredViewPin = viewPin.get();
                        break;
                    }
                }

                // Если пин не задет, проверяем попадание в саму ноду
                if(hoveredViewPin == nullptr) {
                    // КРИТИЧЕСКИЙ ФИКС: Переводим мировые координаты ноды в экранные для корректного хитбокса
                    glm::vec2 screenPos = camera.worldToScreen(viewNode->worldPos, windowSize);
                    if(viewNode->shape->isPointInside(mousePos, screenPos, viewNode->nodeSize, zoomFactor)) {
                        viewNode->isHovered = true;
                        hoveredViewNode = viewNode.get();
                        break;
                    }
                }
            }

            // Обработка действий мыши
            if(hoveredViewPin != nullptr) {
                // Логика создания связи (в будущем)
            }
            else if(hoveredViewNode != nullptr) {
                if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                    activeLinkContext.state = MouseEvents::DRAGGING_NODE;
                    activeLinkContext.grabberNodeID = hoveredViewNode->nodeID;
                }
            }
        }
        // Если нода захвачена — плавно перетаскиваем её независимо от положения курсора внутри неё
        else if(activeLinkContext.state == MouseEvents::DRAGGING_NODE) {
            if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                glm::vec2 worldDelta = glm::vec2(io.MouseDelta.x, io.MouseDelta.y) / camera.zoomFactor;
                ViewNode* draggedNode = findViewNodeByID(activeLinkContext.grabberNodeID);
                if (draggedNode) {
                    draggedNode->MoveBy(worldDelta);
                }
            }
            
            // Сбрасываем состояние при отпускании левой кнопки мыши
            if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                activeLinkContext.Reset();
            }
        }
    }
}