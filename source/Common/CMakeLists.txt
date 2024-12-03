set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(SOURCES 
	src/Array.cpp
)

set(INCLUDES 
	include/Array.h
)

source_group("Source Files" FILES ${SOURCES})
source_group("Header Files" FILES ${INCLUDES})

add_library(Common STATIC ${SOURCES} ${INCLUDES})

target_include_directories(Common PUBLIC ${INCLUDE_DIR})
