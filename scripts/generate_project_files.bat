@echo off

REM pop out of scripts folder into our root
cd "%~dp0.."

set SCRIPTS_DIR=%~dp0
set PROJECT_DIR=%SCRIPTS_DIR%..

REM Normalize the PROJECT_DIR path
for %%i in ("%PROJECT_DIR%") do set PROJECT_DIR=%%~fi

set GENERATED_DIR=%PROJECT_DIR%\generated

REM Clean up existing generated files
if exist "%GENERATED_DIR%" (
	rmdir /s /q "%GENERATED_DIR%"
)

mkdir "%GENERATED_DIR%"

cd "%GENERATED_DIR%"

cmake -G "Visual Studio 17 2022" "%PROJECT_DIR%"

cd "%SCRIPTS_DIR%"