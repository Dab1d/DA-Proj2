cmake_minimum_required(VERSION 3.20)
project(DA_Proj2)

set(CMAKE_CXX_STANDARD 20)

add_executable(main
        src/main.cpp
        src/ui/Menu.cpp
)

target_include_directories(main PRIVATE include structures)