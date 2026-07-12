#pragma once
#include "uuid_v4/uuid_v4.h"
#include <mutex>

namespace SwarmIDE::Core {
    class GenerateIDManager {
    public:
        // Глобальная точка доступа к единственному экземпляру
        static GenerateIDManager& getInstance();

        UUIDv4::UUID GetNewID();
        
        GenerateIDManager(const GenerateIDManager&) = delete;
        GenerateIDManager& operator=(const GenerateIDManager&) = delete;
        GenerateIDManager(GenerateIDManager&&) = delete;
        GenerateIDManager& operator=(GenerateIDManager&&) = delete;

    private:
        GenerateIDManager() = default;
        ~GenerateIDManager() = default;

        std::mutex mtx;
        UUIDv4::UUIDGenerator<std::mt19937> generator;
    };
}