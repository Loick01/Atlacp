cmake_minimum_required(VERSION 3.10)

project(atlacp VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(SDL2 REQUIRED)

set(SOURCES
    src/main.cpp
    src/window.cpp
    src/event.cpp
    src/image.cpp
)

add_executable(atlacp ${SOURCES})

target_include_directories(atlacp PRIVATE include ${SDL2_INCLUDE_DIRS})
target_link_libraries(atlacp PRIVATE ${SDL2_LIBRARIES})