cmake_minimum_required(VERSION 3.22)
project(SwarmIDE LANGUAGES CXX C)

# Принудительно заставляем генератор Visual Studio переключиться на x64
if(MSVC)
    set(CMAKE_GENERATOR_PLATFORM x64 CACHE STRING "Architecture" FORCE)
endif()

set(CMAKE_CXX_STANDARD 20)

# =====================================================================
# Собираем все EXE и DLL в одном месте 
# =====================================================================
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
# =====================================================================

# Собираем файлы реализации (.cpp, .c) СТРОГО из ваших папок, исключая папки зависимостей
file(GLOB_RECURSE SOURCE_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/core/*.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/core/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/nodes/*.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/nodes/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/ui/*.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/ui/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/graphics/*.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/graphics/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/oi/*.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/oi/*.c"
)

# Явно добавляем только ВАШ корневой main.cpp, чтобы CMake не хватал main.cpp из примеров ImGui
list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")

# Собираем заголовочные файлы (.hpp, .h) строго из ваших папок
file(GLOB_RECURSE HEADER_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/core/*.hpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/core/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/nodes/*.hpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/nodes/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/ui/*.hpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/ui/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/graphics/*.hpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/graphics/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/oi/*.hpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/oi/*.h"
)
list(APPEND HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/SwarmIDE.hpp")

# Добавляем CUDA-файлы, если они есть
file(GLOB_RECURSE CUDA_FILES "${CMAKE_CURRENT_SOURCE_DIR}/core/cuda/*.cu")
list(APPEND SOURCE_FILES ${CUDA_FILES})

set(EXTERNAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external")

# 1. Либа GLAD
add_library(glad STATIC graphics/glad.c)
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
    "${CMAKE_CURRENT_SOURCE_DIR}"
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

# =====================================================================
# АВТОМАТИЧЕСКАЯ ГЕНЕРАЦИЯ КАРТЫ ПРОЕКТА (DOXYGEN + GRAPHVIZ)
# =====================================================================
find_package(Doxygen)

if(DOXYGEN_FOUND)
    set(DOXYGEN_PROJECT_NAME "SwarmIDE")
    set(DOXYGEN_PROJECT_NUMBER "0.2")
    set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/docs")
    
    set(DOXYGEN_INPUT 
        "${CMAKE_CURRENT_SOURCE_DIR}/core"
        "${CMAKE_CURRENT_SOURCE_DIR}/nodes"
        "${CMAKE_CURRENT_SOURCE_DIR}/ui"
        "${CMAKE_CURRENT_SOURCE_DIR}/graphics"
        "${CMAKE_CURRENT_SOURCE_DIR}/oi"
        "${CMAKE_CURRENT_SOURCE_DIR}/SwarmIDE.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
    )
    
    # Какие расширения файлов сканировать
    set(DOXYGEN_FILE_PATTERNS "*.hpp" "*.h" "*.cpp" "*.c")
    
    set(DOXYGEN_RECURSIVE YES)
    set(DOXYGEN_EXTRACT_ALL YES)      # Вытягивать даже недокументированный код
    set(DOXYGEN_EXTRACT_PRIVATE YES)  # Включаем private члены классов
    set(DOXYGEN_EXTRACT_STATIC YES)
    
    # НАСТРОЙКА ВИЗУАЛЬНЫХ ГРАФОВ (Graphviz)
    set(DOXYGEN_HAVE_DOT YES)
    set(DOXYGEN_CLASS_GRAPH YES)
    set(DOXYGEN_COLLABORATION_GRAPH YES)
    set(DOXYGEN_INCLUDE_GRAPH YES)
    set(DOXYGEN_CALL_GRAPH YES)
    set(DOXYGEN_CALLER_GRAPH YES)
    set(DOXYGEN_DIRECTORY_GRAPH YES)
    
    set(DOXYGEN_UML_LOOK YES)          # Делает графы классов похожими на профессиональные UML-диаграммы
    set(DOXYGEN_DOT_NUM_THREADS 4)     # Ускоряет генерацию картинок-графов в 4 потока
    set(DOXYGEN_GENERATE_LATEX NO)
    set(DOXYGEN_GENERATE_HTML YES)
    set(DOXYGEN_OUTPUT_LANGUAGE "Russian")

    # Передаем список папок в команду сборки
    doxygen_add_docs(doc_swarmide
        ${DOXYGEN_INPUT}
        COMMENT "Генерация интерактивной HTML-карты проекта SwarmIDE с графами зависимостей..."
    )
else()
    message(STATUS "Doxygen не найден в системе. Генерация интерактивной карты отключена.")
endif()

if(TARGET doc_swarmide)
    add_dependencies(SwarmIDE doc_swarmide)
endif()
