#!/bin/bash

# ========================================================================
# JSFX to JUCE Migration - Build Script
# ========================================================================

set -e

echo "======================================"
echo "Building Your Plugin"
echo "======================================"

# Navigate to project directory
cd "$(dirname "$0")"

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Configure with CMake
echo ""
echo "Configuring with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
echo ""
echo "Building..."
cmake --build . --config Release -j$(sysctl -n hw.ncpu 2>/dev/null || nproc)

# Verify build artifacts
echo ""
echo "======================================"
echo "Build artifacts:"
echo "======================================"

if [ -d "IntelliMute_artefacts" ]; then
    echo "VST3:"
    ls -la IntelliMute_artefacts/VST3/*.vst3 2>/dev/null || echo "  None found"
    echo ""
    echo "AU:"
    ls -la IntelliMute_artefacts/AU/*.component 2>/dev/null || echo "  None found"
fi

echo ""
echo "======================================"
echo "Build complete!"
echo "======================================"
