#include "core/GenerateIDManager.hpp"

namespace SwarmIDE::Core {

    GenerateIDManager& GenerateIDManager::getInstance() {
        static GenerateIDManager instance;
        return instance;
    }

    UUIDv4::UUID GenerateIDManager::GetNewID() { 
        std::lock_guard<std::mutex> lock(mtx);
        return generator.getUUID();
    } 
}