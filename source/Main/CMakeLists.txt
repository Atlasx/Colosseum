set(SOURCES
	main.cpp
	stb_image.h
)

add_executable(Main ${SOURCES})

find_package(OpenGL REQUIRED)

target_link_libraries(Main PRIVATE
	Engine
	OpenGL::GL
	glad
	ImGui
	glfw
	RenderLib
)

target_include_directories(Main PRIVATE ${CMAKE_SOURCE_DIR}/vendor/imgui)
target_compile_definitions(Main PRIVATE HAS_IMGUI)
