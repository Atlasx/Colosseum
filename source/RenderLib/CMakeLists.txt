set(SOURCES
	include/Shader.h
	src/Shader.cpp
)

add_library(RenderLib STATIC ${SOURCES})

find_package(OpenGL REQUIRED)

target_link_libraries(RenderLib PUBLIC
	OpenGL::GL
	glad
)

target_include_directories(RenderLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB_RECURSE SHADER_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/../../assets/shaders/*.vert"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../assets/shaders/*.frag"
)

source_group("Shaders" FILES ${SHADER_FILES})

target_sources(RenderLib PRIVATE ${SHADER_FILES})