#include "SwarmIDE.hpp"
#include "CmdCreateConnection.hpp"
#include "ui/canvas/CanvasManager.hpp"

namespace SwarmIDE::UI::Command {


    void CmdCreateConnection::Execute() {
        Core::LoggingManager::Info("[CREATE]: Connection is create.");

        // Создание логической связи
        Core::ConnectionManager::getInstance().connect(
            conn->fromNodeID, conn->fromPinID, conn->fromIndex, conn->toNodeID, conn->toPinID, conn->toIndex);
        
        // Создание визуальной связи
        CanvasManager::getInstance().AddViewConnection(conn);
    }

    void CmdCreateConnection::Undo() {
        Core::LoggingManager::Info("[REMOVE]: Connection is remove.");

        // Проверка на существование связи
        if(!conn) return;

        // Удаление визуальной связи
        CanvasManager::getInstance().RemoveViewConnection(conn);

        // Удаление логической связи
        if(!conn) return;
        Core::ConnectionManager::getInstance().disconnect(conn);
    }
}