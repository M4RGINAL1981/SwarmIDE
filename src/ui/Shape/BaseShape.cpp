/*#include "ui/Shape/BaseShape.hpp"

namespace SwarmIDE::UI::Shape {

    std::vector<TransformedPin> BaseShape::GetWorldPins(float zoomFactor) const {
        std::vector<TransformedPin> result;
            
        for(auto pin : localPins){
            TransformedPin res;

            glm::vec2 scaledLocal = pin.localPos * size;

            float xDirPin = (cos(rotation) * scaledLocal.x - sin(rotation) * scaledLocal.y) + position.x;
            float yDirPin = (sin(rotation) * scaledLocal.x + cos(rotation) * scaledLocal.y) + position.y;

            res.globalPos = glm::vec2(xDirPin * zoomFactor, yDirPin * zoomFactor);

            float xDirNormal = cos(rotation) * pin.localNormal.x - sin(rotation) * pin.localNormal.y;
            float yDirNormal = sin(rotation) * pin.localNormal.x + cos(rotation) * pin.localNormal.y;

            res.globalNormal = glm::vec2(xDirNormal * zoomFactor, yDirNormal * zoomFactor);

            result.push_back(res);
        }

        return result;
            
    }

    TransformedPin BaseShape::GetWorldPin(AnchorPin pin, float zoomFactor) const {
        
        TransformedPin res;

        glm::vec2 scaledLocal = pin.localPos * size;

        float xDirPin = (cos(rotation) * scaledLocal.x - sin(rotation) * scaledLocal.y) + position.x;
        float yDirPin = (sin(rotation) * scaledLocal.x + cos(rotation) * scaledLocal.y) + position.y;

        res.globalPos = glm::vec2(xDirPin * zoomFactor, yDirPin * zoomFactor);

        float xDirNormal = cos(rotation) * pin.localNormal.x - sin(rotation) * pin.localNormal.y;
        float yDirNormal = sin(rotation) * pin.localNormal.x + cos(rotation) * pin.localNormal.y;

        res.globalNormal = glm::vec2(xDirNormal * zoomFactor, yDirNormal * zoomFactor);

        return res;
    }

    AnchorPin* BaseShape::GetAnchorPin(std::string pinID) const {
        for(const auto& pin : localPins){
            if(pin.id == pinID) return const_cast<AnchorPin*>(&pin);
        }
        return nullptr;
    }

}
    */