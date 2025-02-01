# Colosseum

A C++ Game Engine, built on cmake using GLFW, GLM, ImGui, and OpenGL


## cmake

```
cd [project_directory]
git submodule update --init --recursive
cmake .
mkdir build
cmake --build build --config [Debug|Release]
cd bin/[Debug|Release]
Main.exe
```

## Setup

1. Clone project to your directory
2. Navigate to scripts directory
3. Run build.bat with either Debug or Release (default)
4. Run Main.exe in bin directory

## TODO

- Allow for ComponentHandles to store type
	- Write translation (maybe in register component) to convert to bits
- Write a few components
- Hook up rendering to iterate over mesh/transform components
- Hook up physics to iterate over rigidbody components
- Look into archetype arrays instead of SoA