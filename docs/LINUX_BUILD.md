# Linux LV2 Build Guide for BULLsEYE

This document provides comprehensive instructions for building BULLsEYE as an LV2 plugin on Linux systems.

## Overview

BULLsEYE is a LUFS-I gated loudness meter with true peak detection, built using JUCE. This guide covers building the LV2 plugin format for Linux distribution.

## System Requirements

### Minimum Hardware

- CPU: x86_64 or ARM64 processor
- RAM: 4 GB minimum (8 GB recommended)
- Storage: 2 GB free space
- Graphics: OpenGL 3.3+ compatible GPU (for plugin UI)

### Supported Distributions

- Ubuntu 20.04 LTS and later
- Debian 11 (Bullseye) and later
- Fedora 35 and later
- Arch Linux
- openSUSE Leap 15.4+

## Dependencies

### JUCE Linux Dependencies

Install the required development packages:

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    pkg-config \
    libcurl4-gnutls-dev \
    libncurses5-dev \
    libjack-jackd2-dev \
    libasound2-dev \
    libudev-dev \
    libx11-dev \
    libxcomposite-dev \
    libxcursor-dev \
    libxext-dev \
    libxinerama-dev \
    libxrandr-dev \
    libxrender-dev \
    libxtst-dev \
    libgl1-mesa-dev \
    libfreetype6-dev \
    libfontconfig1-dev
```

**Fedora:**
```bash
sudo dnf install -y \
    gcc-c++ \
    cmake \
    pkgconfig \
    curl-devel \
    ncurses-devel \
    jack-audio-connection-kit-devel \
    alsa-lib-devel \
    systemd-devel \
    libX11-devel \
    libXcomposite-devel \
    libXcursor-devel \
    libXext-devel \
    libXinerama-devel \
    libXrandr-devel \
    libXrender-devel \
    libXtst-devel \
    mesa-libGL-devel \
    freetype-devel \
    fontconfig-devel
```

**Arch Linux:**
```bash
sudo pacman -S --needed \
    base-devel \
    cmake \
    pkgconf \
    curl \
    ncurses \
    jack2 \
    alsa-lib \
    systemd \
    libx11 \
    libxcomposite \
    libxcursor \
    libxext \
    libxinerama \
    libxrandr \
    libxrender \
    libxtst \
    mesa \
    freetype2 \
    fontconfig
```

### LV2 SDK Requirements

The LV2 SDK is required for LV2 plugin builds. Install via:

**Ubuntu/Debian:**
```bash
sudo apt-get install -y \
    lilv-utils \
    serd-2 \
    sord-2 \
    sratom-2 \
    lv2-dev
```

**Fedora:**
```bash
sudo dnf install -y \
    lilv-devel \
    serd-devel \
    sord-devel \
    sratom-devel
```

**Manual Installation (All Distributions):**
```bash
# Create a local installation directory
mkdir -p ~/lv2
cd ~/lv2

# Clone and build LV2 dependencies
git clone https://gitlab.com/lv2/serd.git
git clone https://gitlab.com/lv2/sord.git
git clone https://gitlab.com/lv2/sratom.git
git clone https://gitlab.com/lv2/lilv.git

# Build each component
for dir in serd sord sratom lilv; do
    cd $dir
    mkdir -p build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=~/lv2
    make -j$(nproc)
    make install
    cd ../..
done

# Add to PKG_CONFIG_PATH
export PKG_CONFIG_PATH=~/lv2/lib/pkgconfig:$PKG_CONFIG_PATH
export LD_LIBRARY_PATH=~/lv2/lib:$LD_LIBRARY_PATH
```

## Project Setup

### Clone the Repository

```bash
git clone https://github.com/your-username/BULLsEYE-JSFX-CPP.git
cd BULLsEYE-JSFX-CPP
```

### Initialize JUCE Submodule

```bash
git submodule update --init --recursive
```

Or if using a local JUCE installation:
```bash
ln -s /path/to/your/JUCE modules/JUCE
```

## Build Instructions

### Configure with CMake

```bash
# Create build directory
mkdir -p build
cd build

# Configure with LV2 enabled
cmake .. -DCMAKE_BUILD_TYPE=Release
```

### Build Command

```bash
cmake --build . -j$(nproc)
```

The `-j$(nproc)` flag enables parallel compilation using all available CPU cores.

### Build Options

**Custom Installation Directory:**
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local
```

**Debug Build:**
```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug
```

**Static Linking (Advanced):**
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release -DJUCE_USE_LTO=1
```

## Build Output

After a successful build, the LV2 bundle will be located at:

```
build/BULLsEYE.lv2/
├── BULLsEYE           # Shared library (the plugin)
├── manifest.ttl        # Plugin metadata
├── preset.ttl         # Preset definitions (if any)
└── ui/                # UI resources (if any)
```

## Installation

### Local Installation

Copy the LV2 bundle to your local LV2 directory:

```bash
mkdir -p ~/.lv2
cp -r build/BULLsEYE.lv2 ~/.lv2/
```

### System-Wide Installation

For system-wide access (requires root privileges):

```bash
sudo cp -r build/BULLsEYE.lv2 /usr/lib/lv2/
```

### Verify Installation

```bash
# Check if the plugin is recognized
lv2ls | grep BULLsEYE

# Or use a plugin scanner
jalv -n BULLsEYE --help
```

## DAW-Specific Paths

Different DAWs may use different LV2 paths:

| DAW | LV2 Path |
|-----|----------|
| Ardour | `~/.lv2/` or `/usr/lib/lv2/` |
| REAPER | `~/.REAPER/Plugins/` |
| Bitwig Studio | `~/.Bitwig/Extensions/` |
| Qtractor | `~/.lv2/` |
| Carla | `~/.lv2/` or system LV2 path |

### Adding Custom Paths

If your DAW uses a non-standard location, set the LV2_PATH environment variable:

```bash
export LV2_PATH="$HOME/.lv2:$HOME/.reaper/Plugins:/usr/lib/lv2"
```

Add this to your `~/.bashrc` or `~/.zshrc` for persistence.

## Troubleshooting

### Build Fails with LV2 Errors

Ensure LV2 development packages are installed:
```bash
pkg-config --modversion lilv
```

If not found, install manually as shown in the LV2 SDK Requirements section.

### Plugin Not Recognized by DAW

1. Check that the `.lv2` bundle has the correct structure:
   ```
   BULLsEYE.lv2/
   ├── BULLsEYE (executable shared library)
   └── manifest.ttl (metadata file)
   ```

2. Verify file permissions:
   ```bash
   chmod -R 755 ~/.lv2/BULLsEYE.lv2
   chmod +x ~/.lv2/BULLsEYE.lv2/BULLsEYE
   ```

3. Check DAW plugin paths are configured correctly.

### UI Not Rendering

Ensure you have the required graphics libraries:
```bash
# Ubuntu/Debian
sudo apt-get install libgl1-mesa-dev libxcomposite-dev libxrandr-dev
```

### Audio Cracks or Pops

Try increasing the JACK buffer size:
```bash
# Edit ~/.jackdrc or start jackd with larger buffer
jackd -p 1024 -n 2 -r -d alsa
```

## Continuous Integration

This project includes GitHub Actions workflows for automated Linux builds. See `.github/workflows/linux-build.yml` for configuration details.

## Performance Optimization

### Compiler Optimizations

For maximum performance, use these CMake options:
```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_C_FLAGS="-O3 -march=native -mtune=native" \
    -DCMAKE_CXX_FLAGS="-O3 -march=native -mtune=native"
```

### JUCE Settings

Disable asserts and debugging features for production builds:
```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DJUCE_ENABLE_MODULE_CHECKING=0 \
    -DJUCE_VST3_CAN_REPLACE_VST2=0
```

## Additional Resources

- [JUCE Documentation](https://juce.com/learn/)
- [LV2 Specification](https://lv2plug.in/)
- [JUCE LV2 Tutorial](https://github.com/jUCE-Community/juce-plugin-examples)
- [Lilv Library Documentation](https://gitlab.com/lv2/lilv/-/wikis/home)

## Version Information

- **BULLsEYE Version:** 1.2.1
- **JUCE Version:** See `modules/JUCE/CMakeLists.txt`
- **LV2 Version:** See installed lilv version
- **Build System:** CMake 3.15+

---

**Last Updated:** 2026-02-06
**Maintained By:** BULLsEYE Development Team
