#include "core/ObjectManager.hpp"
#include "core/ConnectionManager.hpp"

namespace SwarmIDE::Core{

    ObjectManager& ObjectManager::getInstance() {
        static ObjectManager instance;
        return instance;
    }

    void ObjectManager::drawUIAllObject(Camera& cam, glm::vec2 winSize){
        for(auto& object : objects){

            object->drawUI(cam, winSize);

            if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows) && ImGui::IsKeyPressed(ImGuiKey_Delete)) {
                int idToDelete = object->GetID();
                removeObject(idToDelete); 
            }
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

