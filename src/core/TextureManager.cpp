#define STB_IMAGE_IMPLEMENTATION
#include "core/TextureManager.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

namespace SwarmIDE::Core {

    TextureManager& TextureManager::getInstance() {
        static TextureManager instance;
        return instance;
    }

    int TextureManager::count = 0;

    void TextureManager::loadTexture(const std::string& name, const std::string& filePath) {
        count++;

        Texture textura;
        textura.path = filePath;
        textura.id = count;

        auto it = textures.find(name);
        if(it != textures.end()) return;

        textura.data = stbi_load(filePath.c_str(), &textura.width, &textura.height, &textura.channels, 0);
    }

    std::optional<Texture> TextureManager::getTexture(const std::string& name) const {

        auto it = textures.find(name);
        if(it == textures.end()) return std::nullopt;

        return it->second;
    }


    TextureManager::~TextureManager() {
        for(auto& [key, value] : textures){
            stbi_image_free(value.data);
        }
    }
}


