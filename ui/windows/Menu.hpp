#pragma once
#include "SwarmIDE.hpp"
#include <glm/glm.hpp>

namespace SwarmIDE::UI {

    /**
     * @class Menu
     * @brief Панель создания элементов (Спавн нод, агентов, моделей, памяти).
     * 
     * @todo После нужно переделать для кросплотформенности. Так как сейчас использует библиотеку windows.h
     */
    class Menu {
    public:
        /**
         * @brief Доступ к единственному экземпляру класса Menu
         * @return Menu& 
         */
        static Menu& getInstance();
        void Draw(Graphics::Camera* camera);

        // Запрещаем перемешение и копирование менеджера(панель) создания элементов/нод.
        Menu(const Menu&) = delete;
        Menu& operator=(const Menu&) = delete;
        Menu(Menu&&) = delete;
        Menu& operator=(Menu&&) = delete;
    private:
        Menu() = default;
        ~Menu() = default;
    };
}
