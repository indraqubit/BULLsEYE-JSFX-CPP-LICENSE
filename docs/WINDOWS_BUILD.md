# Windows VST3 Build Guide

This guide documents the Windows VST3 build configuration and setup for the BULLsEYE plugin.

## Prerequisites

### Visual Studio Requirements

**Visual Studio 2019 or 2022** with the following components:

- **Desktop development with C++** workload
- **C++ CMake tools for Windows**
- **Windows SDK** (10.0.19041.0 or later recommended)
- **MSVC v142 or v143** C++ compiler

**Visual Studio 2022 (Recommended):**
- Download from: https://visualstudio.microsoft.com/downloads/
- Select "Community" (free) or "Professional/Enterprise"
- During installation, select:
  - Desktop development with C++
  - C++ CMake tools for Windows
  - Windows 10/11 SDK

**Visual Studio 2019:**
- Download from: https://docs.microsoft.com/en-us/visualstudio/releases/2019
- MSVC v142 compiler required

### JUCE Windows Dependencies

JUCE on Windows requires:

1. **JUCE Framework** (included as submodule in `modules/JUCE`)
2. **VST3 SDK** (bundled with JUCE)
3. **Windows SDK** (via Visual Studio installation)
4. **CMake 3.15 or later**

No additional dependencies are required for basic VST3 builds. JUCE handles all audio processing dependencies internally.

## Build Configuration

### Current CMakeLists.txt Settings

The project is configured for VST3 builds with the following settings in `CMakeLists.txt`:

```cmake
juce_add_plugin(BULLsEYE
    COMPANY_NAME "IQ"
    PLUGIN_MANUFACTURER_CODE IQDE
    PLUGIN_CODE BULL
    BUNDLE_ID "com.iq.bullseye"
    FORMATS VST3 AU
    PRODUCT_NAME "BULLsEYE"
    DESCRIPTION "LUFS-I Gated Loudness Meter with True Peak Detection"
    VERSION 1.2.1
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER "IQ"
)
```

Platform-specific Windows settings:

```cmake
if(WIN32)
    set_target_properties(BULLsEYE PROPERTIES
        WIN32_EXECUTABLE FALSE
    )
endif()
```

### Build Commands

**Using Visual Studio Generator (VS 2022):**

```bash
# Create build directory
mkdir build
cd build

# Configure with Visual Studio 2022
cmake .. -G "Visual Studio 17 2022" -A x64

# Build in Release mode
cmake --build . --config Release

# Alternatively, build in Debug mode
cmake --build . --config Debug
```

**Using Visual Studio Generator (VS 2019):**

```bash
# Configure with Visual Studio 2019
cmake .. -G "Visual Studio 16 2019" -A x64

# Build
cmake --build . --config Release
```

**Using Ninja Generator (faster builds):**

```bash
# Install Ninja from https://ninja-build.org/
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

### Build Output

After a successful build, the VST3 plugin will be located at:

```
build/BULLsEYE.vst3/Contents/x64-win/BULLsEYE.vst3
```

For VST3 format, JUCE creates a `.vst3` bundle directory structure.

## Project Structure

```
BULLsEYE-JSFX-CPP/
├── CMakeLists.txt
├── Source/
│   ├── PluginProcessor.cpp/h
│   ├── PluginEditor.cpp/h
│   ├── SSOT/
│   │   ├── ModelSSOT.h
│   │   ├── UISSOT.h
│   │   ├── ProcessorSSOT.h
│   │   └── DSPSSOT.h
│   ├── DSP/
│   │   └── BULLsEYEProcessor.h
│   └── Components/
│       ├── StatusDisplayComponent.cpp/h
│       ├── ModeSelectorComponent.cpp/h
│       └── LEDStripMeter.cpp/h
├── modules/
│   └── JUCE/ (submodule)
└── build/ (created during build)
```

## Troubleshooting

### Common Issues

**1. CMake cannot find Visual Studio:**
```bash
# Ensure Visual Studio is installed and CMake tools are selected
# Open "Developer Command Prompt for VS 2022" and run cmake from there
```

**2. Windows SDK not found:**
```bash
# Install Windows SDK via Visual Studio Installer
# Or download from: https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/
```

**3. JUCE submodule not initialized:**
```bash
git submodule update --init --recursive
```

**4. Build fails with linker errors:**
```bash
# Ensure you have enough system resources
# Try building with single thread: cmake --build . --config Release -j 1
```

### Debug Build Issues

For debug builds, ensure you have the corresponding debug runtime libraries installed with Visual Studio. Debug builds require additional setup for JUCE plugins due to runtime checks.

## Advanced Configuration

### Custom VST3 Location

To change the build output location:

```cmake
set_target_properties(BULLsEYE PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build/output"
)
```

### Plugin Symbols

Debug builds include debug symbols useful for troubleshooting:

```bash
cmake --build . --config Debug
```

Debug builds will output PDB files alongside the VST3 bundle.

## CI/CD Integration

See `.github/workflows/windows-build.yml` for automated Windows build configuration using GitHub Actions.

## Next Steps

1. **Build the plugin** using the commands above
2. **Test the plugin** in a DAW (Cubase, Reaper, Studio One, etc.)
3. **Package for distribution** using Inno Setup or similar installer

For installation instructions, see [INSTALLATION.md](INSTALLATION.md).

## Cross-Compilation from macOS to Windows

Cross-compiling JUCE VST3 plugins from macOS to Windows is **not officially supported** by JUCE due to platform-specific dependencies and runtime requirements. JUCE plugins require native platform libraries and headers that differ between operating systems.

### Why Cross-Compilation Is Not Recommended

1. **Windows-specific runtime dependencies**: JUCE plugins on Windows require Windows-specific DLLs and runtime libraries that are not available on macOS
2. **Different ABI conventions**: macOS and Windows have different application binary interfaces
3. **Platform-specific SDKs**: JUCE requires Windows SDK headers and libraries
4. **Testing complexity**: Cross-compiled builds cannot be properly tested on the host system

### Alternative Approaches

**Option 1: Native Windows Build (Recommended)**

The most reliable approach is to build natively on Windows:
- Set up a Windows virtual machine (VMware, Parallels, VirtualBox)
- Use Boot Camp for Intel Mac users
- Use GitHub Actions for automated builds

**Option 2: GitHub Actions CI/CD**

For automated Windows builds without local hardware:

```yaml
# See .github/workflows/windows-build.yml for complete configuration
```

Benefits:
- No local Windows setup required
- Consistent build environment
- Automated testing and release packaging
- Free for open source projects

**Option 3: Windows Cloud Build Services**

- **AppVeyor**: Windows CI/CD with Visual Studio
- **Azure Pipelines**: Microsoft-hosted Windows agents
- **GitHub Actions**: Windows runners available on all plans

### Cloud Build Setup (GitHub Actions Example)

```yaml
name: Windows Build CI
on: [push, pull_request]

jobs:
  build:
    runs-on: windows-latest
    steps:
    - uses: actions/checkout@v4
      with:
        submodules: true
    - uses: cmake-actions/setup-cmake@v1
    - run: cmake -B build -G "Visual Studio 17 2022"
    - run: cmake --build build --config Release
```

### Virtual Machine Setup

**Using UTM (Apple Silicon Mac):**
1. Download Windows 11 ARM ISO
2. Install via UTM with ARM virtualization
3. Install Visual Studio 2022
4. Clone repository and build natively

**Using Parallels/VMware (Intel Mac):**
1. Install Windows 10/11 x64
2. Install Visual Studio 2022
3. Enable folder sharing for source access
4. Build natively in the VM

### Build Server Options

For automated production builds:

| Service | Windows Runners | Pricing |
|---------|-----------------|---------|
| GitHub Actions | Free minutes available | Free for open source |
| AppVeyor | Included | Free for open source |
| Azure Pipelines | Microsoft-hosted | Free tier available |
| CircleCI | Windows 2022 image | Paid plans |

### Recommendations

1. **Development**: Use native Windows VM or Boot Camp for development builds
2. **CI/CD**: Use GitHub Actions for automated testing and release builds
3. **Distribution**: Package builds from CI for distribution to avoid user setup
4. **Testing**: Test on actual Windows hardware when possible before release

For CI/CD workflow details, see `.github/workflows/windows-build.yml`.
