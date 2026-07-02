#pragma once
#include "SwarmIDE.hpp"
#include "stb_image.h"
#include <unordered_map>
#include <string>
#include <optional>

namespace SwarmIDE::Core {

    struct Texture {
        unsigned char* data;
        int id;       
        int width;         // ширина иконки/текстуры
        int height;        // высота 
        int channels;      // Кол-во каналов
        std::string path;  // Строковой путь до иконки
    };

    class TextureManager {
    public:
        // Глобальная точка доступа к единственному экземпляру
        static TextureManager& getInstance();

        // Принимает название текстуры и путь до неё
        void loadTexture(const std::string& name, const std::string& filePath);
        std::optional<Texture> getTexture(const std::string& name) const;

        TextureManager(const TextureManager&) = delete;
        TextureManager& operator=(const TextureManager&) = delete;
        TextureManager(TextureManager&&) = delete;
        TextureManager& operator=(TextureManager&&) = delete;

    private:
        friend SwarmIDE::UI::Menu;    
        friend SwarmIDE::UI::ConnectionDrawer;
        friend SwarmIDE::Core::AppController;

        TextureManager() {};
        ~TextureManager();

        std::unordered_map<std::string, Texture> textures;
        static int count;
    };
}