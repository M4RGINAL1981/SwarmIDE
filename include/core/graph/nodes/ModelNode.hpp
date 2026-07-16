#pragma once
#include "core/graph/LogicNode.hpp"

namespace SwarmIDE::Core::Graph {

    class ModelNode : public LogicNode {
    public:
        ModelNode(const std::string& name);
        ModelNode(const std::string& name, const std::string& targetModelID);       // Конструктор
        virtual ~ModelNode() = default;                                             // Деструктор 

        
        virtual void Execute() override;                                            // Логика модели
        virtual std::string getTypeName() override { return "ModelNode"; }          // Вернуть тип ноды

        std::string getTargetModelID() const { return modelID; }                    // Вернуть id модели
        void setTargetModelID(const std::string& mD) { modelID = mD; }              // Установить id модели

    private:
        std::string modelID;                                                        // ID модели из ModelManager
    };

}