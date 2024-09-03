set(SOURCES
	main.cpp
)

add_executable(Main ${SOURCES})

target_link_libraries(Main PRIVATE
	Engine
	glfw
)
