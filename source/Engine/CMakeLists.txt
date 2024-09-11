set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(SOURCES
	src/Engine.cpp
)

set(INCLUDES
	include/Engine.h
	include/EngineSystem.h
)

source_group("Source Files" FILES ${SOURCES})
source_group("Header Files" FILES ${INCLUDES})

add_library(Engine STATIC ${SOURCES} ${INCLUDES})

target_include_directories(Engine PUBLIC ${INCLUDE_DIR})