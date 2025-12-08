cmake_minimum_required(VERSION 3.10)

project(atlacp VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(SDL2 REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2IMAGE REQUIRED SDL2_image)

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
)

add_executable(atlacp src/main.cpp ${SOURCES})

add_executable(editor src/editor.cpp ${SOURCES})

foreach (target atlacp editor)
    target_include_directories(${target} PRIVATE include ${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS})
    target_link_libraries(${target} PRIVATE ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES})
endforeach()