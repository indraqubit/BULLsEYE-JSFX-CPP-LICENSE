# Portable JSFX to JUCE Migration Template

**Version:** 2.0.0  
**Date:** 2025-12-20  
**JUCE Version:** 8  
**C++ Standard:** C++17

---

## Overview

This is a **portable, drop-in template** for migrating JSFX plugins to JUCE 8/C++17. It includes all best practices, frameworks, and patterns developed during IntelliMute and BULLsEYE migration projects.

## Features

- **SSOT Architecture**: Single Source of Truth for all constants
- **TETRIS Compliance**: Thread-safe DSP implementation
- **Virtue DNA Framework**: Development philosophy and principles
- **Professional UI**: Modern dark theme with branding
- **CMake Build System**: Modern build configuration
- **Complete Documentation**: Templates for all phases
- **Comprehensive Testing**: Integration tests and DAW testing procedures

## ⚠️ IMPORTANT: Read First

Before starting your migration, **read these documents in order:**

1. **PORTING_GUIDE.md** (30 min) — Framework migration strategy + 15 laws that prevent bugs
2. **MIGRATION_LAWS.md** (30 min) — Detailed laws with incident examples and templates
3. **MIGRATION_PLAN.md** (10 min) — Roadmap template for your specific project
4. **REFERENCE.md** (10 min) — Algorithm documentation (if doing DSP port)

These aren't optional reading. They will **save 10-20 hours of debugging** on your first port.

---

## Quick Start

### 1. Copy Template

```bash
cp -r portable-jsfx-to-juce-migration /path/to/your-new-plugin/
cd /path/to/your-new-plugin/
```

### 2. Customize

Edit these files with your plugin information:

- `CMakeLists.txt` - Plugin name, company, formats
- `REFERENCE.md` - Plugin identity, versions, paths
- `Source/SSOT/ModelSSOT.h` - Parameter IDs, names, modes
- `Source/SSOT/UISSOT.h` - Colors, dimensions, strings
- `Source/SSOT/DSPSSOT.h` - DSP constants
- `Source/SSOT/ProcessorSSOT.h` - Processing constants

### 3. Build

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

---

## Completed Projects

This template has been successfully used to migrate the following plugins:

### BULLsEYE - LUFS-I Gated Loudness Meter

**Status:** Complete  
**Version:** 2.0.0  
**Migration Date:** 2026-02-06  
**Release Date:** 2026-02-07  
**Plugin Code:** BULL  
**Company:** IQ  

**Key Features:**
- ITU-R BS.1770 compliant K-weighting filters
- Gated integration with absolute (-70 LUFS) and relative (L-10) thresholds
- True Peak detection with 4x oversampling and Hermite interpolation
- Three content type modes (Music Non-drums, Music Drums, Cinema/Trailer)
- Real-time LED strip meter with 20 segments
- Comprehensive integration test suite (42 tests)

**Build Artifacts:**
- AU: 8.7 MB
- VST3: 9.0 MB

**Documentation:**
- `REFERENCE.md` - Complete parameter documentation
- `LEARNINGS.md` - Development notes and decisions
- `DAW_TESTING.md` - Comprehensive DAW testing procedures

---

## Project Structure

```
portable-jsfx-to-juce-migration/
├── CMakeLists.txt              # Build configuration
├── REFERENCE.md                # Plugin identity & constants (SSOT)
├── LEARNINGS.md                # Development notes
├── MIGRATION_PLAN.md           # Migration roadmap template
├── DAW_TESTING.md              # DAW testing procedures
├── LUFS_BOOTSTRAP.md           # LUFS meter implementation guide
│
├── 📁 jsfx/                    # ← YOUR ORIGINAL JSFX SOURCE FILES
│   └── *.jsfx                  # Original JSFX plugins (reference only)
│
├── 📁 Source/                  # ← GENERATED JUCE C++ CODE
│   ├── SSOT/                   # Single Source of Truth files
│   │   ├── ModelSSOT.h         # Model layer constants
│   │   ├── UISSOT.h            # UI layer constants
│   │   ├── ProcessorSSOT.h     # Processor layer constants
│   │   └── DSPSSOT.h           # DSP layer constants
│   │
│   ├── DSP/                    # DSP implementation
│   │   └── YourProcessor.h     # TETRIS-compliant processor
│   │
│   ├── Components/             # UI components
│   │   ├── StatusDisplayComponent.h/cpp
│   │   ├── ModeSelectorComponent.h/cpp
│   │   ├── MuteButtonComponent.h/cpp
│   │   ├── InvertToggleComponent.h/cpp
│   │   ├── ModeDescriptionComponent.h/cpp
│   │   └── LEDStripMeter.h/cpp
│   │
│   ├── PluginProcessor.h/cpp   # Main audio processor
│   └── PluginEditor.h/cpp      # Main UI editor
│
├── 📁 build/                   # ← BUILD OUTPUT (git-ignored)
│   ├── *.vst3/                # VST3 plugin (Windows/Linux)
│   ├── *.component/           # AU plugin (macOS)
│   └── *.so/                  # LV2 plugin (Linux)
│
├── 📁 modules/
│   └── JUCE/                   # JUCE as submodule/symlink
│
├── 📁 templates/               # Code templates
│   ├── COMPONENT_TEMPLATE.md
│   ├── DSP_TEMPLATE.md
│   ├── SSOT_TEMPLATE.md
│   └── REVIEW_TEMPLATE.md
│
├── 📁 tests/                   # Integration tests
│   ├── Integration/
│   │   └── TestBULLsEYEIntegration.cpp
│   ├── run_tests.sh
│   └── CMakeLists.txt
│
└── 📁 docs/                    # Additional documentation
    └── (project-specific docs)
```

---

## Source Locations

| Content | Location | Purpose |
|---------|----------|---------|
| **JSFX Source** | `jsfx/*.jsfx` | Your original JSFX reference files |
| **JUCE C++ Source** | `Source/` | Migrated C++ code |
| **SSOT Constants** | `Source/SSOT/` | All centralized constants |
| **DSP Implementation** | `Source/DSP/` | Audio processing logic |
| **UI Components** | `Source/Components/` | User interface elements |
| **Build Output** | `build/` | Compiled plugins (VST3/AU/LV2) |
| **Integration Tests** | `tests/` | Test suite |

---

## The 15 Laws (Critical for Success)

JSFX→JUCE ports have systematic traps. These 15 laws prevent 80% of them:

| # | Law | Summary |
|---|-----|---------|
| 1 | Algorithm ≠ Output | Identical math ≠ identical results across platforms |
| 2 | Implicit → Explicit | JSFX framework provides things; JUCE requires you to build them |
| 3 | Sentinel ≠ Value | Special values can't share domain with real measurements |
| 4 | Heuristics fail | No silence detection, sample counting, or pattern matching |
| 5 | Transport ≠ Audio | "Stopped" doesn't mean processBlock() stops |
| 6 | Warmup is algorithm | Early block behavior affects final integrated result |
| 7 | Lifecycle = drift | Different reset timing → different final value |
| 8 | Topology matters | Block size, order, timing all affect result |
| 9 | Unknowable ≠ unsolvable | Some diffs are economically irrational to chase |
| 10 | Calibration valid | Empirical offsets are legitimate engineering |
| 11 | Synthetic tests lie | Real audio reveals bugs synthetic tests hide |
| 12 | Read source, not cache | UI must read APVTS, not DSP object |
| 13 | Glitches = timing | Stale UI values are thread-safety bugs |
| 14 | Host is active | DAW behavior is part of your system |
| 15 | Code ≠ intent | Translate intent, not just syntax |

**See MIGRATION_LAWS.md for full details + incident examples + defensive templates.**

---

## Migration Workflow

```
1. Read PORTING_GUIDE.md + MIGRATION_LAWS.md ← Do this first!
        ↓
2. Create CONTRACT.md (framework differences documented)
        ↓
3. Create SENTINEL_AUDIT.md (special values documented)
        ↓
4. 📁 jsfx/           ← Place your original .jsfx files here
        ↓
5. 📁 Source/         ← Migrate to C++ using SSOT patterns
        ↓
6. Test: synthetic tests → real audio A/B → cross-DAW testing
        ↓
7. 📁 build/          ← Compile to VST3/AU/LV2 plugins
        ↓
8. DAW                ← Test your migrated plugin
```

---

## Build Outputs

| Format | Location | Platform |
|--------|----------|----------|
| **VST3** | `build/*.vst3/` | Windows, Linux |
| **AU** | `build/*.component/` | macOS |
| **LV2** | `build/*.so/` | Linux |

---

## SSOT Files

### ModelSSOT.h

Contains all model-related constants:
- Parameter IDs and names
- Mode enums
- Helper functions

```cpp
namespace ModelSSOT
{
    namespace ParameterIDs
    {
        constexpr const char* PARAM1 = "param1";
        constexpr const char* PARAM2 = "param2";
    }
    
    enum class YourMode
    {
        Mode1 = 0,
        Mode2 = 1,
        Mode3 = 2
    };
    
    constexpr int MODE_COUNT = 3;
    
    namespace Helpers
    {
        // Your helper functions here
    }
}
```

### UISSOT.h

Contains all UI-related constants:
- Colors
- Dimensions
- Strings
- Helper functions

### ProcessorSSOT.h

Contains all processor-related constants:
- Buffer sizes
- Sample rates
- Processing thresholds

### DSPSSOT.h

Contains all DSP-related constants:
- Algorithm constants
- Filter parameters
- Processing thresholds

---

## Testing Strategy (From PORTING_GUIDE.md)

Porting requires **three levels of testing**:

### Level 1: Synthetic Tests (20% of time)
- Sine waves, pink noise, impulses
- Fast to run, catches obvious bugs
- **Limitation:** Doesn't catch platform differences

### Level 2: Real Audio A/B Testing (40% of time)
- 10+ diverse music files (pop, electronic, classical, etc.)
- Compare against reference (JSFX) sample-by-sample
- **Pass:** Within ±0.1 dB on 95%+ of files
- **Essential:** This catches bugs synthetic tests hide

### Level 3: Cross-DAW Testing (30% of time)
- REAPER, Ableton Live, Logic Pro, Cubase (minimum)
- Verify behavior is consistent across hosts
- Test parameter changes, transport, edge cases
- **Essential:** Hosts are active agents, not passive environments

**See PORTING_GUIDE.md "Testing Strategy" section for detailed procedures.**

---

## TETRIS Principles

The DSP core follows these principles:

1. **T - Thread Separation**: No UI/audio thread mixing
2. **E - Encapsulation**: Private state, validated setters
3. **T - Trivially Copyable**: `static_assert(std::is_trivially_copyable_v<...>)`
4. **R - Reference Processing**: `template<typename T> void process(T& sample)`
5. **I - Internal Double**: Process in double precision
6. **S - Smoothing**: For parameter transitions

---

## Framework Compliance

### SSOT (Single Source of Truth)

All constants must be defined in SSOT files:
- ❌ No hardcoded values in implementation
- ✅ All values from SSOT files
- ✅ Helper functions used

### DRY (Don't Repeat Yourself)

No code duplication:
- ❌ No copy-paste code
- ✅ Reusable helper functions
- ✅ Template-based implementation

### YAGNI (You Aren't Gonna Need It)

Only implement what's needed:
- ❌ No over-engineering
- ❌ No premature optimization
- ✅ Simple, focused implementation

---

## Migration Checklist

### Phase 1: Foundation

- [x] Set up project structure
- [x] Create SSOT files
- [x] Configure CMake
- [x] Set up JUCE submodule
- [x] Create basic processor skeleton

**Milestone:** Project builds successfully

---

### Phase 2: DSP Core

- [x] Implement TETRIS-compliant processor
- [x] Migrate mode logic
- [x] Implement parameter handling
- [x] Implement audio processing
- [x] Test DSP accuracy

**Milestone:** Audio output matches JSFX

---

### Phase 3: UI Implementation

- [x] Create PluginEditor base
- [x] Create UI components
- [x] Connect components to APVTS
- [x] Implement visual feedback
- [x] Polish UI design

**Milestone:** UI matches or improves on JSFX

---

### Phase 4: Integration & Polish

- [x] Test all functionality
- [x] Handle edge cases
- [x] Optimize performance
- [x] Add LUFS meter
- [x] Polish UI

**Milestone:** Plugin is production-ready

---

### Phase 5: Testing & Validation

- [x] Unit testing
- [x] Integration testing
- [x] DAW testing
- [x] Performance testing
- [x] Final review

**Milestone:** Plugin passes all tests

---

## Tools & Scripts

### Setup Scripts

- `setup_metadata.py` - Interactive metadata configuration
- `build.sh` - Build script
- `clean.sh` - Clean build artifacts

### Dashboard Tools

- `metadata_dashboard.py` - TUI dashboard
- `generate_html_dashboard.py` - HTML dashboard

### Testing Scripts

- `tests/run_tests.sh` - Integration test runner

---

## Best Practices

### Code Standards

- **Language**: C++17
- **Framework**: JUCE 8
- **Linter**: CLANG-TIDY
- **Naming**: PascalCase (classes), camelCase (methods)

### Thread Safety

- **Audio Thread**: No allocations, no UI access, no locks
- **UI Thread**: Can access model via thread-safe APIs
- **Parameters**: Use `std::atomic` or `juce::AudioParameter*`

### Documentation

- Update REFERENCE.md when adding constants
- Document all SSOT files
- Create migration plan for each project
- Review code at each phase

---

## JUCE 8 Compatibility

### Important Notes

1. **Module Includes**: Use module-specific includes instead of `<JuceHeader.h>`
   ```cpp
   #include <juce_audio_processors/juce_audio_processors.h>
   #include <juce_graphics/juce_graphics.h>
   ```

2. **juce::Colour**: Constructor is not constexpr in JUCE 8
   ```cpp
   // ❌ Wrong
   constexpr juce::Colour COLOR = juce::Colour(60, 180, 60);
   
   // ✅ Correct
   inline juce::Colour COLOR() { return juce::Colour::fromRGB(60, 180, 60); }
   ```

3. **juce::Font**: Use FontOptions instead of deprecated constructor
   ```cpp
   // ❌ Wrong
   g.setFont(juce::Font(14.0f));
   
   // ✅ Correct
   g.setFont(juce::FontOptions().withHeight(14.0f));
   ```

---

## Build Commands

### Using build.sh (Recommended)

```bash
# Run build script
./build.sh

# Or manually:
chmod +x build.sh
./build.sh
```

The `build.sh` script handles:
- Creating build directory
- Configuring with CMake
- Building all targets
- Verifying output artifacts

---

### Manual Build

### macOS

```bash
# Create build directory
mkdir build
cd build

# Configure with JUCE symlink
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64" \
    -DCMAKE_OSX_DEPLOYMENT_TARGET="12.0"

# Build
cmake --build . --config Release

# Install
cmake --build . --target install
```

**Universal Binary Support:**
```bash
# Build universal binary (Intel + Apple Silicon)
./build.sh universal

# Verify architecture
lipo -info ~/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3/Contents/x86_64/BULLsEYE
# Output: Architectures in the fat file: BULLsEYE are: x86_64 arm64
```

### Verification

```bash
# Check build artifacts
ls -la *.vst3/
ls -la *.component/
```

---

## Customization Guide

### 1. Rename Plugin

Edit `CMakeLists.txt`:
```cmake
project(YourPluginName VERSION 1)
juce_add_plugin(YourPluginName
    COMPANY_NAME "Your Company"
    PLUGIN_CODE YPCD
    PRODUCT_NAME "Your Plugin"
    # ...
)
```

### 2. Update Identity

Edit `REFERENCE.md`:
```markdown
- **Plugin Name:** Your Plugin
- **Plugin Code:** YPCD
- **Company:** Your Company
- **Version:** 1.0.0
```

### 3. Add Parameters

Edit `Source/SSOT/ModelSSOT.h`:
```cpp
namespace ParameterIDs
{
    constexpr const char* YOUR_PARAM = "yourParam";
}

namespace ParameterNames
{
    constexpr const char* YOUR_PARAM = "Your Parameter";
}
```

### 4. Add Colors

Edit `Source/SSOT/UISSOT.h`:
```cpp
namespace Colors
{
    inline juce::Colour YOUR_COLOR() { 
        return juce::Colour::fromRGB(R, G, B); 
    }
}
```

---

## Troubleshooting

### Build Errors

1. **Module not found**: Ensure JUCE symlink/submodule is set up
2. **Missing symbols**: Check CMake include directories
3. **Compilation errors**: Verify JUCE 8 compatibility

### Runtime Issues

1. **Parameter not updating**: Check APVTS connection
2. **Audio not processing**: Verify processBlock implementation
3. **UI not showing**: Check component visibility and layout

---

## Contributing

This template is based on the IntelliMute and BULLsEYE migration projects. To contribute:

1. Fork the repository
2. Make improvements
3. Submit pull request
4. Document changes

---

## License

MIT License - Use freely for your projects.

---

## Document Index

### Getting Started (Read in This Order)
1. **PORTING_GUIDE.md** — Main reference (framework migration strategy)
2. **MIGRATION_LAWS.md** — Detailed laws (15 laws + incidents + templates)
3. **FRAMEWORK_MIGRATION.md** — What's new in v2.0 (this release)

### Reference Documentation
- **REFERENCE.md** — Algorithm documentation (constants, formulas)
- **LEARNING.md** — Development notes and discoveries
- **MIGRATION_PLAN.md** — Roadmap template for your project

### Project Structure
- **Source/SSOT/** — Centralized constants (no hardcoding)
- **Source/DSP/** — DSP implementation
- **Source/Components/** — UI components
- **jsfx/** — Your original JSFX files
- **build/** — Compiled plugins (gitignored)

### Code Templates
- **templates/COMPONENT_TEMPLATE.md** — UI component template
- **templates/DSP_TEMPLATE.md** — DSP processor template
- **templates/SSOT_TEMPLATE.md** — SSOT constants template
- **templates/REVIEW_TEMPLATE.md** — Code review template

---

## Acknowledgments

- **JUCE Framework**: https://juce.com
- **Virtue DNA Framework**: Internal development philosophy
- **TETRIS Principles**: Thread-safe DSP architecture
- **BULLsEYE Project**: Real-world case study (Feb 2026)
- **ITU-R BS.1770**: International loudness standard

---

**Template Version:** 2.0.0  
**Created:** 2025-12-20  
**Updated:** 2026-02-07  
**Based on:** IntelliMute + BULLsEYE Migration Projects  
**Status:** Production-ready, tested on 3+ real plugins