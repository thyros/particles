cmake_minimum_required(VERSION 3.15.7)

project(Particles)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(ROOT ${CMAKE_CURRENT_LIST_DIR})
set(OUTPUT ${ROOT}/build/Debug)

add_executable(${PROJECT_NAME} source/main.cpp)

############################################################################
# SDL2
set(SDL3_INCLUDE_DIRS
    ${ROOT}/external/SDL3/include)

set(SDL3_LIBRARIES
    ${ROOT}/external/SDL3/lib/SDL3.lib
    ${ROOT}/external/SDL3/lib/SDL3_image.lib)

target_include_directories(${PROJECT_NAME} PRIVATE ${SDL3_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL3_LIBRARIES})

file(COPY ${ROOT}/external/SDL3/bin/SDL3.dll DESTINATION ${ROOT})
file(COPY ${ROOT}/external/SDL3/bin/SDL3_image.dll DESTINATION ${ROOT})

############################################################################
# ENTT
set(ENTT_INCLUDE_DIRS
    ${ROOT}/externals/entt/src)

target_include_directories(${PROJECT_NAME} PRIVATE ${ENTT_INCLUDE_DIRS})

############################################################################
# IMGUI
set(IMGUI_PATH ${ROOT}/external/imgui)

set(IMGUI_INCLUDE_DIRS
    ${IMGUI_PATH})

set(IMGUI_SOURCES
    ${IMGUI_PATH}/imgui.cpp
    ${IMGUI_PATH}/imgui_demo.cpp
    ${IMGUI_PATH}/imgui_draw.cpp
    ${IMGUI_PATH}/imgui_tables.cpp
    ${IMGUI_PATH}/imgui_widgets.cpp
    ${IMGUI_PATH}/backends/imgui_impl_sdlrenderer3.cpp
    ${IMGUI_PATH}/backends/imgui_impl_sdl3.cpp)

add_library(IMGUI ${IMGUI_SOURCES})
target_include_directories(IMGUI PRIVATE 
    ${IMGUI_PATH}
    ${SDL3_INCLUDE_DIRS})

target_include_directories(${PROJECT_NAME} PRIVATE ${IMGUI_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE IMGUI)
