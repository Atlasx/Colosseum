set(SOURCES
	main.cpp
)

add_executable(Main ${SOURCES})

find_package(OpenGL REQUIRED)

target_link_libraries(Main PRIVATE
	Engine
	glfw
	OpenGL::GL
)
