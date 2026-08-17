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
        std::erase_if(viewNodes, [this, &graph_manager](const auto& item) {
            const auto& [viewNodeID, viewNode] = item;
            if (!graph_manager.findNodeByID(viewNodeID)) {
                removeViewConnectionsForNode(viewNodeID);
                return true; 
            }
            return false; 
        });
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

        // Если идёт событие проведение связи
        if (tempWire.has_value()) {
            // Находим стартовую ноду
            ViewNode* startNode = findViewNodeByID(tempWire->startNodeID);
            if (startNode) {
                // Находим стартовый пин
                ViewPin* startPin = startNode->findViewPin(tempWire->startPinID);
                if (startPin) {
                    // Получаем экранные координаты пина - Начало связи
                    glm::vec2 startScreenPos = startPin->GetScreenPos();
                    glm::vec2 startNormalPin = startPin->GetNormal();
                    // Получаем координаты мыши - Конец связи
                    glm::vec2 endScreenPos = camera.worldToScreen(tempWire->currentMouseWorldPos, windowSize);

                    auto& theme = ThemeManager::getInstance();
                    float strength = theme.GetStyleVar(StyleVarRole::ConnectionStrength);

                    // Отрисовываем кубическую кривую Безье
                    drawList->AddBezierCubic(

                        ImVec2(startScreenPos.x, startScreenPos.y),                                                                                   // Начало связи
                        ImVec2(startScreenPos.x + startNormalPin.x * zoom * strength, startScreenPos.y + startNormalPin.y * zoom * strength),         // Первая направляющая/конечная точка
                        ImVec2(endScreenPos.x - startNormalPin.x * zoom * strength, endScreenPos.y - startNormalPin.y * zoom * strength),             // Вторая направляющая/Конечная точка
                        ImVec2(endScreenPos.x, endScreenPos.y),                                                                                       // Конец связи
                        /** @todo Передача цвета должна происходить через ThemeManger. Хотя не обязательно, раз это проведение, а не построение. */
                        IM_COL32(200, 200, 200, 255), 
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

}