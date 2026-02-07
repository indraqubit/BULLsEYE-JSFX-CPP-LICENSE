# BULLsEYE Build & Deployment Guide

Quick reference for building and deploying BULLsEYE audio plugins.

## Quick Start

### Build (Development)
```bash
./build.sh              # Universal binary (x86_64 + arm64)
./build.sh x86_64       # Intel only
./build.sh arm64        # Apple Silicon only
```

### Run Tests
```bash
./tests/run_tests.sh              # Build and run all tests
./tests/run_tests.sh build        # Build tests only
./tests/run_tests.sh run          # Run tests only
./tests/run_tests.sh clean        # Clean test artifacts
```

## Build System

- **Build Tool:** CMake 3.15+
- **Formats:** VST3, Audio Units (AU)
- **C++ Standard:** C++17 with JUCE framework
- **Platforms:** macOS 12.0+

## Build Artifacts

### Locations

**Development Install:**
- VST3: `~/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3`
- AU: `~/Library/Audio/Plug-Ins/Components/BULLsEYE.component`

**Build Directory:**
- VST3: `./build/BULLsEYE_artefacts/Release/VST3/BULLsEYE.vst3`
- AU: `./build/BULLsEYE_artefacts/Release/AU/BULLsEYE.component`

### Verification

Check plugin status:
```bash
# File info
file ~/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3/Contents/MacOS/BULLsEYE

# Size
du -sh ~/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3

# Code signature
codesign -v ~/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3
```

## Distribution Workflow

### Step 1: Build Release
```bash
./build.sh universal
```

### Step 2: Code Sign (if distributing)
```bash
# Get your Developer ID
security find-identity -p codesigning -v

# Sign VST3
codesign --force --sign "Developer ID Application: Your Name (XXXXX)" \
  --deep --timestamp --options=runtime \
  ~/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3

# Sign AU
codesign --force --sign "Developer ID Application: Your Name (XXXXX)" \
  --deep --timestamp --options=runtime \
  ~/Library/Audio/Plug-Ins/Components/BULLsEYE.component

# Verify
codesign -v ~/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3
codesign -v ~/Library/Audio/Plug-Ins/Components/BULLsEYE.component
```

### Step 3: Apple Notarization
See NOTARIZATION.md for detailed instructions:
```bash
# Submit for notarization
xcrun altool --notarize-app -f ~/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3 \
  -t osx -u "your-apple-id@example.com" -p "app-specific-password"

# Wait for approval (usually 5-15 minutes)
# Check status with RequestID from above response

# Staple the ticket
xcrun altool --staple-notarization -f ~/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3
```

### Step 4: Verify Notarization
```bash
# Check notarization status
spctl -a -v ~/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3

# Expected output: "accepted" or "valid on disk"
```

### Step 5: Package for Distribution
```bash
# Create distribution package
mkdir -p BULLsEYE-v2.0.0
cp -r ~/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3 BULLsEYE-v2.0.0/
cp -r ~/Library/Audio/Plug-Ins/Components/BULLsEYE.component BULLsEYE-v2.0.0/
cp README.md RELEASE_NOTES.md BULLsEYE-v2.0.0/

# Create archive
zip -r BULLsEYE-v2.0.0.zip BULLsEYE-v2.0.0/
```

## Testing

### Unit Tests
```bash
# Run all 48 tests (should all pass)
./tests/run_tests.sh run

# Expected output: [  PASSED  ] 48 tests.
```

### DAW Testing
Test the plugins in your favorite DAWs:
- **Logic Pro** - Standard AU host
- **Ableton Live** - VST3 support (11.1+)
- **Studio One** - Excellent VST3 support
- **Reaper** - Both AU and VST3
- **Bitwig** - VST3 support

### Cross-Architecture Testing
- Test on **Intel Mac** (x86_64)
- Test on **Apple Silicon Mac** (arm64/M1+)
- Verify both architectures work correctly

## Troubleshooting

### Plugin Not Loading
1. Check if code-signed correctly: `codesign -v path/to/plugin`
2. Check macOS version: Requires 12.0+
3. Check DAW compatibility: VST3 requires 3.6+ hosts
4. Check plugin directory permissions: Should be readable

### Build Failures
1. Ensure JUCE is properly set up: `ls -la modules/JUCE`
2. Check CMake version: `cmake --version` (need 3.15+)
3. Clean and rebuild: `rm -rf build && ./build.sh`
4. Check compiler: `clang++ --version`

### Notarization Issues
1. Verify code signing: `codesign -v --deep plugin.vst3`
2. Check for hardened runtime: Notarization requires `--options=runtime`
3. Wait for Apple response: Can take 5-30 minutes
4. Check status: Use RequestID from notarization submission

## Performance

- **Build Time:** ~60 seconds (universal binary)
- **Test Suite:** ~100ms (48 tests)
- **Plugin Load:** <100ms typical
- **CPU Usage:** <1% idle, <5% at full scale

## Version Info

- **Current Version:** 2.0.0
- **JUCE Framework:** Latest compatible
- **Minimum macOS:** 12.0 (Monterey)
- **Latest Tested:** macOS 15.x (Sequoia)

## Support

For issues or questions:
1. Check NOTARIZATION.md for distribution questions
2. See QUICKSTART.md for setup help
3. Review DAW_TESTING.md for DAW-specific issues
4. Check README.md for general information
