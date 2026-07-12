cmake_minimum_required(VERSION 3.22)
project(SwarmIDE LANGUAGES CXX C)

# Принудительно заставляем генератор Visual Studio переключиться на x64
if(MSVC)
    set(CMAKE_GENERATOR_PLATFORM x64 CACHE STRING "Architecture" FORCE)
endif()

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
    "${EXTERNAL_DIR}/stb"
    "${EXTERNAL_DIR}/json"
    "${EXTERNAL_DIR}/pfd"
    "${EXTERNAL_DIR}"
    "${EXTERNAL_DIR}/llama/include"
    "${EXTERNAL_DIR}/uuid_v4"
)

# --- Интеграция библиотеки llama.cpp ---
# Отключаем сборку встроенных примеров, тестов и серверов llama.cpp, чтобы сэкономить время и вес
set(LLAMA_BUILD_EXAMPLES OFF CACHE BOOL "Build llama.cpp examples" FORCE)
set(LLAMA_BUILD_TESTS OFF CACHE BOOL "Build llama.cpp tests" FORCE)
set(LLAMA_BUILD_SERVER OFF CACHE BOOL "Build llama.cpp server" FORCE)

# Добавляем поддиректорию external/llama в дерево сборки CMake
add_subdirectory("${EXTERNAL_DIR}/llama")

# Специфичный хак для MSVC + C++20: заставляем u8"строки" вести себя как const char* # для исправления ошибок компиляции в файле llama-chat.cpp
if(MSVC)
    target_compile_options(llama PRIVATE /Zc:char8_t-)
endif()

# Линкуем библиотеки
target_link_libraries(SwarmIDE PRIVATE 
    "${EXTERNAL_DIR}/glfw/lib/glfw3.lib"
    glad 
    imgui_lib
    opengl32.lib # Нужно для Windows
    urlmon.lib
    llama        # Библиотека для инференса моделей
    $<$<PLATFORM_ID:Windows>:opengl32.lib>
    $<$<PLATFORM_ID:Windows>:urlmon.lib>
)

# Принудительно отключаем конфликтную Release-библиотеку MSVCRT в режиме отладки Debug
if(MSVC)
    target_link_options(SwarmIDE PRIVATE "$<$<CONFIG:Debug>:/NODEFAULTLIB:MSVCRT>")
endif()
