#include "core/SerializationManager.hpp"
#include <json.hpp>
#include <fstream>
#include <iostream>
#include <cerrno>
#include <cstring>
#include "object/ai/ConversationalAgent.hpp"
#include "object/ai/PlanExecuteAgent.hpp"
#include "object/ai/ToolsAgent.hpp"
#include "object/memory/PostgreSQL.hpp"
#include "object/BaseModel.hpp"
#include "object/tool/JiraSoftware.hpp"

using namespace SwarmIDE::Core;
using namespace SwarmIDE::Object;
using namespace SwarmIDE::Object::Tool;
using namespace SwarmIDE::Object::Memory;


using json = nlohmann::json;

namespace SwarmIDE::Core {

    /*
    SerializationManager& SerializationManager::getInstance() {
        static SerializationManager instance;
        return instance;
    }

    std::string SerializationManager::GetNextAvailableFileName(const std::string& baseName) {
        int counter = 1;
        std::string defaultName = baseName + std::to_string(counter) + ".json";

        while(fs::exists(defaultName)) {
            counter++;
            defaultName = baseName + std::to_string(counter) + ".json";
        }

        return defaultName;
    }
   

    void SerializationManager::SaveGraph(const std::string& filepath) {

        std::cout << "SaveGraph" << std::endl;

        json root;

        root["objects"] = json::array();
        root["connections"] = json::array();

        auto& objects = SwarmIDE::Core::ObjectManager::getInstance().getObjects();
        for(auto& object : objects) {
            json objJson;
            objJson["id"] = object->GetID();
            objJson["name"] = object->GetName();
            
            // В качестве типа всегда пишем "BaseModel", чтобы фабрика загрузки знала, какой класс создавать
            objJson["type"] = object->GetTypeName(); 
            
            objJson["posX"] = object->GetWorldPos().x;
            objJson["posY"] = object->GetWorldPos().y;

            if (object->GetTypeName() == "Model") {
                auto* modelNode = dynamic_cast<SwarmIDE::Object::BaseModel*>(object.get());
                if (modelNode) objJson["modelId"] = modelNode->GetModelID();
            }

            root["objects"].push_back(objJson);            
        }
        auto& connection = SwarmIDE::Core::ConnectionManager::getInstance().getConnection();
        for(auto& connect : connection) {
            json connJson;
            connJson["fromNodeID"] = connect.fromNodeID;
            connJson["toNodeID"] = connect.toNodeID;
            connJson["fromPinID"] = connect.fromPinID;
            connJson["toPinID"] = connect.toPinID;

            root["connections"].push_back(connJson);
        }

        std::ofstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "SaveGraph Error: " << std::strerror(errno) << " (Path: " << filepath << ")" << std::endl;
            return;
        }
        file << root.dump(4);
        file.close();
    }

    void SerializationManager::LoadGraph(const std::string& filepath) {

        std::cout << "LoadGraph" << std::endl;

        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "LoadGraph Error: " << std::strerror(errno) << " (Path: " << filepath << ")" << std::endl;
            return;
        }

        json root;
        file >> root;
        
        SerializationManager::ClearGraph();

        if(root.contains("objects")) {
            for (const auto& objJson : root["objects"]) {
                int id = objJson["id"];
                std::string name = objJson["name"];
                std::string type = objJson["type"];
                glm::vec2 pos(objJson["posX"], objJson["posY"]);

                std::unique_ptr<BaseObject> newObj = nullptr;

                if (type == "ConversationalAgent")    newObj.reset(new SwarmIDE::Object::AI::ConversationalAgent(name, pos));
                else if (type == "PlanExecuteAgent")  newObj.reset(new SwarmIDE::Object::AI::PlanExecuteAgent(name, pos));
                else if (type == "ToolsAgent")        newObj.reset(new SwarmIDE::Object::AI::ToolsAgent(name, pos));
                else if (type == "PostgreSQL")        newObj.reset(new SwarmIDE::Object::Memory::PostgreSQL(name, pos));
                else if (type == "JiraSoftware")      newObj.reset(new SwarmIDE::Object::Tool::JiraSoftware(name, pos));
                else if (type == "Model") {
                    std::string modelId = objJson.value("modelId", ""); 
                    auto* modelPtr = SwarmIDE::Core::ModelManager::getInstance().GetModelForID(modelId);
                    
                    if (modelPtr) {
                        newObj.reset(new SwarmIDE::Object::BaseModel(name, pos, modelPtr));
                    } 
                    else {
                        std::cerr << "Warning: Model with ID '" << modelId << "' not found in ModelManager!" << std::endl;
                    }
                }

                if(newObj) {
                    newObj->SetID(id);
                    newObj->SetFonts(SerializationManager::getInstance().GetFonts());
                    ObjectManager::getInstance().addObject(std::move(newObj));
                }
            }
        }

        if(root.contains("connections")) {
            for (const auto& connJson : root["connections"]) {
                int sObjID = connJson["sourceObjectID"];
                int tObjID = connJson["targetObjectID"];
                std::string sPinID = connJson["sourcePinID"];
                std::string tPinID = connJson["targetPinID"];

                auto* sObj = ObjectManager::getInstance().findObjectByID(sObjID);
                auto* tObj = ObjectManager::getInstance().findObjectByID(tObjID);

                if (sObj && tObj) {
                    auto* sPin = sObj->GetShape()->GetAnchorPin(sPinID);
                    auto* tPin = tObj->GetShape()->GetAnchorPin(tPinID);

                    if(sPin && tPin) ConnectionManager::getInstance().connect(sObjID, tObjID, *sPin, *tPin);
                }
            }
        }

    }

    void SerializationManager::ClearGraph() {

        std::cout << "ClearGraph" << std::endl;

        SwarmIDE::Core::ConnectionManager::getInstance().removeAllConnections();
        SwarmIDE::Core::ObjectManager::getInstance().removeAllObjects();
    }
    */

}