set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(SOURCES
	src/Engine.cpp
	src/Globals.cpp

	# Editor
	src/GUI/Editor.cpp

	# Systems
	src/Systems/EngineSystem.cpp
	src/Systems/ResourceSystem.cpp
	src/Systems/InputSystem.cpp
	src/Systems/EventSystem.cpp
	src/Systems/RenderSystem.cpp
	src/Systems/ObjectSystem.cpp
	src/Systems/LogSystem.cpp
)

set(INCLUDES
	include/Engine.h
	include/Globals.h

	# Systems
	include/Systems/EngineSystem.h
	include/Systems/ResourceSystem.h
	include/Systems/InputSystem.h
	include/Systems/EventSystem.h
	include/Systems/RenderSystem.h
	include/Systems/ObjectSystem.h
	include/Systems/LogSystem.h

	#Editor
	include/GUI/Editor.h
)

source_group("Source Files" FILES ${SOURCES})
source_group("Header Files" FILES ${INCLUDES})

find_package(OpenGL REQUIRED)

add_library(Engine STATIC ${SOURCES} ${INCLUDES})

target_link_libraries(Engine PUBLIC
	Common
	OpenGL::GL
	glad
	ImGui
	glfw
)

target_include_directories(Engine PRIVATE ${CMAKE_SOURCE_DIR}/vendor/imgui)
target_compile_definitions(Engine PRIVATE HAS_IMGUI)

target_include_directories(Engine PUBLIC ${INCLUDE_DIR})

target_compile_definitions(Engine PRIVATE 
	$<$<CONFIG:Release>:CRELEASE>
	$<$<CONFIG:Debug>:CDEBUG>
)