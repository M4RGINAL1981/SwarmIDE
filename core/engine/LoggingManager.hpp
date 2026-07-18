#pragma once
#include "SwarmIDE.hpp"
#include <mutex>
#include <iostream>

namespace SwarmIDE::Core {

    /**
     * @class LoggingManager
     * @brief Статический менеджер для потокобезопасного логирования (Thread-safe Logging).
     * @details Обеспечивает централизованный вывод отладочной информации и системных логов 
     * вычислительного ядра в консоль. Защищает поток `std::cout` от состояния гонки (Race Condition), 
     * когда несколько ИИ-агентов одновременно пытаются напечатать логи из параллельных потоков.
     * 
     * @note Все методы класса являются статическими. Объект класса создавать не нужно.
     */
    class LoggingManager {
    public:
        
        /**
         * @brief Выполняет атомарную печать переданных аргументов в консоль.
         * @details Использует механизм Fold Expression (сворачивание параметров шаблона), появившийся в C++17.
         * Метод автоматически блокирует мьютекс #logMutex перед выводом и гарантирует, что строки от 
         * разных потоков не перемешаются в консоли. В конце автоматически добавляется перенос строки `std::endl`.
         * 
         * **Пример использования:**
         * @code
         * SwarmIDE::Core::LoggingManager::Print("[Core] Запуск ноды ID: ", node->GetID().str());
         * @endcode
         * 
         * @tparam Args Типы передаваемых аргументов (любые типы, поддерживающие оператор `<<` для `std::cout`).
         * @param[in] args Переменное число аргументов (Variadic Arguments) для вывода.
         */
        template<typename... Args>
        static void Print(Args&&... args) {
            std::lock_guard<std::mutex> lock(logMutex);
            (std::cout << ... << args) << std::endl;
        }

    private:
        static inline std::mutex logMutex; /**< Статический мьютекс для синхронизации доступа к стандартному выводу std::cout. */
    };
}
