#pragma once
#include "SwarmIDE.hpp"
#include <string>
#include <vector>

namespace SwarmIDE::UI {
    
    /**
     * @enum NotificationType
     * @brief Класс, указывающий тип уведомления.
     */
    enum class NotificationType { 
        Info,                        /** @brief < Информирующие уведомление. */
        Warning,                     /** @brief < Уведомление о предупреждении. */
        Error,                       /** @brief < Уведомление об ошибке. */
    };

    /**
     * @struct Notification
     * @brief Структура уведомления
     * @see NotificationType
     */
    struct Notification {
        std::string title;           /** @brief < Заголовок уведомления. */
        std::string message;         /** @brief < Текст уведомления. */
        NotificationType type;       /** @brief < Тип уведомления. */
        float duration;              /** @brief < Время жизни в секундах уведомления. */
    };

    /**
     * @class NotificationMenu
     * @brief Менеджер окон уведомлений
     * 
     * @todo Нужно будет сделать по уму. Потому что сейчас не испльзуется в полном объёме. Пока даже не проверял работу.
     */
    class NotificationMenu {
    public:
        /**
         * @brief Доступ к единственному экземпляру NotificationMenu
         * @return NotificationMenu& 
         */
        static NotificationMenu& getInstance();

        void AddNotification(const std::string& title, const std::string& message, const NotificationType type, float duration);
        void Draw(float deltaTime);

        // Запрещаем перемещение и копирование менеджера сообщений.
        NotificationMenu(const NotificationMenu&) = delete;
        NotificationMenu& operator=(const NotificationMenu&) = delete;
        NotificationMenu(NotificationMenu&&) = delete;
        NotificationMenu& operator=(NotificationMenu&&) = delete;
    private:
        NotificationMenu() = default;
        ~NotificationMenu() = default;

        std::vector<Notification> notifications;
    };
}