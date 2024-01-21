cmake_minimum_required(VERSION 3.15.7)

project(Particles)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(ROOT ${CMAKE_CURRENT_LIST_DIR})
set(OUTPUT ${ROOT}/build/Debug)

add_executable(${PROJECT_NAME} source/main.cpp)

set(SDL2_INCLUDE_DIRS
    ${ROOT}/external/SDL2_image-2.8.2/include
    ${ROOT}/external/SDL2-2.28.5/include)

set(SDL2_LIBRARIES
    ${ROOT}/external/SDL2_image-2.8.2/lib/x64/SDL2_image.lib
    ${ROOT}/external/SDL2-2.28.5/lib/x64/SDL2main.lib
    ${ROOT}/external/SDL2-2.28.5/lib/x64/SDL2.lib)

target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_LIBRARIES})

target_include_directories(${PROJECT_NAME} PRIVATE external/entt/src)

message(${OUTPUT})

file(COPY ${ROOT}/external/SDL2_image-2.8.2/lib/x64/SDL2_image.dll DESTINATION ${ROOT})
file(COPY ${ROOT}/external/SDL2-2.28.5/lib/x64/SDL2.dll DESTINATION ${ROOT})
