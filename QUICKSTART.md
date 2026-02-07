# BULLsEYE - Quick Start Guide for New Team Members

**Last Updated:** 2026-02-07  
**Purpose:** Get up and running in 5 minutes

---

## Quick Start (Choose Your Platform)

### macOS (Easiest - 2 steps)

```bash
# 1. Clone repository
git clone https://github.com/indraqubit/portable-jsfx-to-juce-migration
cd portable-jsfx-to-juce-migration

# 2. Run build script (universal binary by default)
./build.sh

# For specific architectures:
./build.sh universal   # Intel + Apple Silicon (default)
./build.sh x86_64      # Intel only
./build.sh arm64       # Apple Silicon only

# Done! Plugin installed to ~/Library/Audio/Plug-Ins/
```

**Verify universal binary:**
```bash
lipo -info ~/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3/Contents/x86_64/BULLsEYE
# Expected: Architectures in the fat file: x86_64 arm64
```

**Test it:** Open REAPER, Logic, or Ableton → Add BULLsEYE plugin → Play audio

---

### Windows (3 steps)

```bash
# 1. Clone repository
git clone https://github.com/indraqubit/portable-jsfx-to-juce-migration
cd portable-jsfx-to-juce-migration

# 2. Initialize JUCE submodule (IMPORTANT!)
git submodule update --init --recursive

# 3. Build (from Visual Studio 2022 Developer Command Prompt)
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release

# 4. Copy to plugin folder (manual step - see below)
```

**Test it:** Open REAPER, Cubase, or Studio One → Add BULLsEYE plugin → Play audio

---

### Linux (Easiest - 2 steps)

```bash
# 1. Clone repository
git clone https://github.com/indraqubit/portable-jsfx-to-juce-migration
cd portable-jsfx-to-juce-migration

# 2. Run build script
./build.sh

# Done! Plugin installed to ~/.lv2/
```

**Test it:** Open REAPER, Bitwig, or Ardour → Add BULLsEYE plugin → Play audio

---

## Prerequisites

| Platform | What You Need | Where to Get It |
|----------|---------------|------------------|
| **macOS** | Xcode Command Line Tools | Terminal: `xcode-select --install` |
| **Windows** | Visual Studio 2019/2022 | visualstudio.microsoft.com (free Community edition) |
| **Windows** | CMake | Included with Visual Studio, or cmake.org |
| **Linux** | build-essential, cmake | `sudo apt-get install build-essential cmake` |

---

## Common Issues & Fixes

### ❌ "Permission denied" (macOS/Linux)

```bash
chmod +x build.sh
./build.sh
```

### ❌ "JUCE not found" (All platforms)

```bash
git submodule update --init --recursive
```

### ❌ "Could not find Visual Studio" (Windows)

Open **"Developer Command Prompt for VS 2022"** from Start Menu, then run cmake commands.

**OR** specify generator explicitly:
```bash
cmake .. -G "Visual Studio 17 2022" -A x64
```

### ❌ POST_BUILD error on Windows (MacOS paths on Windows)

**Error:** `CMake Error: Could not create symbolic link '/Library/Audio/...'`

**Workaround:** Ignore this error. Build still succeeds.

**To install manually:**
```bash
# After build completes:
copy build\BULLsEYE_artefacts\VST3\BULLsEYE.vst3 "C:\Program Files\Common Files\VST3\"
```

### ❌ Build fails mysteriously

```bash
# Clean and rebuild
rm -rf build/        # macOS/Linux
rmdir /s /q build    # Windows (cmd)
# or
rm -r -fo build/       # Windows (PowerShell)

# Rebuild
mkdir build
cd build
cmake ..
cmake --build .
```

---

## Windows Manual Installation

Since POST_BUILD commands don't work on Windows yet:

### Option 1: Copy to VST3 folder (Recommended)

```cmd
copy build\BULLsEYE_artefacts\VST3\BULLsEYE.vst3 "C:\Program Files\Common Files\VST3\"
```

### Option 2: Use DAW plugin scanner

Most DAWs can scan custom folders:

**REAPER:**
1. Options → Preferences → Plug-ins → VST
2. Add `build/BULLsEYE_artefacts/VST3/` to path list
3. Click "Re-scan"

**Cubase:**
1. Studio → Plug-in Manager
2. Add custom VST3 folder
3. Scan for new plug-ins

---

## Verify Build Succeeded

### macOS:
```bash
# Check if files exist
ls ~/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3
ls ~/Library/Audio/Plug-Ins/Components/BULLsEYE.component
```

### Windows:
```cmd
dir build\BULLsEYE_artefacts
```

Should see:
- `VST3/` folder with `BULLsEYE.vst3/`
- `Debug/` or `Release/` folder with build output

### Linux:
```bash
ls build/BULLsEYE.lv2/
```

Should see:
- `manifest.ttl`
- `BULLsEYE` (binary)
- Other LV2 bundle files

---

## Testing in Your DAW

### Quick Test:

1. **Open your DAW** (REAPER recommended - works on all platforms)
2. **Create audio track**
3. **Insert BULLsEYE plugin**
4. **Play audio** (any audio file)
5. **Check:**
   - ✅ Meter moves up and down
   - ✅ LUFS-I value updates
   - ✅ True Peak value updates
   - ✅ Status shows "Balanced", "Hot", or "Quiet"
   - ✅ Circular meter displays level

### Compare with JSFX:

If you have the original JSFX:
1. Load both plugins on the same track
2. Play same audio
3. Compare LUFS-I measurements
4. They should match within ±0.01 LU

---

## What to Read Next

### If you want to understand the code:

1. **README.md** - Project overview
2. **GUI_COMPONENTS.md** - UI components explanation
3. **STATUS_STATES.md** - How Quiet/Balanced/Hot works
4. **REFERENCE.md** - Plugin constants and identity

### If you want to modify the code:

1. **Source/SSOT/** - Single Source of Truth (all constants)
2. **templates/** - Code templates for components
3. **LEARNINGS.md** - Development decisions and lessons

### If you want to build for production:

1. **docs/WINDOWS_BUILD.md** - Windows build details
2. **docs/LINUX_BUILD.md** - Linux build details
3. **docs/CODE_SIGNING.md** - macOS code signing
4. **NOTARIZATION.md** - macOS notarization

---

## Auto-Build (CI/CD)

The project has automated builds via GitHub Actions:

### Windows:
- Triggers on: Push to main/develop, pull requests, releases
- Builds: VST3 (Release + Debug)
- Status: ✅ Working

### Linux:
- Triggers on: Push to main/develop, pull requests, releases
- Builds: LV2 (x86_64 + ARM64)
- Status: ✅ Working

### macOS:
- **Status:** ❌ Not configured yet
- **Manual build only** (use `./build.sh`)

---

## Project Structure (What's Where)

```
BULLsEYE-JSFX-CPP/
├── CMakeLists.txt              # Build configuration
├── build.sh                    # Quick build script (macOS/Linux)
├── README.md                   # Start here!
├── GUI_COMPONENTS.md           # UI documentation
├── STATUS_STATES.md             # Status states explanation
├── REFERENCE.md                # Plugin constants
│
├── Source/                     # All C++ code
│   ├── SSOT/                  # Single Source of Truth
│   │   ├── ModelSSOT.h       # Parameters, modes
│   │   ├── UISSOT.h          # Colors, fonts, strings
│   │   ├── ProcessorSSOT.h   # Processing constants
│   │   └── DSPSSOT.h         # DSP constants
│   │
│   ├── DSP/                   # Audio processing
│   ├── Components/             # UI components
│   ├── PluginProcessor.cpp/h   # Main processor
│   └── PluginEditor.cpp/h      # Main UI
│
├── build/                      # Build output (git-ignored)
├── modules/JUCE/               # JUCE framework (submodule)
├── tests/                     # Integration tests
└── docs/                      # Additional documentation
```

---

## Key Concepts

### SSOT (Single Source of Truth)
All constants are in `Source/SSOT/` files. No hardcoded values in code.

### TETRIS Principles
DSP core follows thread-safe architecture:
- **T**hread Separation
- **E**ncapsulation
- **T**rivially Copyable
- **R**eference Processing
- **I**nternal Double
- **S**moothing

### JUCE 8 Compatibility
- Use module-specific includes: `#include <juce_audio_processors/...>`
- Use `juce::FontOptions()` instead of deprecated Font constructor
- Use `inline` functions for Color (not constexpr)

---

## Getting Help

### Build Issues:
1. Check **Common Issues & Fixes** above
2. Read platform-specific docs (WINDOWS_BUILD.md, LINUX_BUILD.md)
3. Verify JUCE submodule is initialized
4. Check CMake version (need 3.15+)

### Plugin Issues:
1. Verify build succeeded (check build artifacts)
2. Reinstall plugin to DAW
3. Force DAW to rescan plugins
4. Test in different DAW
5. Check DAW console for error messages

### Code Questions:
1. Read relevant documentation (GUI_COMPONENTS.md, STATUS_STATES.md)
2. Check SSOT files for constants
3. Look at templates for examples
4. Ask in project issues/discussions

---

## Summary

| Platform | Build Command | Difficulty | Auto-Build? |
|----------|---------------|-------------|--------------|
| **macOS** | `./build.sh` | ⭐ Easy | ❌ No |
| **Windows** | Manual CMake | ⭐⭐⭐ Medium | ✅ Yes |
| **Linux** | `./build.sh` | ⭐ Easy | ✅ Yes |

**Most common issue:** JUCE submodule not initialized  
**Fix:** `git submodule update --init --recursive`

**Second most common issue:** Windows POST_BUILD error  
**Fix:** Ignore it, copy manually to plugin folder

**Documentation is good:** You should be able to build and test in 5-10 minutes

---

## You're Ready! 🎉

You've successfully:
- ✅ Cloned the repository
- ✅ Installed prerequisites
- ✅ Built the plugin
- ✅ Installed it in your DAW
- ✅ Tested it with audio

**Next steps:**
- Explore the code in `Source/`
- Read documentation in `docs/`
- Modify parameters in `Source/SSOT/`
- Contribute to the project!

**Happy coding! 🚀**

---

*Quick Start Guide v1.0*  
*Last Updated: 2026-02-07*