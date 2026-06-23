cmake_minimum_required(VERSION 3.22)
project(SwarmIDE LANGUAGES CXX C) # Добавили поддержку C для glad.c

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXTERNAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external)

# 1. Создаем отдельную статическую либу для GLAD
add_library(glad STATIC src/glad.c)
target_include_directories(glad PRIVATE ${EXTERNAL_DIR}/glad)

# 2. Основной исполняемый файл
add_executable(agent_framework src/main.cpp)

# 3. Настройки инклудов для твоего кода
target_include_directories(agent_framework PRIVATE 
    include
    ${EXTERNAL_DIR}/glad
    ${EXTERNAL_DIR}/glfw/include
    ${EXTERNAL_DIR}/glm
)

# Пути к GLFW
target_link_directories(agent_framework PRIVATE ${EXTERNAL_DIR}/glfw/lib)

# 4. Линкуем и GLFW, и нашу принудительную либу glad
target_link_libraries(agent_framework PRIVATE glfw3 glad)