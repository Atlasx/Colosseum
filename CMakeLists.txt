cmake_minimum_required(VERSION 3.20)

project(Colosseum VERSION 0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(source/vendor/glfw)

add_subdirectory(source/Engine)
add_subdirectory(source/Main)
