set(SOURCES
	engine.cpp
	engine.h
)

add_library(Engine STATIC ${SOURCES})

target_include_directories(Engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})