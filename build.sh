#!/bin/bash

# ========================================================================
# BULLsEYE - Build Script
# ========================================================================

set -e

echo "======================================"
echo "Building BULLsEYE"
echo "======================================"

# Navigate to project directory
cd "$(dirname "$0")"

# Check for JUCE
if [ ! -d "modules/JUCE" ]; then
    echo ""
    echo "ERROR: JUCE not found!"
    echo ""
    echo "Please set up JUCE before building:"
    echo ""
    echo "Option 1: Clone JUCE submodule"
    echo "  git submodule add https://github.com/juce-framework/JUCE.git modules/JUCE"
    echo "  git submodule update --init --recursive"
    echo ""
    echo "Option 2: Symlink to existing JUCE installation"
    echo "  ln -s /path/to/JUCE modules/JUCE"
    echo ""
    exit 1
fi

# Architecture selection
ARCH=${1:-"universal"}

case "$ARCH" in
    universal)
        ARCHS="x86_64;arm64"
        echo "Building universal binary (Intel + Apple Silicon)"
        ;;
    x86_64)
        ARCHS="x86_64"
        echo "Building for Intel (x86_64) only"
        ;;
    arm64)
        ARCHS="arm64"
        echo "Building for Apple Silicon (ARM64) only"
        ;;
    *)
        echo "Usage: $0 [universal|x86_64|arm64]"
        echo ""
        echo "Options:"
        echo "  universal  - Build for both Intel and Apple Silicon (default)"
        echo "  x86_64     - Build for Intel Macs only"
        echo "  arm64      - Build for Apple Silicon Macs only"
        echo ""
        exit 1
        ;;
esac

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Configure with CMake
echo ""
echo "Configuring with CMake..."
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_OSX_ARCHITECTURES="$ARCHS" \
    -DCMAKE_OSX_DEPLOYMENT_TARGET="12.0"

# Build
echo ""
echo "Building..."
cmake --build . --config Release -j$(sysctl -n hw.ncpu 2>/dev/null || nproc)

# Verify build artifacts
echo ""
echo "======================================"
echo "Build artifacts:"
echo "======================================"

if [ -d "BULLsEYE_artefacts" ]; then
    echo "VST3:"
    ls -la BULLsEYE_artefacts/VST3/*.vst3 2>/dev/null || echo "  None found"
    echo ""
    echo "AU:"
    ls -la BULLsEYE_artefacts/AU/*.component 2>/dev/null || echo "  None found"
fi

# Check binary architecture
echo ""
echo "Binary architecture:"

# Detect platform and set plugin directory
if [[ "$OSTYPE" == "darwin"* ]]; then
    PLUGIN_DIR="${HOME}/Library/Audio/Plug-Ins/VST3"
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    PLUGIN_DIR="${HOME}/.vst3"
elif [[ "$OSTYPE" == "msys" ]]; then
    PLUGIN_DIR="${APPDATA}/Local/Plug-Ins/VST3"
else
    PLUGIN_DIR="${HOME}/.vst3"
fi

VST3_BINARY=$(find "${PLUGIN_DIR}/BULLsEYE.vst3" -name "BULLsEYE" -type f 2>/dev/null | head -1)
if [ -n "$VST3_BINARY" ]; then
    echo "VST3: $VST3_BINARY"
    file "$VST3_BINARY" 2>/dev/null || echo "  Could not determine architecture"
    lipo -info "$VST3_BINARY" 2>/dev/null || echo "  (lipo not available for single-architecture builds)"
else
    echo "  VST3 binary not found in ${PLUGIN_DIR}"
fi

echo ""
echo "======================================"
echo "Build complete!"
echo "======================================"
echo ""
echo "Next steps:"
echo "1. Test the plugin in your DAW"
echo "2. Compare measurements with JSFX reference"
echo ""
echo "Architecture options:"
echo "  ./build.sh universal   - Intel + Apple Silicon (default)"
echo "  ./build.sh x86_64      - Intel only"
echo "  ./build.sh arm64       - Apple Silicon only"
