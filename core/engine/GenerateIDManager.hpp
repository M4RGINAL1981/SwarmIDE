#pragma once
#include "SwarmIDE.hpp"
#include "uuid_v4/uuid_v4.h"
#include "core/engine/LoggingManager.hpp"
#include <mutex>

namespace SwarmIDE::Core {

    /**
     * @class GenerateIDManager
     * @brief Менеджер генерации уникальных идентификаторов UUID (Синглтон).
     * @details Обеспечивает потокобезопасную генерацию псевдослучайных идентификаторов 
     * стандарта UUIDv4. Используется для присвоения уникального ID каждому новому узлу графа, 
     * логическому пину или сессии выполнения, гарантируя отсутствие коллизий в рамках всего приложения.
     * 
     * @note Класс спроектирован как потокобезопасный синглтон.
     * @see SwarmIDE::Nodes::LogicNode::MakeNewID()
     */
    class GenerateIDManager {
    public:
        /**
         * @brief Глобальная точка доступа к единственному экземпляру GenerateIDManager.
         * @return GenerateIDManager& Ссылка на синглтон генератора идентификаторов.
         */
        static GenerateIDManager& getInstance();

        /**
         * @brief Сгенерировать и вернуть новый уникальный UUIDv4.
         * @details Метод блокирует внутренний мьютекс #mtx перед обращением к движку 
         * генерации. Это предотвращает одновременный доступ нескольких потоков к состоянию 
         * вихря Мерсенна (std::mt19937), защищая систему от рассинхронизации.
         * 
         * @return UUIDv4::UUID Сгенерированный уникальный объект-идентификатор.
         */
        UUIDv4::UUID GetNewID();
        
        // Блокируем копирование и перемещение менеджера
        GenerateIDManager(const GenerateIDManager&) = delete;
        GenerateIDManager& operator=(const GenerateIDManager&) = delete;
        GenerateIDManager(GenerateIDManager&&) = delete;
        GenerateIDManager& operator=(GenerateIDManager&&) = delete;

    private:
        GenerateIDManager() { Core::LoggingManager::Info("[GENERATE_ID_MANAGER]: Constructor"); }
        ~GenerateIDManager() { Core::LoggingManager::Info("[GENERATE_ID_MANAGER]: Destructor");}


        std::mutex mtx; /**< Мьютекс для защиты внутреннего состояния генератора при многопоточном вызове. */
        UUIDv4::UUIDGenerator<std::mt19937> generator; /**< Движок генерации UUIDv4, использующий стандартный алгоритм mt19937. */
    };
}
