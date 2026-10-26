cmake_minimum_required(VERSION 3.16)
project(DayDoom VERSION 0.0.1 LANGUAGES CXX C RC)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Resolução canônica interna (16:9 Widescreen Retrô)
add_compile_definitions(
    SCREEN_WIDTH=480
    SCREEN_HEIGHT=270
    WINDOW_TITLE="DayDoom 2.5D [v001] - DayZ Retro Demake"
    DAYDOOM_VERSION="v001"
)

# Inclusão de diretórios de código
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hal
    ${CMAKE_CURRENT_SOURCE_DIR}/src/engine
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game
)

# Detecção ou download automático da SDL2
find_package(SDL2 QUIET)

if(NOT SDL2_FOUND)
    message(STATUS "SDL2 não encontrada no sistema. Baixando e configurando SDL2 via FetchContent...")
    include(FetchContent)
    FetchContent_Declare(
        SDL2
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-2.30.2
        GIT_SHALLOW TRUE
    )
    set(SDL_SHARED TRUE CACHE BOOL "Build shared SDL library" FORCE)
    set(SDL_STATIC FALSE CACHE BOOL "Build static SDL library" FORCE)
    set(SDL_TEST FALSE CACHE BOOL "Build SDL test programs" FORCE)
    FetchContent_MakeAvailable(SDL2)
    set(SDL2_LIBRARIES SDL2::SDL2)
else()
    message(STATUS "SDL2 encontrada com sucesso!")
    if(TARGET SDL2::SDL2)
        set(SDL2_LIBRARIES SDL2::SDL2)
    elseif(TARGET SDL2::SDL2-static)
        set(SDL2_LIBRARIES SDL2::SDL2-static)
    endif()
endif()

# Arquivos de código-fonte
file(GLOB_RECURSE SOURCES
    "src/*.cpp"
    "src/*.hpp"
    "src/*.h"
)

if(WIN32)
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/version.rc")
endif()

add_executable(DayDoom ${SOURCES})

if(TARGET SDL2::SDL2main)
    target_link_libraries(DayDoom PRIVATE SDL2::SDL2main)
endif()
target_link_libraries(DayDoom PRIVATE ${SDL2_LIBRARIES})

# Geração automática do executável versionado com tag da release
add_custom_command(TARGET DayDoom POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_FILE:DayDoom>"
    "$<TARGET_FILE_DIR:DayDoom>/DayDoom_v001.exe"
    COMMENT "Gerando executavel versionado DayDoom_v001.exe"
)

# Copiar assets para a pasta de saída do executável
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/assets")
    add_custom_command(TARGET DayDoom POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:DayDoom>/assets"
    )
endif()
