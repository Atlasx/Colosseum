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

# Define a static library for ImGui
add_library(ImGui STATIC
    source/vendor/imgui/imgui.cpp
    source/vendor/imgui/imgui_draw.cpp
    source/vendor/imgui/imgui_tables.cpp
    source/vendor/imgui/imgui_widgets.cpp
    source/vendor/imgui/imgui_demo.cpp
    source/vendor/imgui/backends/imgui_impl_glfw.cpp
    source/vendor/imgui/backends/imgui_impl_opengl3.cpp
)

# Include directories for ImGui
target_include_directories(ImGui PUBLIC 
    source/vendor/imgui
    source/vendor/imgui/backends
    source/vendor/glfw/include
    source/vendor/glad/include
)

# Link against GLFW and OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(ImGui PRIVATE glfw OpenGL::GL glad)

set_target_properties(ImGui PROPERTIES FOLDER ${VENDOR_DIR})

add_subdirectory(source/Common)
add_subdirectory(source/RenderLib)
add_subdirectory(source/Engine)
add_subdirectory(source/Main)
