cmake_minimum_required(VERSION 3.17)
project(DA_Proj2)
set(CMAKE_CXX_STANDARD 17)

add_executable(main
        src/main.cpp
        src/ui/Menu.cpp
        src/ui/BatchMode.cpp
        src/ui/InteractiveMode.cpp
        src/parser/Parser.cpp
)

target_include_directories(main PRIVATE include structures)