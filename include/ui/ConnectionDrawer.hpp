#pragma once
#include "ui/Topology.hpp"
#include "SwarmIDE.hpp"
#include "imgui.h"
#include "core/ObjectManager.hpp"
#include "core/ConnectionManager.hpp"

namespace SwarmIDE::UI {

    class ConnectionDrawer {
    public:
        static void DrawLink(Camera& camera, glm::vec2 windowSize); 
        static void DrawPins(Camera& camera, glm::vec2 windowSize);

        static ConnectionDrawer& getInstance(); 

        void SetConnectionManager(SwarmIDE::Core::ConnectionManager* mgr) { connection_manager = mgr; }
        void SetObjectManager(SwarmIDE::Core::ObjectManager* mgr) { object_manager = mgr; }
    private:
        static float tension;
        SwarmIDE::Core::ConnectionManager* connection_manager = nullptr;
        SwarmIDE::Core::ObjectManager* object_manager = nullptr;

        // State Machine

        static bool isConnecting;
        int sourceObjectID;
        std::string sourcePinID;
    };

}