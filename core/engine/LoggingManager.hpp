#pragma once
#include "SwarmIDE.hpp"
#include <mutex>
#include <iostream>
#include <chrono>
#include <iomanip>

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
         * @brief Универсальный атомарный вывод лога с временной меткой.
         * @details Выводит время, применяет цветовой ANSI-код, сворачивает аргументы и сбрасывает цвет.
         */
        template<typename... Args>
        static void PrintWithColor(const char* colorCode, const char* label, Args&&... args) {
            std::lock_guard<std::mutex> lock(logMutex);
            
            // Получаем текущее системное время
            auto now = std::chrono::system_clock::now();
            auto time_t_now = std::chrono::system_clock::to_time_t(now);
            auto local_time = *std::localtime(&time_t_now);

            // Выводим серый таймстамп [HH:MM:SS] и цветной тег уровня лога
            std::cout << "\033[90m[" << std::put_time(&local_time, "%H:%M:%S") << "]\033[0m "
                      << colorCode << label << "\033[0m ";
            
            // Выводим само сообщение через Fold Expression
            (std::cout << ... << std::forward<Args>(args)) << std::endl;
        }

        /**
         * @brief Информационный лог для системных событий.
         * @details Используется для фиксации этапов штатной инициализации модулей.
         */
        template<typename ...Args>
        static void Info(Args&&... args) {
            PrintWithColor("\033[36m", "[INFO]:", std::forward<Args>(args)...); // Голубой
        }

        /**
         * @brief Предупреждение о потенциальной проблеме.
         * @details Фиксирует некритичные сбои, не требующие немедленной остановки программы.
         */
        template<typename ...Args>
        static void Warning(Args&&... args) {
            PrintWithColor("\033[33m", "[WARNING]:", std::forward<Args>(args)...); // Желтый
        }

        /**
         * @brief Ошибка выполнения конкретной операции.
         * @details Сигнализирует о сбое таски, поломке коннектора или ненайденном файле.
         */
        template<typename ...Args>
        static void Error(Args&&... args) {
            PrintWithColor("\033[31m", "[ERROR]:", std::forward<Args>(args)...); // Красный
        }

        /**
         * @brief Критический сбой, ведущий к остановке подсистемы.
         * @details Вызывается при фатальных ошибках, ломающих обход или структуру графа.
         */
        template<typename ...Args>
        static void Fatal(Args&&... args) {
            PrintWithColor("\033[41m\033[37m", "[CRITICAL]:", std::forward<Args>(args)...); // Красный фон, белый текст
        }

        /**
         * @brief  Показывает выполнение логики узлан на холсте
         * @details Показывает, что нода отрабатывает алгоритм.
         */
        template<typename ...Args>
        static void ExecutingNode(const std::string& nodeType, UUIDv4::UUID nodeID, Args&&... args) {
            PrintWithColor("\033[36m", "[NODE EXECUTING]:", nodeType, " (ID: ", nodeID.str(), "): ", std::forward<Args>(args)...); // Голубой
        }

        /**
         * @brief Проверка, готова ли нода к запуску или нет
         * @details Нету
         */
        template<typename ...Args>
        static void ReadinessCheck(const std::string& nodeType, UUIDv4::UUID nodeID, Args&&... args) {
            PrintWithColor("\033[36m", "[READINESS CHEACK]:", nodeType, " (ID: ", nodeID.str(), "): ", std::forward<Args>(args)...); // Голубой
        }

        /**
         * @brief Указывает что нода готова или не готова к запуску
         * @details Идёт проверка по флагу ready
         */
        template<typename ...Args>
        static void QueueResult(const std::string& nodeType, UUIDv4::UUID nodeID, bool ready, Args&&... args) {
            if(ready) PrintWithColor("\033[32m", "[QUEUE RESULT]:", nodeType, " (ID: ", nodeID.str(), "): Node is ready.", std::forward<Args>(args)...); // Зелёный 
            else PrintWithColor("\033[31m", "[QUEUE RESULT]:", nodeType, " (ID: ", nodeID.str(), "): Node is not ready.", std::forward<Args>(args)...); // Красный 
        }

        /**
         * @brief Успешное выполнение логики узла на холсте.
         * @details Показывает, что нода отработала алгоритм и успешно передала данные в пины.
         */
        template<typename ...Args>
        static void SuccessNode(const std::string& nodeType, UUIDv4::UUID nodeId, Args&&... args) {
            PrintWithColor("\033[32m", "[NODE SUCCESS]:", nodeType, " (ID: ", nodeId.str(), "): ", std::forward<Args>(args)...); // Зеленый
        }

        /**
         * @brief Ошибка или провал выполнения логического узла.
         * @details Фиксирует падение конкретного агента, таймаут API или провал инференса модели.
         */
        template<typename ...Args>
        static void FailedNode(const std::string& nodeType, UUIDv4::UUID nodeId, Args&&... args) {
            PrintWithColor("\033[35m", "[NODE FAILED]:", nodeType, " (ID: ", nodeId.str(), "): ", std::forward<Args>(args)...); // Пурпурный
        }

        /**
         * @brief Просто выводит строку перехода между логами
         */
        static void EnterText() {
            std::lock_guard<std::mutex> lock(logMutex);
            std::cout << std::endl;
        }

        // Блокируем копирование и перемещение менеджера
        LoggingManager(const LoggingManager&) = delete;
        LoggingManager& operator=(const LoggingManager&) = delete;
        LoggingManager(LoggingManager&&) = delete;
        LoggingManager& operator=(LoggingManager&&) = delete;
    private:
        LoggingManager() = default;
        ~LoggingManager() = default;

        static inline std::mutex logMutex; /**< Мьютекс защиты std::cout. */
    };
}
