#include "SwarmIDE.hpp"
#include "nodes/core/ModelNode.hpp"
#include "core/models/ModelManager.hpp"
#include "core/engine/LoggingManager.hpp"

namespace SwarmIDE::Nodes {

    ModelNode::ModelNode(const std::string& nodeName) : LogicNode(nodeName) {
        addOutputPin("model_core", "string");
    }

    ModelNode::ModelNode(const std::string& nodeName, const std::string& targetModelID) : LogicNode(nodeName) {
        modelID = targetModelID;

        // Подключаем выходные пины
        addOutputPin("model_core", "string"); 
    }
  
    void ModelNode::Execute() {
        // Устанавливаем ноде статус Executing (Выполнение)
        SetState(Executing); 

        // Логирование
        Core::LoggingManager::Print("\n[EXECUTING] ModelNode ID: ", GetID(), "\n"); 

        // Создание ActiveModel и загрузка в память
        try {
            bool result = SwarmIDE::Core::ModelManager::getInstance().loadModelToMemory(modelID);
            if(result) {
                // Устанавливаем ID модели в выходной пин
                setOutputPinValue("model_core", Core::DataVariant(modelID));                    
                SetState(Success); 

                Core::LoggingManager::Print("\n[EXECUTION COMPLETED]: State: SUCCESS; ModelNode ID: ", GetID(), "\n"); 
            }
            else {
                SetState(Failed);

                Core::LoggingManager::Print("\n[EXECUTION COMPLETED]: State: Failed; ModelNode ID: ", GetID(), "\n"); 
            }
        }
        catch (const std::exception& e) {
            std::cerr << "[ERROR] ModelNode failed: " << e.what() << std::endl;
            SetState(Failed);

            Core::LoggingManager::Print("\n[EXECUTION COMPLETED]: State: Failed; ModelNode ID: ", GetID(), "\n"); 
        }
    }

    // РЕГИСТРАЦИЯ НОДЫ В ФАБРИКЕ
    REGISTER_NODE(ModelNode, "ModelNode");
}