#pragma once
#include "ui/Topology.hpp"
#include "SwarmIDE.hpp"
#include "imgui.h"
#include "core/graph/GraphManager.hpp"
#include "core/ConnectionManager.hpp"

namespace SwarmIDE::UI {

    class ConnectionDrawer {
    public:
        //static void DrawLink(Camera& camera, glm::vec2 windowSize); 
        //static void DrawPins(Camera& camera, glm::vec2 windowSize);

        static ConnectionDrawer& getInstance(); 

        void SetConnectionManager(Core::ConnectionManager* mgr) { connection_manager = mgr; }
        void SetGraphManager(Core::Graph::GraphManager* mgr) { graph_manager = mgr; }
    private:
        static float tension;
        
        Core::ConnectionManager* connection_manager = nullptr;
        Core::Graph::GraphManager* graph_manager = nullptr;

        // State Machine

        static bool isConnecting;
        int sourceObjectID;
        std::string sourcePinID;
    };

}