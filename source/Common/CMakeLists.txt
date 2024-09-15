set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(SOURCES 
	src/DependencyGraph.cpp
)

set(HEADERS 
	include/DependencyGraph.h
)

source_group("Source Files" FILES ${SOURCES})
source_group("Header Files" FILES ${INCLUDES})

add_library(Common STATIC ${SOURCES} ${HEADERS})

target_include_directories(Common PUBLIC ${INCLUDE_DIR})
