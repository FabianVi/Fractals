cmake_minimum_required(VERSION 3.20)

project(Fractals VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
configure_file(src/Config.h.in src/Config.h)

add_subdirectory(libs/wxWidgets)
add_subdirectory(libs/boost)

add_executable(${PROJECT_NAME} src/main.cpp src/Algorithm.h src/Algorithm.cpp src/Vector.h src/benchmark.h src/benchmark.cpp)

target_link_libraries(${PROJECT_NAME} wx::net wx::core wx::base)

target_include_directories(${PROJECT_NAME} PUBLIC wxWidgets)