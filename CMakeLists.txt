cmake_minimum_required(VERSION 3.10)

project(atlacp VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(SDL2 REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2IMAGE REQUIRED SDL2_image)
pkg_check_modules(SDL2TTF REQUIRED SDL2_ttf)
pkg_check_modules(SDL2MIXER REQUIRED SDL2_mixer)

set(SOURCES
    src/player.cpp
    src/window.cpp
    src/event.cpp
    src/texture.cpp
    src/file.cpp
    src/tilemap.cpp
    src/drawable.cpp
    src/element.cpp
    src/tileset.cpp
    src/camera.cpp
    src/animation.cpp
    src/npc.cpp
    src/entity.cpp
    src/behaviour.cpp
    src/scene.cpp
    src/pathfind.cpp
    src/ui.cpp
    src/layer.cpp
    src/sound.cpp
    src/interaction.cpp
    src/movement.cpp
    src/entity_controller.cpp
    src/battle.cpp
)

add_executable(atlacp src/main.cpp ${SOURCES})

target_include_directories(atlacp PRIVATE include ${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS} ${SDL2TTF_INCLUDE_DIRS} ${SDL2MIXER_INCLUDE_DIRS})
target_link_libraries(atlacp PRIVATE ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SDL2TTF_LIBRARIES} ${SDL2MIXER_LIBRARIES})
