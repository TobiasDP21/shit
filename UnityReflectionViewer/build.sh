#!/bin/bash

# Unity Reflection Viewer Build Script

set -e  # Exit on error

echo "=================================================="
echo "Unity Reflection Viewer - Build Script"
echo "=================================================="

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Check if ImGui exists
if [ ! -d "external/imgui" ]; then
    echo -e "${RED}ERROR: ImGui not found!${NC}"
    echo ""
    echo "Please download ImGui:"
    echo "  cd external"
    echo "  git clone https://github.com/ocornut/imgui.git"
    echo ""
    exit 1
fi

echo -e "${GREEN}✓ ImGui found${NC}"

# Check for required tools
command -v cmake >/dev/null 2>&1 || {
    echo -e "${RED}ERROR: cmake is not installed${NC}"
    exit 1
}

echo -e "${GREEN}✓ CMake found${NC}"

# Create build directory
echo ""
echo "Creating build directory..."
mkdir -p build
cd build

# Run CMake
echo ""
echo "Running CMake..."
cmake .. || {
    echo -e "${RED}ERROR: CMake configuration failed${NC}"
    exit 1
}

echo -e "${GREEN}✓ CMake configuration successful${NC}"

# Build
echo ""
echo "Building project..."
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4) || {
    echo -e "${RED}ERROR: Build failed${NC}"
    exit 1
}

echo ""
echo -e "${GREEN}=================================================="
echo "Build successful!"
echo "==================================================${NC}"
echo ""
echo "To run the viewer:"
echo "  cd build"
echo "  ./UnityReflectionViewer"
echo ""
