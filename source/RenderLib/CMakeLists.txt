

set(SOURCES
	include/Shader.h
	src/Shader.cpp

	src/Mesh.cpp
)

set(INCLUDES

	include/Mesh.h
)

# Setup source group to mimic file structure
foreach(source IN LISTS SOURCES INCLUDES)
    get_filename_component(source_path "${source}" PATH)
    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
    source_group("${source_path_msvc}" FILES "${source}")
endforeach()

add_library(RenderLib STATIC ${SOURCES})

find_package(OpenGL REQUIRED)

target_link_libraries(RenderLib PUBLIC
	OpenGL::GL
	glad
)

target_include_directories(RenderLib PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_SOURCE_DIR}/source/vendor/glm
)

file(GLOB_RECURSE SHADER_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/../../assets/shaders/*.vert"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../assets/shaders/*.frag"
)

source_group("Shaders" FILES ${SHADER_FILES})

target_sources(RenderLib PRIVATE ${SHADER_FILES})