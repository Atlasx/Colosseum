@echo off

REM Check if the user provided a configuration parameter
if "%1"=="" (
    echo Usage: build.bat [Debug^|Release]
    exit /b 1
)

REM Set the scripts dir and build dir based on the configuration
set SCRIPTS_DIR=%~dp0
set CONFIGURATION=%1

REM Validate configuration parameter TODO: get these from somewhere else less "manual"
if /I "%CONFIGURATION%" neq "Debug" if /I "%CONFIGURATION%" neq "Release" (
    echo Invalid configuration. Use Debug or Release.
    exit /b 1
)

REM Navigate to the project root directory
cd /d "%SCRIPTS_DIR%.."

REM Normalize PROJECT_DIR path
SET PROJECT_DIR=%CD%

REM Set the build directory
SET BUILD_DIR=%PROJECT_DIR%\build\%CONFIGURATION%

REM Clean up existing build files
IF EXIST "%BUILD_DIR%" (
    rmdir /s /q "%BUILD_DIR%"
)

REM Create the build directory
mkdir "%BUILD_DIR%"

REM Navigate to the build directory
cd /d "%BUILD_DIR%"

REM Run CMake to configure the project
cmake -G "Visual Studio 17 2022" "%PROJECT_DIR%"

REM Build the project using the specified configuration
cmake --build . --config %CONFIGURATION%

REM Make our destination bin directory if needed
SET BIN_DIR=%PROJECT_DIR%\bin\%CONFIGURATION%

IF NOT EXIST "%BIN_DIR%" (
    md "%BIN_DIR%" 2> nul
)

REM Move exe out of build
FOR /R "%BUILD_DIR%" %%f IN (Colosseum.exe) DO (
    echo Found executable: %%f
    move /Y "%%f" "%BIN_DIR%"
)

cd /d "%PROJECT_ROOT%"

REM Delete build files
rmdir /s /q "%BUILD_DIR%"