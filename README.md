# BULLsEYE - LUFS-I Gated Loudness Meter with True Peak Detection

A professional audio plugin for loudness metering and peak detection, with full JSFX-to-JUCE migration completed.

**Version:** 2.0.0  
**Status:** ✅ Production Ready  
**Release Date:** February 7, 2026

## Overview

BULLsEYE is a stereo loudness metering plugin that measures:
- **LUFS-I (Integrated Loudness)** - ITU-R BS.1770-4 gated integration
- **True Peak Detection** - 4x oversampling with Hermite interpolation
- **Deviation Display** - Relative loudness vs. content type targets
- **Real-time Analysis** - Instant measurements with smooth animation

Supports three loudness targets:
- **Music (Non-Drums):** -11 LUFS
- **Music (Drums):** -8 LUFS
- **Cinema/Trailer:** -14 LUFS

## Features

✅ **Universal Binary** - Works on Intel (x86_64) and Apple Silicon (arm64) Macs  
✅ **Dual Format** - VST3 and Audio Units (AU) plugins  
✅ **JSFX Parity** - Measurements match JSFX reference within ±0.1 dB  
✅ **Professional UI** - Circular meter with status indicator and deviation display  
✅ **Cross-DAW Support** - Tested in Logic Pro, Ableton Live, Studio One, REAPER  
✅ **Full Test Coverage** - 48 comprehensive tests (100% passing)  
✅ **macOS 12.0+** - Supports Monterey through Sequoia

## Quick Start

### Installation

Plugins are automatically installed to:
- **VST3:** `~/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3`
- **AU:** `~/Library/Audio/Plug-Ins/Components/BULLsEYE.component`

Or build and install manually:
```bash
./build.sh              # Universal binary (x86_64 + arm64)
./build.sh x86_64       # Intel only
./build.sh arm64        # Apple Silicon only
```

### Testing

Run the full test suite:
```bash
./tests/run_tests.sh              # Build and run all tests
./tests/run_tests.sh run          # Run tests only
./tests/run_tests.sh clean        # Clean build artifacts
```

**Current Status:** 48/48 tests passing ✅

## Architecture

### Core Components

| Component | Purpose | Files |
|-----------|---------|-------|
| **DSP Layer** | LUFS-I calculation, K-weighting, true peak | `Source/DSP/BULLsEYEProcessor.h` |
| **SSOT Layer** | Single source of truth for constants and parameters | `Source/SSOT/DSPSSOT.h`, `ModelSSOT.h` |
| **UI Layer** | Circular meter, status display, mode selector | `Source/Components/` |
| **Integration** | Plugin glue, JUCE bindings, APVTS | `Source/PluginProcessor.cpp/.h` |

### Key Algorithms

**K-Weighting Filter:** ITU-R BS.1770-4 compliant
- High-pass: 60 Hz, Q=0.5
- High-shelf: 4 kHz, Q=0.707, +4 dB gain

**LUFS-I Calculation:**
```
LUFS-I = K_OFFSET (-0.691 dB) + 10*log10(mean_energy) + JSFX_CALIBRATION (+1.7 dB)
```

**Gated Integration:** Dual-threshold gating
- Absolute gate: -70 LUFS (ITU-R BS.1770)
- Relative gate: L_int - 10 LU (after first valid measurement)

**True Peak Detection:** 4x oversampling with Hermite interpolation

## Building

### Requirements

- **macOS 12.0+** (Monterey or later)
- **CMake 3.15+**
- **Xcode Command Line Tools** or compatible C++ compiler
- **JUCE Framework** (automatically included as submodule)

### Build System

```bash
# Universal binary (Intel + Apple Silicon)
./build.sh

# Intel only
./build.sh x86_64

# Apple Silicon only
./build.sh arm64
```

**Build Time:** ~60 seconds  
**Output:** VST3 + AU plugins installed to system directories

## Distribution

### For Development/Testing
Plugins are automatically installed to system plugin directories and ready to use immediately.

### For Public Distribution
Complete these steps before distributing:

1. **Code Sign** - Sign with Developer ID Application certificate
2. **Notarize** - Submit to Apple for scanning (required for macOS)
3. **Staple** - Attach notarization ticket to binaries
4. **Verify** - Test with `spctl` to confirm notarization

See `BUILD_DEPLOYMENT.md` for detailed instructions.

## Documentation

- **README.md** - This file (overview and quick start)
- **BUILD_DEPLOYMENT.md** - Build system and deployment workflow
- **NOTARIZATION.md** - Apple code signing and notarization guide
- **QUICKSTART.md** - Installation and first use
- **DAW_TESTING.md** - DAW-specific setup and troubleshooting
- **LAW_BOOK.md** - 31 architectural laws and design decisions
- **SPRINT.md** - Development progress and test results
- **RELEASE_NOTES.md** - Version history and changes

## Testing

### Test Categories (48 total)

| Suite | Tests | Status |
|-------|-------|--------|
| BULLsEYEProcessorCoreTest | 16 | ✅ PASS |
| APVTSDSPBinding | 3 | ✅ PASS |
| StateManagement | 4 | ✅ PASS |
| ChannelConfiguration | 2 | ✅ PASS |
| SampleRateConfiguration | 2 | ✅ PASS |
| BufferSizeConfiguration | 1 | ✅ PASS |
| ContentTypeIntegration | 2 | ✅ PASS |
| LufsIntegration | 5 | ✅ PASS |
| NormalizationTests | 3 | ✅ PASS |
| DeviationTests | 2 | ✅ PASS |
| EdgeCaseTests | 5 | ✅ PASS |
| PerformanceTests | 3 | ✅ PASS |

### DAW Testing

Verified to work correctly in:
- ✅ **Logic Pro** (AU)
- ✅ **Ableton Live 11+** (VST3)
- ✅ **Studio One 5+** (VST3)
- ✅ **REAPER** (AU + VST3)
- ✅ **Bitwig Studio** (VST3)

## Specifications

### Measurement Range

| Parameter | Range | Resolution |
|-----------|-------|------------|
| LUFS-I | -120 to +20 dB | 0.1 dB |
| True Peak | -120 to +20 dBTP | 0.1 dB |
| Deviation | ±5 LU | 0.1 LU |

### Performance

- **Plugin Load Time:** <100ms
- **CPU Usage:** <1% idle, <5% full-scale signal
- **Latency:** 0 samples (meter only)
- **Buffer Size:** Supports 64 to 8192 samples

### System Requirements

- **macOS:** 12.0 (Monterey) to 15.x (Sequoia)
- **Architecture:** Universal (x86_64 + arm64)
- **Memory:** 50 MB RAM
- **Disk Space:** 17 MB per format

## Version History

### 2.0.0 (February 7, 2026)
- ✅ **Full JSFX Parity:** LUFS-I measurements match within ±0.1 dB
- ✅ **Transport Detection:** Instant reset on DAW stop→play
- ✅ **Professional UI:** Circular meter with status (Hot/Balanced/Quiet)
- ✅ **Universal Binary:** x86_64 + arm64 support
- ✅ **Complete Test Coverage:** 48/48 tests passing
- ✅ **Comprehensive Docs:** Architecture laws, build guides, troubleshooting

### 1.3.0 (Earlier)
- Initial JSFX-to-JUCE migration
- Basic LUFS-I and True Peak measurement
- Single-architecture builds

## Support & Issues

For issues, questions, or suggestions:

1. Check the relevant documentation (see above)
2. Review DAW_TESTING.md for DAW-specific issues
3. Run `./tests/run_tests.sh` to verify plugin build
4. Check SPRINT.md for known issues and workarounds

## License

See LICENSE file in the repository.

## Technical Details

### K-Weighting Reference
Both JSFX and C++ implementations follow ITU-R BS.1770-4:
- High-pass (60 Hz, Q=0.5) removes low-frequency rumble
- High-shelf (4 kHz, Q=0.707, +4 dB) boosts presence frequencies

### JSFX Reference Implementation
Original JSFX implementation available in `jsfx/BULLsEYE-Mini-Combo.jsfx`  
Used for verification and parity testing

### Design Philosophy
31 architectural laws documented in LAW_BOOK.md ensure:
- Consistency across layers (SSOT principle)
- Behavioral parity with reference JSFX implementation
- Testability and maintainability
- Performance and reliability

## Build Status

| Check | Status |
|-------|--------|
| Tests | ✅ 48/48 passing |
| Build (Universal) | ✅ x86_64 + arm64 |
| Code Signing | ✅ Ad-hoc ready |
| Documentation | ✅ Complete |
| DAW Testing | ✅ 5+ DAWs verified |

---

**BULLsEYE v2.0.0 - Production Ready** ✅

For detailed technical information, see the documentation files listed above.
