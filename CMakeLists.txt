cmake_minimum_required(VERSION 4.2.3 FATAL_ERROR)

set(CMAKE_CXX_COMPILE_COMMANDS "-BUILD_RAYLIB_CPP_MODULES=ON")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES ON) 

project(gican LANGUAGES CXX)

add_subdirectory(lib)
add_subdirectory(gican_shared)
add_subdirectory(gican_client)
add_subdirectory(gican_server)