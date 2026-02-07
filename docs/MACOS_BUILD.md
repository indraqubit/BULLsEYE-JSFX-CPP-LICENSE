# macOS Build Guide for BULLsEYE

**Last Updated:** 2026-02-07  
**Platform:** macOS 12.0 (Monterey) through 15.0 (Sequoia/Tahoe)  
**Architectures:** Universal Binary (x86_64 + ARM64), x86_64-only, ARM64-only

---

## Overview

BULLsEYE supports Universal Binary builds for macOS, running natively on both Intel (x86_64) and Apple Silicon (ARM64) Macs. This guide covers:

- Building for all architectures
- Verifying build artifacts
- Troubleshooting common issues
- Code signing for distribution
- Notarization workflow

---

## Prerequisites

### Required Tools

| Tool | Installation | Purpose |
|------|--------------|---------|
| Xcode Command Line Tools | `xcode-select --install` | Compiler and build tools |
| CMake 3.15+ | Included with Xcode or cmake.org | Build configuration |
| Git | Included with Xcode | Version control, JUCE submodule |

### Verify Installation

```bash
# Check Xcode Command Line Tools
xcode-select --version
# Expected: xcode-select version 2395 or later

# Check CMake
cmake --version
# Expected: cmake version 3.15 or later

# Check Git
git --version
# Expected: git version 2.0 or later
```

---

## Build Instructions

### Quick Start (Universal Binary - Recommended)

```bash
# Clone repository
git clone https://github.com/indraqubit/portable-jsfx-to-juce-migration
cd portable-jsfx-to-juce-migration

# Initialize JUCE submodule (IMPORTANT!)
git submodule update --init --recursive

# Build universal binary (Intel + Apple Silicon)
./build.sh universal

# Verify installation
lipo -info ~/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3/Contents/x86_64/BULLsEYE
# Expected output: Architectures in the fat file: BULLsEYE are: x86_64 arm64
```

### Architecture-Specific Builds

#### Universal Binary (Default)

Builds for both Intel and Apple Silicon in a single binary:

```bash
./build.sh universal
# Output: "Building universal binary (Intel + Apple Silicon)"
```

**Use when:**
- Distributing to users with unknown hardware
- Running on both Intel and Apple Silicon Macs
- Maximum compatibility required

#### Intel-Only Build

Builds for Intel Macs (x86_64) only:

```bash
./build.sh x86_64
# Output: "Building for Intel (x86_64) only"
```

**Use when:**
- Targeting specific Intel-only deployment
- Reducing binary size
- Debugging architecture-specific issues

#### Apple Silicon-Only Build

Builds for Apple Silicon Macs (ARM64) only:

```bash
./build.sh arm64
# Output: "Building for Apple Silicon (ARM64) only"
```

**Use when:**
- Targeting M1/M2/M3 Macs only
- Optimizing for Apple Silicon performance
- Reducing binary size

### Manual Build (Advanced)

```bash
# Create build directory
mkdir build
cd build

# Configure CMake with specific architecture
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64" \
    -DCMAKE_OSX_DEPLOYMENT_TARGET="12.0"

# Build
cmake --build . --config Release -j$(sysctl -n hw.ncpu)

# Install to standard locations
cmake --build . --target install
```

---

## Verifying Build Artifacts

### Check Binary Architecture

```bash
# Check VST3 binary
lipo -info ~/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3/Contents/x86_64/BULLsEYE

# Expected outputs:
# Universal:    Architectures in the fat file: BULLsEYE are: x86_64 arm64
# x86_64 only:  Architectures in the fat file: BULLsEYE are: x86_64
# arm64 only:   Architectures in the fat file: BULLsEYE are: arm64
```

### Check File Type

```bash
# Check binary type
file ~/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3/Contents/x86_64/BULLsEYE

# Expected outputs:
# Universal:    Mach-O 64-bit universal binary with 2 architectures
# x86_64 only:  Mach-O 64-bit executable x86_64
# arm64 only:   Mach-O 64-bit executable arm64
```

### Verify Bundle Structure

```bash
# VST3 bundle
ls -la ~/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3/
# Expected:
# Contents/
#   Info.plist
#   PkgInfo
#   x86_64-linux/          (VST3 on macOS uses x86_64-linux directory structure)
#   x86_64/                (Intel binary or universal binary directory)
#   arm64/                 (ARM64 binary directory for universal builds)

# AU bundle
ls -la ~/Library/Audio/Plug-Ins/Components/BULLsEYE.component/
# Expected:
# Contents/
#   Info.plist
#   MacOS/
#     BULLsEYE            (Universal binary)
```

### Check Info.plist

```bash
# Verify minimum system version
plutil -p ~/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3/Contents/Info.plist | grep LSMinimumSystemVersion
# Expected: LSMinimumSystemVersion => 12.0

# Check high-resolution capable
plutil -p ~/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3/Contents/Info.plist | grep NSHighResolutionCapable
# Expected: NSHighResolutionCapable => 1
```

---

## Testing in DAWs

### REAPER (VST3)

1. Open REAPER
2. Create new audio track
3. Right-click track → Insert virtual instrument on new track
4. Navigate to VST3: BULLsEYE → Add
5. Verify plugin UI opens
6. Play audio track
7. Verify meter responds

**Verify universal binary:**
```bash
# In REAPER, hover over BULLsEYE in plugin browser
# Should show "Universal" or both architectures
```

### Logic Pro (AU)

1. Open Logic Pro
2. Create new audio track
3. Click "Audio FX" button → Audio Units → BULLsEYE
4. Verify plugin UI opens
5. Play audio track
6. Verify meter responds

### Ableton Live (AU)

1. Open Ableton Live
2. Create audio track
3. Click "Audio FX" button → Audio Units → BULLsEYE
4. Verify plugin UI opens
5. Play audio track
6. Verify meter responds

### Troubleshooting DAW Issues

| Issue | Solution |
|-------|----------|
| Plugin not showing | Rescan plugins in DAW preferences |
| Plugin crashes on load | Check DAW console for error messages |
| UI not rendering | Verify NSHighResolutionCapable is true |
| Audio artifacts | Test with different audio files |
| Performance issues | Monitor CPU usage in Activity Monitor |

---

## macOS Version Compatibility

### Tested Configurations

| macOS Version | Status | Notes |
|---------------|--------|-------|
| 12.0 (Monterey) | ✅ Supported | Minimum deployment target |
| 13.0 (Ventura) | ✅ Supported | Common version |
| 14.0 (Sonoma) | ✅ Supported | Current standard |
| 15.0 (Sequoia/Tahoe) | ✅ Supported | Latest version |

### Version-Specific Notes

#### macOS 12.0 (Monterey) and later
- Minimum supported version
- All features available
- Universal binary fully supported

#### macOS 14.0 (Sonoma) and later
- Improved animation performance
- Better Retina display support
- Enhanced security features

#### macOS 15.0 (Sequoia/Tahoe)
- Latest macOS version
- All features supported
- Universal binary fully supported

---

## Troubleshooting Common Issues

### Issue: "JUCE not found"

**Error:**
```
ERROR: JUCE not found!
```

**Solution:**
```bash
git submodule update --init --recursive
```

### Issue: "Permission denied"

**Error:**
```
Permission denied: './build.sh'
```

**Solution:**
```bash
chmod +x build.sh
./build.sh
```

### Issue: Architecture verification fails

**Error:**
```
lipo: can't open input file: ... (No such file or directory)
```

**Solution:**
1. Verify build completed successfully
2. Check plugin is installed:
   ```bash
   ls ~/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3/Contents/x86_64/
   ```
3. Rebuild if necessary:
   ```bash
   rm -rf build
   ./build.sh universal
   ```

### Issue: CMAKE_OSX_ARCHITECTURES not recognized

**Error:**
```
CMake Warning: Manually-specified variable...
```

**Solution:**
Ensure you're using CMake 3.15 or later:
```bash
cmake --version
```

If using older CMake, upgrade or manually specify:
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
```

### Issue: Plugin doesn't load in DAW

**Symptoms:**
- Plugin not visible in plugin browser
- DAW crashes when loading plugin
- Error messages in console

**Solution:**
1. Check system requirements (macOS 12.0+)
2. Verify code signing (if required):
   ```bash
   codesign -dv ~/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3
   ```
3. Check Gatekeeper:
   ```bash
   spctl -a -t exec -vv ~/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3
   ```
4. Clear DAW plugin cache

### Issue: Bundle structure invalid

**Error:**
```
Invalid bundle structure
```

**Solution:**
1. Verify Info.plist exists:
   ```bash
   ls ~/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3/Contents/Info.plist
   ```
2. Check bundle identifier:
   ```bash
   plutil -p ~/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3/Contents/Info.plist | grep CFBundleIdentifier
   ```
3. Rebuild plugin:
   ```bash
   rm -rf build
   ./build.sh universal
   ```

---

## Code Signing for Distribution

### Prerequisites

1. **Apple Developer Account** - Required for code signing
2. **Developer ID Certificate** - For distribution outside App Store
3. **Xcode** - For code signing tools

### Obtain Developer ID Certificate

1. Go to [Apple Developer Portal](https://developer.apple.com/account)
2. Navigate to Certificates, Identifiers & Profiles
3. Create Developer ID certificate (Developer ID Application)
4. Download and install in Keychain Access

### Sign the Plugin

```bash
# Sign AU component
codesign --force --sign "Developer ID Application: Your Name" \
    --deep --timestamp --options=runtime \
    ~/Library/Audio/Plug-Ins/Components/BULLsEYE.component

# Sign VST3 component
codesign --force --sign "Developer ID Application: Your Name" \
    --deep --timestamp --options=runtime \
    ~/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3
```

### Verify Signature

```bash
# Check signature
codesign -dv ~/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3

# Expected output:
# Authority=Developer ID Application: Your Name
# Timestamp=...
```

### Gatekeeper Verification

```bash
# Test Gatekeeper acceptance
spctl -a -t exec -vv ~/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3

# Expected output: accepted
```

---

## Notarization Workflow

### Prerequisites

1. **App Store Connect API Key** - For submitting to Apple
2. **xcrun altool** - For uploading to Apple
3. **Developer ID Certificate** - Already obtained

### Create API Key

1. Go to [App Store Connect](https://appstoreconnect.apple.com)
2. Navigate to Users and Access → Keys
3. Create new API Key with "Developer ID" role
4. Download and save the API Key file (.p8)

### Submit for Notarization

```bash
# Variables
API_KEY_ID="YOUR_API_KEY_ID"
API_ISSUER_ID="YOUR_ISSUER_ID"
API_KEY_FILE="AuthKey_YOUR_API_KEY_ID.p8"

# Upload for notarization
xcrun altool --notarize-app \
    --primary-bundle-id com.iq.bullseye \
    --username "your-email@example.com" \
    --password "@keychain:AC_PASSWORD" \
    --apiKey "$API_KEY_ID" \
    --apiIssuer "$API_ISSUER_ID" \
    --file ~/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3.zip

# Wait for notarization to complete
# Check status:
xcrun altool --notarization-info <RequestUUID> \
    --username "your-email@example.com" \
    --password "@keychain:AC_PASSWORD"
```

### Staple Notarization Ticket

```bash
# Staple ticket to plugin
xcrun stapler staple ~/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3

# Verify staple
xcrun stapler validate ~/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3
# Expected: The validate action worked!
```

---

## Distribution

### Create Distribution Package

```bash
# Create ZIP file
cd ~/Library/Audio/Plug-Ins/
zip -r BULLsEYE.vst3.zip VST3/BULLsEYE.vst3/
zip -r BULLsEYE.component.zip Components/BULLsEYE.component/
```

### Distribution Options

| Method | Requirements | Notes |
|--------|--------------|-------|
| Direct Download | None | Users must bypass Gatekeeper |
| Notarized ZIP | Developer ID + Notarization | Users can open directly |
| DMG with Notarization | Developer ID + Notarization | Professional distribution |

### Recommended Distribution

1. **Notarize** the plugins (see above)
2. **Create DMG** with installation instructions
3. **Test** distribution package on clean macOS installation
4. **Upload** to website or distribution platform

---

## Performance Considerations

### Architecture-Specific Performance

| Architecture | CPU Usage | Notes |
|--------------|-----------|-------|
| x86_64 | Baseline | Native Intel performance |
| ARM64 | ~10-15% better | Native Apple Silicon performance |
| Universal | Similar to x86_64 | OS selects appropriate architecture |

### Optimization Tips

1. **Use ARM64-only build** for Apple Silicon Macs for best performance
2. **Monitor CPU usage** in Activity Monitor during playback
3. **Test with high sample rates** (96kHz, 192kHz)
4. **Profile** using Instruments if performance issues occur

---

## FAQ

### Q: Do I need to build separate installers for Intel and Apple Silicon?

**A:** No. Use universal binary builds for maximum compatibility. Single installer works on both architectures.

### Q: Can I build on Intel Mac for Apple Silicon?

**A:** Yes, but you need Apple Silicon Mac or cross-compilation tools. For simplicity, build on target architecture.

### Q: Why is the universal binary larger?

**A:** Universal binaries contain code for both architectures, roughly 2x the size of single-architecture builds.

### Q: How do I reduce binary size?

**A:** Build architecture-specific versions (x86_64 or arm64) for targeted distribution.

### Q: Is Rosetta 2 required?

**A:** No. Universal binary runs natively on both architectures without Rosetta 2.

### Q: Can I distribute unsigned plugins?

**A:** Yes, but users will see Gatekeeper warnings. Notarization is recommended for distribution.

---

## Support

### Getting Help

1. **Check Troubleshooting section** above
2. **Review DAW-specific documentation** in docs/DAW_TESTING.md
3. **Search existing issues** on GitHub
4. **Create new issue** with:
   - macOS version
   - Architecture
   - DAW being used
   - Error messages
   - Steps to reproduce

### Reporting Bugs

When reporting build issues, include:
```bash
# System information
uname -a
cmake --version
xcode-select --version

# Build output (last 50 lines)
./build.sh universal 2>&1 | tail -50
```

---

## Related Documentation

| Document | Purpose |
|----------|---------|
| [README.md](../README.md) | Project overview |
| [QUICKSTART.md](../QUICKSTART.md) | 5-minute setup guide |
| [docs/DAW_TESTING.md](./DAW_TESTING.md) | DAW-specific testing |
| [docs/CODE_SIGNING.md](./CODE_SIGNING.md) | Code signing guide |
| [docs/NOTARIZATION.md](./NOTARIZATION.md) | Notarization guide |

---

*macOS Build Guide v1.0*  
*Last Updated: 2026-02-07*
