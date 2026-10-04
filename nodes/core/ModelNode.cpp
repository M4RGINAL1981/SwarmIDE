#include "SwarmIDE.hpp"
#include "nodes/core/ModelNode.hpp"
#include "core/models/ModelManager.hpp"
#include "core/engine/LoggingManager.hpp"

namespace SwarmIDE::Nodes {

    ModelNode::ModelNode(const std::string& nodeName) : LogicNode(nodeName) {
        addPin("Model", Core::PinType::Model, Core::Direction::Output, Core::DataType::STRING);
    }

    ModelNode::ModelNode(const std::string& nodeName, const std::string& targetModelID) : LogicNode(nodeName) {
        modelID = targetModelID;

        // Подключаем выходные пины
        addPin("Model", Core::PinType::Model, Core::Direction::Output, Core::DataType::STRING);
    }
  
    void ModelNode::Execute() {
        // Устанавливаем ноде статус Executing (Выполнение)
        SetState(Executing); 

        // Логирование
        Core::LoggingManager::ExecutingNode("ModelNode", GetID()); 

        // Создание ActiveModel и загрузка в память
        try {
            bool result = SwarmIDE::Core::ModelManager::getInstance().loadModelToMemory(modelID);
            if(result) {
                // Устанавливаем ID модели в выходной пин
                setPinValue("Model", Core::DataVariant(modelID));                    
                SetState(Success); 

                Core::LoggingManager::SuccessNode("ModelNode", GetID());
            }
            else {
                SetState(Failed);

                Core::LoggingManager::FailedNode("ModelNode", GetID());
            }
        }
        catch (const std::exception& e) {
            std::cerr << "[ERROR] ModelNode failed: " << e.what() << std::endl;
            SetState(Failed);

            Core::LoggingManager::FailedNode("ModelNode", GetID()); 
        }
    }

    std::vector<Core::PropertyDescriptor> ModelNode::properties() { 
        auto property = Prop("modelID", "Model", Core::PropertyKind::ModelRef, modelID);
        for (const auto& asset : Core::ModelManager::getInstance().getAllAssets()) {
            property.enumOptions.push_back(asset->id);
        }
        return { std::move(property) };
    }

    void ModelNode::ApplyDefaultSettings() {
        SetName("ModelNode");
        setTargetModelID("0"); // По умолчанию устанавливаем модель под индексом 0
    }

    // РЕГИСТРАЦИЯ НОДЫ В ФАБРИКЕ
    REGISTER_NODE(ModelNode, "ModelNode", "Model", "COGNITIVE MODELS");
}