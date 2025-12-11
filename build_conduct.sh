#!/bin/bash

# Simple build script for Particle Fluid Simulation
# This script compiles the project using g++ with the necessary dependencies

set -e  # Exit on error

echo "=== Building Particle Fluid Simulation ==="

# Configuration
BUILD_DIR="build"
OUTPUT_NAME="fluidsim.exe"
SRC_DIR="fluidsim"

# Compiler settings
CXX="g++"
CXXFLAGS="-std=c++17 -O2 -Wall -DIMGUI_IMPL_OPENGL_LOADER_GL3W"

# Include directories (adjust these paths to match your system)
INCLUDES="-I/c/libs/imgui -I/c/libs/imgui/backends -I/c/libs/glfw-3.4.bin.WIN64/include -I/c/libs/implot-master/implot-master"

# Library directories and libraries
LIB_DIRS="-L/c/libs/glfw-3.4.bin.WIN64/lib-vc2022"
LIBS="-lglfw3 -lopengl32 -lgdi32"

# Source files
SOURCES=(
    "$SRC_DIR/main.cpp"
    "/c/libs/imgui/imgui.cpp"
    "/c/libs/imgui/imgui_demo.cpp"
    "/c/libs/imgui/imgui_draw.cpp"
    "/c/libs/imgui/imgui_tables.cpp"
    "/c/libs/imgui/imgui_widgets.cpp"
    "/c/libs/imgui/backends/imgui_impl_glfw.cpp"
    "/c/libs/imgui/backends/imgui_impl_opengl3.cpp"
    "/c/libs/implot-master/implot-master/implot.cpp"
    "/c/libs/implot-master/implot-master/implot_demo.cpp"
    "/c/libs/implot-master/implot-master/implot_items.cpp"
)

# Create build directory
mkdir -p "$BUILD_DIR"

echo "Compiling sources..."
echo "CXX: $CXX"
echo "Output: $BUILD_DIR/$OUTPUT_NAME"

# Compile
$CXX $CXXFLAGS $INCLUDES "${SOURCES[@]}" $LIB_DIRS $LIBS -o "$BUILD_DIR/$OUTPUT_NAME"

if [ $? -eq 0 ]; then
    echo ""
    echo "=== Build successful! ==="
    echo "Executable: $BUILD_DIR/$OUTPUT_NAME"
    echo ""
    echo "To run: ./$BUILD_DIR/$OUTPUT_NAME"
else
    echo ""
    echo "=== Build failed! ==="
    exit 1
fi
