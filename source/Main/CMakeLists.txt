set(SOURCES
	main.cpp
	stb_image.h
)

add_executable(Main ${SOURCES})


target_link_libraries(Main PRIVATE
	Engine
)

