@echo off
REM Unity Reflection Viewer Build Script for Windows

echo ==================================================
echo Unity Reflection Viewer - Build Script (Windows)
echo ==================================================

REM Check if ImGui exists
if not exist "external\imgui" (
    echo ERROR: ImGui not found!
    echo.
    echo Please download ImGui:
    echo   cd external
    echo   git clone https://github.com/ocornut/imgui.git
    echo.
    pause
    exit /b 1
)

echo [OK] ImGui found

REM Check for CMake
where cmake >nul 2>nul
if %errorlevel% neq 0 (
    echo ERROR: cmake is not installed
    echo Please install CMake from https://cmake.org/download/
    pause
    exit /b 1
)

echo [OK] CMake found

REM Create build directory
echo.
echo Creating build directory...
if not exist "build" mkdir build
cd build

REM Run CMake
echo.
echo Running CMake...
cmake .. -G "Visual Studio 16 2019"
if %errorlevel% neq 0 (
    echo ERROR: CMake configuration failed
    pause
    exit /b 1
)

echo [OK] CMake configuration successful

REM Build
echo.
echo Building project (Release)...
cmake --build . --config Release
if %errorlevel% neq 0 (
    echo ERROR: Build failed
    pause
    exit /b 1
)

echo.
echo ==================================================
echo Build successful!
echo ==================================================
echo.
echo To run the viewer:
echo   cd build\Release
echo   UnityReflectionViewer.exe
echo.
pause
