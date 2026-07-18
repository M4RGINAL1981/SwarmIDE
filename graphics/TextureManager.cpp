#define STB_IMAGE_IMPLEMENTATION
#include "graphics/TextureManager.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include <iostream>

namespace SwarmIDE::Graphics {

    /*
    TextureManager& TextureManager::getInstance() {
        static TextureManager instance;
        return instance;
    }

    void TextureManager::loadTexture(const std::string& name, const std::string& filePath) {

        auto it = textures.find(name);
        if(it != textures.end()) return;


        Texture textura;
        textura.path = filePath;
        textura.data = stbi_load(filePath.c_str(), &textura.width, &textura.height, &textura.channels, 4);

        if (textura.data) {
            GLuint gl_tex_id = 0;
            glGenTextures(1, &gl_tex_id);             // Просим у OpenGL один свободный ID
            glBindTexture(GL_TEXTURE_2D, gl_tex_id);  // Делаем эту текстуру активной

            // Настраиваем параметры фильтрации (чтобы при зуме не было мыла)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // Загружаем массив пикселей из RAM (textura.data) в VRAM видеокарты!
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textura.width, textura.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textura.data);
            
            // Отвязываем текстуру, чтобы случайно не сломать что-то другое
            glBindTexture(GL_TEXTURE_2D, 0);

            textura.id = gl_tex_id; // <-- ВОТ ТЕПЕРЬ ID ПРАВИЛЬНЫЙ! Это дескриптор OpenGL.
            
            // Пиксели в RAM нам больше не нужны, они уже на видеокарте. Освобождаем!
            stbi_image_free(textura.data);
            textura.data = nullptr; 
            
            textures[name] = textura;
        } else {
            std::cerr << "Failed to load image: " << filePath << std::endl;
        }

    }

    std::optional<Texture> TextureManager::getTexture(const std::string& name) const {

        auto it = textures.find(name);
        if(it == textures.end()) return std::nullopt;

        return it->second;
    }


    TextureManager::~TextureManager() {
        for(auto& [key, value] : textures){
            if(value.id) {
                GLuint gLuintID = static_cast<GLuint>(value.id);
                glDeleteTextures(1, &gLuintID);
            }
            if(value.data) {
                stbi_image_free(value.data);
            }
        }
    }
    */
}


