#include "core/ObjectManager.hpp"

namespace SwarmIDE::Core{

    ObjectManager& ObjectManager::getInstance() {
        static ObjectManager instance;
        return instance;
    }

    void ObjectManager::drawUIAllObject(Camera& cam, glm::vec2 winSize){
        for(auto& object : objects){
            object->drawUI(cam, winSize);
        }
    }

    void ObjectManager::updateAllObject(){
        for(auto& object : objects){
            object->update();
        }
    }

    void ObjectManager::addObject(std::unique_ptr<SwarmIDE::Object::BaseObject> object) { 
        objects.push_back(std::move(object));
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

