#include "SwarmIDE.hpp"
#include "ui/AppController.hpp"
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace SwarmIDE;

int main() {
#ifdef _WIN32
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
#endif
    setlocale(LC_ALL, "ru_RU.UTF-8");

    try {
        UI::AppController app;
        app.run();
    } 
    catch (const std::exception& e) {
        std::cerr << "\n[FATAL ERROR]: " << e.what() << std::endl;
        return 1;
    } 

    return 0;
}