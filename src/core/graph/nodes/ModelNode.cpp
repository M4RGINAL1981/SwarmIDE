#include "core/graph/nodes/ModelNode.hpp"
#include "core/ModelManager.hpp"
#include "core/LoggingManager.hpp"

namespace SwarmIDE::Core::Graph { 

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
        SetState(Core::Graph::Executing); 

        // Логирование
        LoggingManager::Print("\n[EXECUTING] ModelNode ID: ", GetID(), "\n"); 

        // Создание ActiveModel и загрузка в память
        try {
            bool result = SwarmIDE::Core::ModelManager::getInstance().loadModelToMemory(modelID);
            if(result) {
                // Устанавливаем ID модели в выходной пин
                setOutputPinValue("model_core", DataVariant(modelID));                    
                SetState(Core::Graph::Success); 

                LoggingManager::Print("\n[EXECUTION COMPLETED]: State: SUCCESS; ModelNode ID: ", GetID(), "\n"); 
            }
            else {
                SetState(Core::Graph::Failed);

                LoggingManager::Print("\n[EXECUTION COMPLETED]: State: Failed; ModelNode ID: ", GetID(), "\n"); 
            }
        }
        catch (const std::exception& e) {
            std::cerr << "[ERROR] ModelNode failed: " << e.what() << std::endl;
            SetState(Core::Graph::Failed);

            LoggingManager::Print("\n[EXECUTION COMPLETED]: State: Failed; ModelNode ID: ", GetID(), "\n"); 
        }
    }

    // РЕГИСТРАЦИЯ НОДЫ В ФАБРИКЕ
    REGISTER_NODE(ModelNode, "ModelNode");
}