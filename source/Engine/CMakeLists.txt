set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(SOURCES
	src/Engine.cpp

	# Systems
	src/Systems/EngineSystem.cpp
	src/Systems/ResourceSystem.cpp
	src/Systems/InputSystem.cpp
	src/Systems/EventSystem.cpp
)

set(INCLUDES
	include/Engine.h

	# Systems
	include/Systems/EngineSystem.h
	include/Systems/ResourceSystem.h
	include/Systems/InputSystem.h
	include/Systems/EventSystem.h
)

source_group("Source Files" FILES ${SOURCES})
source_group("Header Files" FILES ${INCLUDES})

add_library(Engine STATIC ${SOURCES} ${INCLUDES})

target_include_directories(Engine PUBLIC ${INCLUDE_DIR})

target_compile_definitions(Engine PRIVATE 
	$<$<CONFIG:Release>:CRELEASE>
	$<$<CONFIG:Debug>:CDEBUG>
)