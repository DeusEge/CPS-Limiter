cmake_minimum_required(VERSION 4.0)
project(CPS-Limiter)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(cps-limiter
        src/main.cpp
        src/gui.cpp
        src/gui.h
)

add_executable(cps-limiter-cli
        src/cli.cpp
)

target_link_libraries(cps-limiter PRIVATE Qt6::Widgets)

set_target_properties(cps-limiter PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set_target_properties(cps-limiter-cli PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
