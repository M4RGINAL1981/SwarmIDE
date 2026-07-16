cmake_minimum_required(VERSION 3.22)
project(SwarmIDE LANGUAGES CXX C)

# Принудительно заставляем генератор Visual Studio переключиться на x64
if(MSVC)
    set(CMAKE_GENERATOR_PLATFORM x64 CACHE STRING "Architecture" FORCE)
endif()

set(CMAKE_CXX_STANDARD 20)

# =====================================================================
# ВАЖНОЕ ИСПРАВЛЕНИЕ: Собираем все EXE и DLL в одном месте (Решает ошибку -1073741515)
# =====================================================================
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
# =====================================================================

file(GLOB_RECURSE SOURCE_FILES "src/*.cpp" "src/*.c")
file(GLOB_RECURSE HEADER_FILES "include/*.hpp")

set(EXTERNAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external")

# 1. Либа GLAD
add_library(glad STATIC src/glad.c)
target_include_directories(glad PUBLIC "${EXTERNAL_DIR}/glad")

# 2. Либа ImGui
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
    "${EXTERNAL_DIR}/cpr"
)

# --- Интеграция библиотеки llama.cpp ---
set(LLAMA_BUILD_EXAMPLES OFF CACHE BOOL "Build llama.cpp examples" FORCE)
set(LLAMA_BUILD_TESTS OFF CACHE BOOL "Build llama.cpp tests" FORCE)
set(LLAMA_BUILD_SERVER OFF CACHE BOOL "Build llama.cpp server" FORCE)

# --- Настройки для CPR и встроенного curl ---
set(CPR_BUILD_TESTS OFF CACHE INTERNAL "")
set(CPR_BUILD_TESTS_WITH_WEBSOCKETS OFF CACHE INTERNAL "")
set(CPR_CURL_USE_LIBPSL OFF CACHE BOOL "Disable libpsl in CPR" FORCE)
set(CURL_USE_LIBPSL OFF CACHE BOOL "Disable libpsl in built-in curl" FORCE)

# Подключаем поддиректории зависимостей в дерево сборки
add_subdirectory("${EXTERNAL_DIR}/cpr")
add_subdirectory("${EXTERNAL_DIR}/llama")

# Специфичный хак для MSVC + C++20: фикс u8"" строк в llama-chat.cpp
if(MSVC)
    target_compile_options(llama PRIVATE /Zc:char8_t-)
endif()

# Линкуем библиотеки
target_link_libraries(SwarmIDE PRIVATE 
    "${EXTERNAL_DIR}/glfw/lib/glfw3.lib"
    cpr::cpr
    glad 
    imgui_lib
    llama        # Библиотека для инференса моделей
    $<$<PLATFORM_ID:Windows>:opengl32.lib>
    $<$<PLATFORM_ID:Windows>:urlmon.lib>
)

# Принудительно отключаем конфликтную Release-библиотеку MSVCRT в режиме отладки Debug
if(MSVC)
    target_link_options(SwarmIDE PRIVATE "$<$<CONFIG:Debug>:/NODEFAULTLIB:MSVCRT>")
endif()