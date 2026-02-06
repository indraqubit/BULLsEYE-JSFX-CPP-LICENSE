# Portable JSFX to JUCE Migration Template

**Version:** 1.0.0  
**Date:** 2025-12-20  
**JUCE Version:** 8  
**C++ Standard:** C++17

---

## Overview

This is a **portable, drop-in template** for migrating JSFX plugins to JUCE 8/C++17. It includes all best practices, frameworks, and patterns developed during the IntelliMute migration project.

## Features

- **SSOT Architecture**: Single Source of Truth for all constants
- **TETRIS Compliance**: Thread-safe DSP implementation
- **Virtue DNA Framework**: Development philosophy and principles
- **Professional UI**: Modern dark theme with branding
- **CMake Build System**: Modern build configuration
- **Complete Documentation**: Templates for all phases

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

## Project Structure

```
portable-jsfx-to-juce-migration/
├── CMakeLists.txt              # Build configuration
├── REFERENCE.md                # Plugin identity & constants (SSOT)
├── LEARNINGS.md                # Development notes
├── MIGRATION_PLAN.md           # Migration roadmap template
│
├── Source/
│   ├── SSOT/                   # Single Source of Truth files
│   │   ├── ModelSSOT.h         # Model layer constants
│   │   ├── UISSOT.h            # UI layer constants
│   │   ├── ProcessorSSOT.h      # Processor layer constants
│   │   └── DSPSSOT.h           # DSP layer constants
│   │
│   ├── DSP/                    # DSP implementation
│   │   └── YourProcessor.h      # TETRIS-compliant processor
│   │
│   ├── Components/             # UI components
│   │   ├── StatusDisplayComponent.h/cpp
│   │   ├── ModeSelectorComponent.h/cpp
│   │   ├── MuteButtonComponent.h/cpp
│   │   ├── InvertToggleComponent.h/cpp
│   │   └── ModeDescriptionComponent.h/cpp
│   │
│   ├── PluginProcessor.h/cpp   # Main audio processor
│   └── PluginEditor.h/cpp       # Main UI editor
│
├── modules/
│   └── JUCE/                   # JUCE as submodule/symlink
│
├── templates/                  # Code templates
│   ├── COMPONENT_TEMPLATE.md
│   ├── DSP_TEMPLATE.md
│   ├── SSOT_TEMPLATE.md
│   └── REVIEW_TEMPLATE.md
│
└── .cursor/
    └── rules                   # AI assistant rules
```

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

- [ ] Set up project structure
- [ ] Create SSOT files
- [ ] Configure CMake
- [ ] Set up metadata (TUI tool)
- [ ] Create basic processor skeleton

### Phase 2: DSP Core

- [ ] Implement TETRIS-compliant processor
- [ ] Implement mode logic
- [ ] Implement parameter handling
- [ ] Implement audio processing

### Phase 3: UI Implementation

- [ ] Create PluginEditor base
- [ ] Create UI components
- [ ] Connect components to APVTS
- [ ] Implement visual feedback

### Phase 4: Integration & Polish

- [ ] Test all functionality
- [ ] Handle edge cases
- [ ] Optimize performance
- [ ] Polish UI

### Phase 5: Testing & Validation

- [ ] Unit testing
- [ ] Integration testing
- [ ] DAW testing
- [ ] Performance testing

---

## Tools & Scripts

### Setup Scripts

- `setup_metadata.py` - Interactive metadata configuration
- `build.sh` - Build script
- `clean.sh` - Clean build artifacts

### Dashboard Tools

- `metadata_dashboard.py` - TUI dashboard
- `generate_html_dashboard.py` - HTML dashboard

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

### macOS

```bash
# Create build directory
mkdir build
cd build

# Configure with JUCE symlink
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build . --config Release

# Install
cmake --build . --target install
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

This template is based on the IntelliMute migration project. To contribute:

1. Fork the repository
2. Make improvements
3. Submit pull request
4. Document changes

---

## License

MIT License - Use freely for your projects.

---

## Acknowledgments

- **JUCE Framework**: https://juce.com
- **Virtue DNA Framework**: Internal development philosophy
- **TETRIS Principles**: Thread-safe DSP architecture

---

**Template Version:** 1.0.0  
**Created:** 2025-12-20  
**Based on:** IntelliMute Migration Project
