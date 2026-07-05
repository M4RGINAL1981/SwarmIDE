#include "core/AppController.hpp"

int main(){
    setlocale(LC_ALL, "ru");

    SwarmIDE::Core::AppController app;
    app.run();
    return 0;
}