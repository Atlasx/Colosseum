set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(SOURCES 
	src/Array.cpp
)

set(INCLUDES 
	include/Handle.h
	include/Array.h
	include/Random.h
	include/ObjectPool.h
)

# Setup source group to mimic file structure
foreach(source IN LISTS SOURCES INCLUDES)
    get_filename_component(source_path "${source}" PATH)
    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
    source_group("${source_path_msvc}" FILES "${source}")
endforeach()

add_library(Common STATIC ${SOURCES} ${INCLUDES})

target_include_directories(Common PUBLIC ${INCLUDE_DIR})
