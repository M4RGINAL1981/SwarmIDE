#include "SwarmIDE.hpp"
#include "ui/canvas/CanvasManager.hpp"
#include "graphics/Camera.hpp"
#include "graphics/Grid.hpp"

namespace SwarmIDE::UI {
    CanvasManager& CanvasManager::getInstance(){
        static CanvasManager instance;
        return instance;
    }

    CanvasManager::CanvasManager() {
        camera = Graphics::Camera();
        grid = Graphics::Grid();
        grid.init();
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
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        grid.draw(camera);

        float zoom = camera.GetCurrentZoomIndex();

        // Отрисовка соединений
        for(auto& conn : viewLinks) {
            conn->Draw(drawList, zoom);
        }

        // Отрисовка нод и их пинов
        for(auto& [id, viewNode] : viewNodes) {
            glm::vec2 worldPos = viewNode->GetWorldPos();
            glm::vec2 screenPos = camera.worldToScreen(worldPos, windowSize);

            viewNode->Draw(drawList, screenPos, zoom);
        }
    }
}