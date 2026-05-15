cmake_minimum_required(VERSION 3.17)
project(DA_Proj2)
set(CMAKE_CXX_STANDARD 17)

add_executable(main
        src/main.cpp
        src/view/Menu.cpp
        src/view/BatchMode.cpp
        src/view/InteractiveMode.cpp
        src/parser/Parser.cpp
        src/parser/parserTools.cpp
        src/algorithms/WebBuilder.cpp
        src/algorithms/GraphColoring.cpp
        src/controllers/LoadRangesController.cpp
        src/controllers/RegisterAllocaterController.cpp
)

target_include_directories(main PRIVATE include include/structures include/data_structures)