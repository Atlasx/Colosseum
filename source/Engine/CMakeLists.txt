set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(COMPILE_WARNING_AS_ERROR true)

set(SOURCES
	src/Engine.cpp
	src/Globals.cpp

	# Systems
	src/Systems/EngineSystem.cpp
	src/Systems/ResourceSystem.cpp
	src/Systems/InputSystem.cpp
	src/Systems/EventSystem.cpp
	src/Systems/RenderSystem.cpp
	src/Systems/LogSystem.cpp

	# World
	src/Systems/WorldSystem.cpp
	src/Systems/World/EntitySubsystem.cpp
	src/Systems/World/ComponentSubsystem.cpp

	# Components
	#src/Components/RenderComponent.cpp

	# Input
	src/Input/InputAction.cpp

	# Editor
	src/GUI/Editor.cpp

	# Debug
	src/Systems/Debug/EventSystemDebug.cpp
	src/Systems/Debug/InputSystemDebug.cpp
	src/Systems/Debug/LogSystemDebug.cpp
	src/Systems/Debug/WorldSystemDebug.cpp
)

set(INCLUDES
	include/Engine.h
	include/stdlibincl.h
	include/Globals.h

	# Systems
	include/Systems/EngineSystem.h
	include/Systems/ResourceSystem.h
	include/Systems/InputSystem.h
	include/Systems/EventSystem.h
	include/Systems/RenderSystem.h
	include/Systems/LogSystem.h

	# World
	include/Systems/WorldSystem.h
	include/Systems/World/EntitySubsystem.h
	include/Systems/World/ComponentSubsystem.h

	# Components
	include/Components/RenderComponent.h

	# Input
	include/Input/Input.h
	include/Input/InputAction.h

	# Editor
	include/GUI/Editor.h
	include/GUI/DebugGUI.h
	include/GUI/FrameCounter.h

	# Debug
	include/Systems/Debug/EventSystemDebug.h
	include/Systems/Debug/InputSystemDebug.h
	include/Systems/Debug/LogSystemDebug.h
	include/Systems/Debug/WorldSystemDebug.h
)

# Setup source group to mimic file structure
foreach(source IN LISTS SOURCES INCLUDES)
    get_filename_component(source_path "${source}" PATH)
    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
    source_group("${source_path_msvc}" FILES "${source}")
endforeach()

find_package(OpenGL REQUIRED)

add_library(Engine STATIC ${SOURCES} ${INCLUDES})

target_link_libraries(Engine PUBLIC
	Common
	OpenGL::GL
	glad
	ImGui
	glfw
	RenderLib
)

target_include_directories(Engine PRIVATE ${CMAKE_SOURCE_DIR}/vendor/imgui)
target_compile_definitions(Engine PRIVATE HAS_IMGUI)


target_include_directories(Engine PUBLIC 
	${INCLUDE_DIR}
	${CMAKE_SOURCE_DIR}/source/vendor/glm
)

add_compile_options(/showIncludes)

target_compile_definitions(Engine PRIVATE 
	$<$<CONFIG:Release>:CRELEASE>
	$<$<CONFIG:Debug>:CDEBUG>
)