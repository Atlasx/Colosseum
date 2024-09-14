cmake_minimum_required(VERSION 3.20)

project(Colosseum
	VERSION 0.0
	LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(VENDOR_DIR vendor)

add_definitions(-DASSETS_PATH="${CMAKE_SOURCE_DIR}/assets")

add_subdirectory(source/vendor/glad)
add_subdirectory(source/vendor/glfw)
add_subdirectory(source/vendor/imgui)

add_subdirectory(source/RenderLib)
add_subdirectory(source/Engine)
add_subdirectory(source/Main)
