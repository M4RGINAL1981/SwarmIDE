#ifndef GRID_HPP
#define GRID_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <graphics/Camera.hpp>
#include <graphics/Colors.hpp>


class Grid {
public:

    Grid(glm::vec3 C, float s = 100.0f) : color(C), step(s) {};
    Grid() : Grid(Colors::DEFAULT_GRAY, 100.0f) {};
    Grid(float s) : Grid(Colors::DEFAULT_GRAY, s) {};

    void init() {
        // 1. Считывание шейдеров
        // Временно ставим путь "resources/shaders/...", предполагая запуск из корня проекта.
        // Если запускаешь из папки build/Debug, путь должен быть "../../resources/shaders/"
        std::string vPath = "resources/shaders/grid_vertex.glsl";
        std::string fPath = "resources/shaders/grid_fragment.glsl";

        std::ifstream ShaderVFile(vPath);
        std::ifstream ShaderFFile(fPath);

        // Если не открылось из корня, пробуем путь для запуска из build/Debug
        if (!ShaderVFile.is_open() || !ShaderFFile.is_open()) {
            ShaderVFile.close(); ShaderFFile.close();
            vPath = "resources/shaders/grid_vertex.glsl";
            fPath = "resources/shaders/grid_fragment.glsl";
            ShaderVFile.open(vPath);
            ShaderFFile.open(fPath);
        }

        if (!ShaderVFile.is_open() || !ShaderFFile.is_open()) {
            std::cerr << "ERROR: Не удалось найти или открыть файлы шейдеров сетки!\n";
            std::cerr << "Искал по путям: " << vPath << " и " << fPath << "\n";
            return;
        }

        std::stringstream ShaderVStream, ShaderFStream;
        ShaderVStream << ShaderVFile.rdbuf();     
        ShaderFStream << ShaderFFile.rdbuf(); 
        ShaderVFile.close(); ShaderFFile.close();

        std::string vStringCode = ShaderVStream.str();
        std::string fStringCode = ShaderFStream.str();
        const char* vCode = vStringCode.c_str();        
        const char* fCode = fStringCode.c_str();

        int success;
        char infoLog[512];

        unsigned int vShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vShader, 1, &vCode, NULL); 
        glCompileShader(vShader);
        glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vShader, 512, NULL, infoLog);
            std::cerr << "ERROR: Вершинный шейдер сетки НЕ скомпилировался:\n" << infoLog << "\n";
        }

        unsigned int fShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fShader, 1, &fCode, NULL); 
        glCompileShader(fShader);
        glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fShader, 512, NULL, infoLog);
            std::cerr << "ERROR: Фрагментный шейдер сетки НЕ скомпилировался:\n" << infoLog << "\n";
        }

        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vShader);
        glAttachShader(shaderProgram, fShader);
        glLinkProgram(shaderProgram);
        
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            std::cerr << "ERROR: Шейдерная программа сетки НЕ слинковалась:\n" << infoLog << "\n";
        }

        glDeleteShader(vShader); glDeleteShader(fShader);

        // 2. Инициализация геометрии (Статичный NDC квад)
        float quadVertices[] = {
            -1.0f,  1.0f,
            -1.0f, -1.0f,
            1.0f, -1.0f,

            -1.0f,  1.0f,
            1.0f, -1.0f,
            1.0f,  1.0f
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
        
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);
    }

   void draw(Camera& camera) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glUseProgram(shaderProgram);

        // Находишь location
        GLint winSizeLoc = glGetUniformLocation(shaderProgram, "windowSize");
        // И передаешь
        glUniform2f(winSizeLoc, (float)windowWidth, (float)windowHeight);

        // 1. Параметры сетки
        glUniform3f(glGetUniformLocation(shaderProgram, "gridColor"), color.r, color.g, color.b);
        glUniform1f(glGetUniformLocation(shaderProgram, "stepSize"), step);
        glUniform1f(glGetUniformLocation(shaderProgram, "thickness"), lineThickness);

        // 2. Передаем логические размеры окна
        glUniform2f(glGetUniformLocation(shaderProgram, "windowSize"), (float)windowWidth, (float)windowHeight);

        // 3. Состояние твоей камеры напрямую
        glUniform2f(glGetUniformLocation(shaderProgram, "cameraOffset"), camera.offset.x, camera.offset.y);
        glUniform1f(glGetUniformLocation(shaderProgram, "cameraZoom"), camera.zoomFactor);

        // 4. Отрисовка статичного полноэкранного квада
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        glUseProgram(0);
        glDisable(GL_BLEND);
    }
            
    // Сеттеры для размеров окна (чтобы обновлять их при ресайзе)
    void SetWindowSize(int width, int height)  { windowWidth = width; windowHeight = height; }
    void SetLineThicknessa(float lineT) { if(lineT <= 0.0f) return; lineThickness = lineT; }

private:

    unsigned int shaderProgram = 0;

    // Переменные состояния сестки
    float step = 100.0f;        // Шаг сетки
    glm::vec3 color;            // Цвет сетки
    int windowWidth = 3840;     // Ширина окна
    int windowHeight = 2160;    // Высота окна
    float lineThickness = 5.0;      // Толшина сетки

    // ID объектов OpenGL
    unsigned int VAO = 0;
    unsigned int VBO = 0;

    // Вершины одной линии
    std::vector<glm::vec2> lineVertices;
};


#endif // GRID_HPP