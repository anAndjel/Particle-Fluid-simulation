@echo off
REM Simple build script for Particle Fluid Simulation
REM This script compiles the project using MSBuild or g++

setlocal enabledelayedexpansion

echo === Building Particle Fluid Simulation ===
echo.

REM Check if MSBuild is available (Visual Studio)
where msbuild >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo Found MSBuild, using Visual Studio build...
    echo.
    msbuild fluidsim.sln /p:Configuration=Release /p:Platform=x64 /m
    if !ERRORLEVEL! EQU 0 (
        echo.
        echo === Build successful! ===
        echo Executable: x64\Release\fluidsim.exe
        goto :end
    ) else (
        echo.
        echo === Build failed! ===
        exit /b 1
    )
)

REM Fallback to g++ if available
where g++ >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo MSBuild not found, trying g++...
    echo.

    set BUILD_DIR=build
    set OUTPUT_NAME=fluidsim.exe
    set SRC_DIR=fluidsim

    REM Compiler settings
    set CXXFLAGS=-std=c++17 -O2 -Wall -DIMGUI_IMPL_OPENGL_LOADER_GL3W

    REM Include directories (adjust these paths to match your system)
    set INCLUDES=-IC:\libs\imgui -IC:\libs\imgui\backends -IC:\libs\glfw-3.4.bin.WIN64\include -IC:\libs\implot-master\implot-master

    REM Library directories and libraries
    set LIB_DIRS=-LC:\libs\glfw-3.4.bin.WIN64\lib-vc2022
    set LIBS=-lglfw3 -lopengl32 -lgdi32

    REM Create build directory
    if not exist !BUILD_DIR! mkdir !BUILD_DIR!

    echo Compiling sources...

    g++ !CXXFLAGS! !INCLUDES! ^
        !SRC_DIR!\main.cpp ^
        C:\libs\imgui\imgui.cpp ^
        C:\libs\imgui\imgui_demo.cpp ^
        C:\libs\imgui\imgui_draw.cpp ^
        C:\libs\imgui\imgui_tables.cpp ^
        C:\libs\imgui\imgui_widgets.cpp ^
        C:\libs\imgui\backends\imgui_impl_glfw.cpp ^
        C:\libs\imgui\backends\imgui_impl_opengl3.cpp ^
        C:\libs\implot-master\implot-master\implot.cpp ^
        C:\libs\implot-master\implot-master\implot_demo.cpp ^
        C:\libs\implot-master\implot-master\implot_items.cpp ^
        !LIB_DIRS! !LIBS! -o !BUILD_DIR!\!OUTPUT_NAME!

    if !ERRORLEVEL! EQU 0 (
        echo.
        echo === Build successful! ===
        echo Executable: !BUILD_DIR!\!OUTPUT_NAME!
        goto :end
    ) else (
        echo.
        echo === Build failed! ===
        exit /b 1
    )
)

echo ERROR: Neither MSBuild nor g++ found!
echo Please install Visual Studio or MinGW/MSYS2 with g++
exit /b 1

:end
echo.
echo Build complete.
