#pragma once
#include "SwarmIDE.hpp"
#include "stb_image.h"
#include "core/engine/GenerateIDManager.hpp"
#include <unordered_map>
#include <string>
#include <optional>

namespace SwarmIDE::Graphics {

    struct Texture {
        unsigned char* data;   // Сама Иконки/Текстура
        UUIDv4::UUID id;       // id Иконки/Текстуры
        int width;             // Ширина Иконки/Текстуры
        int height;            // Высота Иконки/Текстуры
        int channels;          // Кол-во каналов Иконки/Текстуры
        std::string path;      // Строковой путь до Иконки/Текстуры
    };

    class TextureManager {
    public:
        // Глобальная точка доступа к единственному экземпляру
        static TextureManager& getInstance();

        // Принимает название текстуры и путь до неё
        void loadTexture(const std::string& name, const std::string& filePath);     // Загрузка текстуры
        std::optional<Texture> getTexture(const std::string& name) const;           // Выдаёт текстуру

        TextureManager(const TextureManager&) = delete;
        TextureManager& operator=(const TextureManager&) = delete;
        TextureManager(TextureManager&&) = delete;
        TextureManager& operator=(TextureManager&&) = delete;

    private:
        friend UI::Menu;    
        friend UI::ConnectionDrawer;
        friend Core::AppController;

        TextureManager() {};
        ~TextureManager();

        std::unordered_map<std::string, Texture> textures;
    };
}