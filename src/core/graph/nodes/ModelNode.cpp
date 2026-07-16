#include "core/graph/nodes/ModelNode.hpp"
#include "core/ModelManager.hpp"

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
        // Создание ActiveModel и загрузка в память
        std::cout << "Model node ID: " << modelID << std::endl;

        try {
            bool result = SwarmIDE::Core::ModelManager::getInstance().loadModelToMemory(modelID);
            if(result) {
                setOutputPinValue("model_core", modelID);                    // Устанавливаем ID модели в выходной пин
                SetState(Core::Graph::Success);
            }
            else {
                SetState(Core::Graph::Failed);
            }
        }
        catch (const std::exception& e) {
            std::cerr << "[ERROR] ModelNode failed: " << e.what() << std::endl;
            SetState(Core::Graph::Failed);
        }

        
    }

    // РЕГИСТРАЦИЯ НОДЫ В ФАБРИКЕ
    REGISTER_NODE(ModelNode, "ModelNode");
}