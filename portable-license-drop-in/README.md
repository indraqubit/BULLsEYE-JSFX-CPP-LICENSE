# Portable License Drop-In Kit

A modular, DRY-compliant licensing system for audio plugins and commercial software products.

**Status:** Verified (2026-02-01)  
**Cross-Platform:** Windows ✅ | macOS ✅ | Linux ✅  
**Related:** [Blindspots Analysis](../osx/BLINDSPOT_FIX_PLAN.md) | [Law 10: Third-Party Library Handling](../laws/10_THIRD_PARTY_LIBRARY_HANDLING.md)

---

## Overview

This kit provides a complete licensing infrastructure that can be dropped into any C++ audio plugin project with minimal integration effort. It's designed to work across different plugin frameworks (VST3, AU, AAX, etc.) while maintaining a consistent licensing experience.

## ⚠️ Important: Thread Safety Design

This library provides **two distinct API sets** for different threading contexts:

### Audio Thread API (Lock-Free, Non-Blocking)
These methods are **SAFE to call from audio thread** (`processBlock`):
- `isLicensedAtomically()` - Quick license status check
- `getDaysRemainingAtomically()` - Days remaining (non-blocking)
- `isInGracePeriodAtomically()` - Grace period check
- `isFeatureEnabledAtomically(index)` - Feature gate check

### UI/Background Thread API (May Block)
These methods **MUST be called from UI or background thread**:
- `initialize()` - One-time initialization (constructor)
- `activateLicense()` - License activation (blocking)
- `refreshLicenseStatus()` - Background validation
- `getLicenseStatusMessage()` - Human-readable status
- `isLicensed()` / `isTrial()` / `isExpired()` - Full status checks

**Why this design?** License systems don't need sample-accurate checking. The correct pattern is:
1. Check license status once at plugin load (UI thread)
2. Cache result in atomic snapshot
3. Read atomic snapshot from audio thread (non-blocking)

See `LICENSE_DROP_IN_LAWS.md` for detailed threading laws.

## Key Features

- **Dual API Design**: Separate audio-thread-safe and UI-thread APIs
- **Thread-Safe**: Atomic operations for audio thread, mutex for background updates
- **Minimal Dependencies**: No external libraries required for core functionality
- **DRY Principle**: Single implementation, multiple use cases
- **Comprehensive**: Trial, subscription, and perpetual licensing models
- **Offline Validation**: Works without internet connectivity
- **Grace Periods**: Configurable subscription grace periods (default 7 days)
- **Drop-Data Fallback**: Audio continues with last known good state on validation skip

## Quick Start

### 1. Include the Header

```cpp
#include "portable-license-drop-in/core/LICENSE_ENGINE.hpp"
```

### 2. Initialize in Plugin Constructor

```cpp
class MyPlugin : public juce::AudioProcessor {
private:
    LicenseEngine::LicenseEngine licenseEngine;
    
public:
    MyPlugin() {
        // Initialize with your product configuration
        LicenseEngine::LicenseConfig config;
        config.companyName = "MyCompany";
        config.productName = "MyPlugin";
        config.productVersion = "1.0.0";
        config.trialDays = 14;
        config.subscriptionGracePeriodDays = 7;
        
        licenseEngine.initialize(config);
    }
};
```

### 3. Check License from Audio Thread (Non-Blocking)

```cpp
void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) override {
    // AUDIO THREAD: Use atomic API only - NEVER block
    if (!licenseEngine.isLicensedAtomically()) {
        buffer.clear();  // Silence for unlicensed
        return;
    }
    
    // Normal audio processing...
}
```

### 4. Check License from UI Thread (Blocking)

```cpp
// UI THREAD: Can use blocking API
void showLicenseStatus() {
    if (licenseEngine.isLicensed()) {
        showValidLicenseUI();
    } else {
        showActivationDialog();
    }
}
```

## Directory Structure

```
portable-license-drop-in/
├── README.md                      # This file
├── LICENSE                        # License for the kit itself
├── LICENSE_DROP_IN_LAWS.md       # Rules and best practices (READ THIS)
├── INTEGRATION_GUIDE.md          # Detailed integration guide
├── DEPENDENCIES_ASSESSMENT.md    # Platform-specific dependencies
├── LEARNING_CURVE_ANALYSIS.md    # Learning curve evaluation
├── baseline/                      # Baseline laws extracted from parent project
│   └── 01_THREAD_SAFETY.md       # Audio thread safety laws
├── core/                          # Core implementation
│   ├── LICENSE_ENGINE.hpp        # Main license engine (audio-safe)
│   ├── LICENSE_ENGINE_EXTRACTOR.hpp  # Source extraction layer
│   └── LICENSE_ENGINE_FACTORIZED.hpp # Factored implementation with DI
└── examples/                      # Example implementations
    ├── VST3/                      # VST3 integration example
    ├── AU/                        # Audio Unit integration example
    ├── Standalone/                # Standalone application example
    └── AnalogMorphV3Plugin/       # Source extraction example
```

## Integration Steps

### Basic Integration (Recommended)

Use the simple drop-in for most audio plugins:

```cpp
#include "portable-license-drop-in/core/LICENSE_ENGINE.hpp"

class MyPlugin : public juce::AudioProcessor {
private:
    LicenseEngine::LicenseEngine licenseEngine;
    
public:
    MyPlugin() {
        LicenseEngine::LicenseConfig config;
        config.companyName = "MyCompany";
        config.productName = "MyPlugin";
        config.productVersion = "1.0.0";
        config.trialDays = 14;
        config.subscriptionGracePeriodDays = 7;
        
        licenseEngine.initialize(config);
    }
    
    void processBlock(juce::AudioBuffer<float>& buffer, 
                      juce::MidiBuffer& midi) override {
        // Use atomic API for audio thread safety
        if (!licenseEngine.isLicensedAtomically()) {
            buffer.clear();
            return;
        }
        
        // Normal processing...
    }
};
```

### Advanced: Try-or-Skip Pattern

For non-blocking background operations:

```cpp
void refreshLicenseAsync() {
    auto result = licenseEngine.tryRefreshStatus();
    if (result == LicenseEngine::LicenseResult::WouldBlock) {
        // Skip this refresh, audio uses cached state
        return;
    }
    // Update UI...
}
```

### Advanced: Drop-Data Fallback

For graceful degradation:

```cpp
void processBlock(juce::AudioBuffer<float>& buffer, 
                  juce::MidiBuffer& midi) override {
    if (licenseEngine.isLicensedAtomically()) {
        // Fully licensed processing
        processFull(buffer);
    } else {
        // Try last known good state
        LicenseEngine::LicenseInfo lastState;
        if (licenseEngine.getLastKnownGoodState(lastState)) {
            // Degraded mode with last valid license
            processDegraded(buffer, lastState);
        } else {
            buffer.clear();
        }
    }
}
```

## License Validation

The license engine supports multiple validation modes:

- **Trial Periods**: Time-limited trials with feature restrictions (14+ days)
- **Subscription Licenses**: Time-based licenses with configurable grace periods
- **Perpetual Licenses**: One-time purchase licenses (no expiry)
- **Feature-Limited Licenses**: Licenses with specific feature restrictions

## Best Practices

1. **Always use atomic API in audio thread** - `isLicensedAtomically()` and related methods
2. **Initialize once in constructor** - Never reinitialize during audio processing
3. **Use try-or-skip for background tasks** - Prevents blocking on mutex contention
4. **Implement graceful degradation** - Use `getLastKnownGoodState()` for drop-data fallback
5. **Respect grace periods** - Don't immediately disable expired subscriptions
6. **Pre-allocate memory** - No heap allocation in audio processing path

## Audio Thread Safety

This library follows the audio thread safety laws defined in:
- `docs/audio-safety/AUDIO_THREAD_SAFETY_LAW.md`
- `LICENSE_DROP_IN_LAWS.md`

**Key Principles:**
- Audio thread: Non-blocking atomic reads only
- Background thread: Mutex-protected writes with try-or-skip pattern
- No heap allocation in audio processing path
- Graceful degradation via last known good state

## Licensing

This kit itself is licensed under the terms specified in the `LICENSE` file. Products using this kit must comply with both this license and the terms outlined in `LICENSE_DROP_IN_LAWS.md`.

## Support

For questions or issues with the license drop-in kit, please refer to:
- `LICENSE_DROP_IN_LAWS.md` - Detailed rules and best practices
- `INTEGRATION_GUIDE.md` - Step-by-step integration instructions
- `docs/audio-safety/` - Audio thread safety documentation
