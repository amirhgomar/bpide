cmake_minimum_required(VERSION 3.29)
project(untitled11)

set(CMAKE_CXX_STANDARD 20)

add_executable(untitled12 main.cpp
        ui.cpp
        compiler.cpp)
target_link_libraries(untitled12 -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer)
