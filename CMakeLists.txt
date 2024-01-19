cmake_minimum_required(VERSION 3.15.7)

project(Particles)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(${PROJECT_NAME} source/main.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC external/entt/src)

