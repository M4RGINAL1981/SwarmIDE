cmake_minimum_required(VERSION 3.22)
project(SwarmIDE LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)

# Ищем все .cpp файлы в папках src и include
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp" "src/*.c" "include/*.cpp")
file(GLOB_RECURSE HEADER_FILES "include/*.hpp")

set(EXTERNAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external")

# 1. Либа GLAD
add_library(glad STATIC src/glad.c)
target_include_directories(glad PUBLIC "${EXTERNAL_DIR}/glad")

# 2. Либа ImGui (добавляем все файлы из бекендов для надежности)
add_library(imgui_lib STATIC 
    "${EXTERNAL_DIR}/imgui/imgui.cpp"
    "${EXTERNAL_DIR}/imgui/imgui_draw.cpp"
    "${EXTERNAL_DIR}/imgui/imgui_widgets.cpp"
    "${EXTERNAL_DIR}/imgui/imgui_tables.cpp"
    "${EXTERNAL_DIR}/imgui/backends/imgui_impl_glfw.cpp"
    "${EXTERNAL_DIR}/imgui/backends/imgui_impl_opengl3.cpp"
)
target_include_directories(imgui_lib PUBLIC 
    "${EXTERNAL_DIR}/imgui"
    "${EXTERNAL_DIR}/imgui/backends"
    "${EXTERNAL_DIR}/glfw/include"
)

# 3. Основной исполняемый файл
add_executable(SwarmIDE ${SOURCE_FILES} ${HEADER_FILES})

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebugDLL")

# Игнорируем специфичную runtime-библиотеку для Release при сборке в Debug
add_link_options("$<$<CONFIG:Debug>:/NODEFAULTLIB:MSVCRT>")

# Подключаем все пути к инклюдам
target_include_directories(SwarmIDE PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${EXTERNAL_DIR}/glm"
    "${EXTERNAL_DIR}/imgui"
    "${EXTERNAL_DIR}/glfw/include"
)

# Линкуем библиотеки
target_link_libraries(SwarmIDE PRIVATE 
    "${EXTERNAL_DIR}/glfw/lib/glfw3.lib"
    glad 
    imgui_lib
    opengl32.lib # Нужно для Windows
)