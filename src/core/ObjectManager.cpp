#include "core/ObjectManager.hpp"
#include <iostream>
#include "core/ConnectionManager.hpp"

namespace SwarmIDE::Core{

    ObjectManager& ObjectManager::getInstance() {
        static ObjectManager instance;
        return instance;
    }

    void ObjectManager::drawUIAllObject(Camera& cam, glm::vec2 winSize){

        std::vector<int> idsToRemove;

        for(auto& object : objects){

            std::optional<bool> ifDelete = object->drawUI(cam, winSize);

            if (ifDelete != std::nullopt && *ifDelete) {
                std::cout << "Deletion request for ID:" << object->GetID() << std::endl;
                idsToRemove.push_back(object->GetID());
            }
        }

        for(int id : idsToRemove) {
            removeObject(id);
        }
    }

    void ObjectManager::updateAllObject(){
        for(auto& object : objects){
            object->update();
        }
    }

    void ObjectManager::addObject(std::unique_ptr<SwarmIDE::Object::BaseObject> object) { 
        objects.push_back(std::move(object));
        std::cout << "Object is add" << std::endl;
    }

    void ObjectManager::removeObject(int objectID) {
        SwarmIDE::Core::ConnectionManager::getInstance().removeConnectionsForObject(objectID);

        objects.erase(
            std::remove_if(objects.begin(), objects.end(),
                [objectID](const std::unique_ptr<SwarmIDE::Object::BaseObject>& obj) {
                    return obj->GetID() == objectID;
                }), 
            objects.end()
        );
    }

    void ObjectManager::removeAllObjects() {
        SwarmIDE::Core::ConnectionManager::getInstance().removeAllConnections();
        objects.clear();
    }

    SwarmIDE::Object::BaseObject* ObjectManager::findObjectByID(int id) {
        for(auto& object : objects){
            if(id == object->GetID()) return object.get();
        }
        return nullptr;
    }


    std::optional<SwarmIDE::UI::TransformedPin> ObjectManager::GetTransformedPinForObject(int objectID, std::string PinID, float zoomFactor) const {
        
        for(auto& object : objects){
            if(object->GetID() == objectID) {
                SwarmIDE::UI::AnchorPin* pin = object->GetShape()->GetAnchorPin(PinID);
                if(pin != nullptr) return object->GetShape()->GetWorldPin(*pin, zoomFactor); 
            }
        }
        return std::nullopt;
    }


    int ObjectManager::getCountObject() { return objects.size(); }
}

