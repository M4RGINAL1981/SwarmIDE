#pragma once
#include "SwarmIDE.hpp"
#include "object/BaseObject.hpp"
#include "graphics/Camera.hpp"
#include "graphics/Colors.hpp"
#include "graphics/Grid.hpp"
#include "core/SerializationManager.hpp"
#include <iostream>
#include <vector>
#include <optional>

namespace SwarmIDE::Core{

    class ObjectManager{
    public:
        // Глобальная точка доступа к единственному экземпляру
        static ObjectManager& getInstance();

        void drawUIAllObject(Camera& cam, glm::vec2 winSize);        // Отрисовка всех объектов
        void updateAllObject();                                      // Обновление всех объектов
        void addObject(std::unique_ptr<SwarmIDE::Object::BaseObject> object);
        void removeObject(int objectID);                             // Удаление объекта по id
        void removeAllObjects();                                     // Удаление вообще всех объектов

        int getCountObject();                                        // Вернуть кол-во объектов
        std::vector<std::unique_ptr<SwarmIDE::Object::BaseObject>>& getObjects() { return objects; }

        SwarmIDE::Object::BaseObject* findObjectByID(int id);        // Поиск объекта по id
 
        std::optional<SwarmIDE::UI::TransformedPin> GetTransformedPinForObject(int objectID, std::string PinID, float zoomFactor) const ;

        ObjectManager(const ObjectManager&) = delete;
        ObjectManager& operator=(const ObjectManager&) = delete;
        ObjectManager(ObjectManager&&) = delete;
        ObjectManager& operator=(ObjectManager&&) = delete;

    private:
        friend SwarmIDE::UI::Menu;    
        friend SwarmIDE::UI::ConnectionDrawer;
        friend SwarmIDE::Core::AppController;
        friend SwarmIDE::Core::SerializationManager;

        ObjectManager() {};
        ~ObjectManager() = default;

        std::vector<std::unique_ptr<SwarmIDE::Object::BaseObject>> objects;
    };

}

