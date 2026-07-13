cmake_minimum_required(VERSION 3.14)
project(grui)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

set(CMAKE_C_STANDARD 23)
set(RAYLIB_INSTALL OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

include(FetchContent)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.0
)

FetchContent_Declare(
    raygui
    GIT_REPOSITORY https://github.com/raysan5/raygui.git
    GIT_TAG 4.0
)

FetchContent_Declare(
    gdslib
    GIT_REPOSITORY https://github.com/gabrunken/gds.git
    GIT_TAG main
)

FetchContent_MakeAvailable(raylib)
FetchContent_MakeAvailable(raygui)
FetchContent_MakeAvailable(gdslib)

add_library(gruilib STATIC src/grui.c)
target_link_libraries(gruilib PUBLIC raylib)
target_include_directories(gruilib PUBLIC
include
${raygui_SOURCE_DIR}/src
${gdslib_SOURCE_DIR}/data_structures)

option(BUILD_TEST "Build Test executable" OFF)

if (BUILD_TEST)
    add_executable(test test/main.c)
    target_include_directories(test PRIVATE include)
    target_link_libraries(test PRIVATE gruilib)
endif()
