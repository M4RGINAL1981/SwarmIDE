#include "SwarmIDE.hpp"
#include "ui/canvas/CanvasManager.hpp"
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

    void CanvasManager::Draw() {
        Core::LoggingManager::Info("[CANVAS_MANAGER]: DRAW is begin");

        ImDrawList* drawList = ImGui::GetWindowDrawList();

        grid.draw(camera);

        // БЫЛО: float zoom = camera.GetCurrentZoomIndex();
        // СТАЛО: Передаем реальный float множитель
        float zoom = camera.zoomFactor; 
        
        Core::LoggingManager::Info("[CANVAS_MANAGER]: Connection is Draw");
        for(auto& conn : viewLinks) {
            conn->Draw(drawList, zoom);
        }
        
        Core::LoggingManager::Info("[CANVAS_MANAGER]: Node is Draw");
        for(auto& [id, viewNode] : viewNodes) {
            glm::vec2 worldPos = viewNode->GetWorldPos();
            glm::vec2 screenPos = camera.worldToScreen(worldPos, windowSize);

            viewNode->Draw(drawList, screenPos, zoom);
        }

        Core::LoggingManager::Info("[CANVAS_MANAGER]: DRAW is end");
    }

    void CanvasManager::HandleCanvasInputs() {
        ImGuiIO& io = ImGui::GetIO();
        
        // Если мышь занята стандартным интерфейсом ImGui (например, контекстным меню) — игнорируем
        //if (io.WantCaptureMouse) return;

        glm::vec2 mousePos = glm::vec2(io.MousePos.x, io.MousePos.y);
        
        static UUIDv4::UUID draggedNodeID;
        static bool isDraggingNode = false;

        // ЛЕВЫЙ КЛИК: Проверяем нажатие на ноду
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            for (auto& [id, viewNode] : viewNodes) {
                glm::vec2 screenPos = camera.worldToScreen(viewNode->GetWorldPos(), windowSize);
                glm::vec2 scaledSize = viewNode->GetNodeSize() * camera.zoomFactor;

                // Попала ли мышь в границы ноды на экране
                if (mousePos.x >= screenPos.x && mousePos.x <= screenPos.x + scaledSize.x &&
                    mousePos.y >= screenPos.y && mousePos.y <= screenPos.y + scaledSize.y) {
                    
                    draggedNodeID = id;
                    isDraggingNode = true;
                    viewNode->isSelected = true;
                    break;
                }
            }
        }

        // ПРОЦЕСС ПЕРЕМЕЩЕНИЯ
        if (isDraggingNode) {
            if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                // Переводим дельту движения мыши из экранных координат в мировые (деля на zoom)
                glm::vec2 worldDelta = glm::vec2(io.MouseDelta.x, io.MouseDelta.y) / camera.zoomFactor;
                
                if (viewNodes.contains(draggedNodeID)) {
                    viewNodes[draggedNodeID]->worldPos += worldDelta;
                }
            }
            
            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                isDraggingNode = false;
            }
        }
        
        // ПОДВЕШИВАЕМ ХОВЕР (Определяем наведение для изменения цвета рамки)
        for (auto& [id, viewNode] : viewNodes) {
            glm::vec2 screenPos = camera.worldToScreen(viewNode->GetWorldPos(), windowSize);
            glm::vec2 scaledSize = viewNode->nodeSize * camera.zoomFactor;

            viewNode->isHovered = (mousePos.x >= screenPos.x && mousePos.x <= screenPos.x + scaledSize.x &&
                                mousePos.y >= screenPos.y && mousePos.y <= screenPos.y + scaledSize.y);
        }
    }
}