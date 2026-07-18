#include <mutex>
#include <iostream>

namespace SwarmIDE::Core {

    class LoggingManager { // Логирование выполнения для модели
    public:
        
        template<typename... Args>
        static void Print(Args&&... args) {
            std::lock_guard<std::mutex> lock(logMutex);
            (std::cout << ... << args) << std::endl;
        }

    private:
        static inline std::mutex logMutex;
    };
}