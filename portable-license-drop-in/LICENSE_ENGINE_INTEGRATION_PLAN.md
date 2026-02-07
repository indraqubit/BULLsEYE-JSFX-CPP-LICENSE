# Comprehensive Integration Plan: Portable License Drop-In Kit → BULLsEYE Plugin

**Document Version:** 1.0  
**Date:** 2026-02-07  
**Author:** @indraqubit  
**Status:** PLANNING  
**Derived From:**
- [`portable-license-drop-in/`](../portable-license-drop-in/) — License engine kit
- [`ANMO-OSX-Staging/laws/osx/MACOS_VST3_DEPLOYMENT_LAWS.md`](https://github.com/indraqubit/ANMO-OSX-Staging/blob/main/laws/osx/MACOS_VST3_DEPLOYMENT_LAWS.md) — macOS deployment dependencies
- [`LAW_BOOK.md`](../LAW_BOOK.md) — BULLsEYE architecture laws

---

## Table of Contents

1. [Executive Summary](#1-executive-summary)
2. [Current State Assessment](#2-current-state-assessment)
3. [Integration Strategy](#3-integration-strategy)
4. [Phase 1: Foundation — CMake & Include Path Setup](#4-phase-1-foundation--cmake--include-path-setup)
5. [Phase 2: Core Engine — PluginProcessor Integration](#5-phase-2-core-engine--pluginprocessor-integration)
6. [Phase 3: UI — PluginEditor License Components](#6-phase-3-ui--plugineditor-license-components)
7. [Phase 4: macOS Deployment Compliance (ANMO Laws)](#7-phase-4-macos-deployment-compliance-anmo-laws)
8. [Phase 5: Cross-Platform Deployment](#8-phase-5-cross-platform-deployment)
9. [Phase 6: Testing & Validation](#9-phase-6-testing--validation)
10. [Phase 7: Release & Documentation](#10-phase-7-release--documentation)
11. [Dependency Matrix](#11-dependency-matrix)
12. [Risk Assessment](#12-risk-assessment)
13. [Integration Checklist](#13-integration-checklist)
14. [File-by-File Change Map](#14-file-by-file-change-map)

---

## 1. Executive Summary

### Goal

Integrate the **Portable License Drop-In Kit** (`portable-license-drop-in/`) into the **BULLsEYE** LUFS-I meter plugin, adding commercial licensing support (trial, activation, subscription) while preserving:

- **Audio thread safety** (Law Book Laws 1-31)
- **SSOT architecture** (no hardcoded values)
- **TETRIS-compliant DSP** (zero-allocation audio path)
- **macOS deployment portability** (ANMO OSX deployment laws 1-9)
- **Cross-platform builds** (VST3 + AU on macOS, VST3 on Windows/Linux)

### Recommended Integration Approach

**Simple Drop-In** (Approach #1 from `INTEGRATION_GUIDE.md`) — cleanest for a new product that doesn't need AnalogMorphV333 extraction. The BULLsEYE plugin has its own identity, SSOT, and DSP core.

### Estimated Effort

| Phase | Duration | Complexity |
|-------|----------|------------|
| Phase 1: CMake & Include Paths | 1 day | Low |
| Phase 2: PluginProcessor Integration | 2 days | Medium |
| Phase 3: UI License Components | 2 days | Medium |
| Phase 4: macOS Deployment Compliance | 2 days | High |
| Phase 5: Cross-Platform Deployment | 1 day | Medium |
| Phase 6: Testing & Validation | 3 days | High |
| Phase 7: Release & Documentation | 1 day | Low |
| **Total** | **~12 days** | |

---

## 2. Current State Assessment

### BULLsEYE Plugin (Host)

```
Source/
├── PluginProcessor.h/cpp      — BULLsEYEProcessor : juce::AudioProcessor
├── PluginEditor.h/cpp         — BULLsEYEEditor : juce::AudioProcessorEditor
├── SSOT/
│   ├── ModelSSOT.h            — Parameters, content types, enums
│   ├── UISSOT.h               — Colors, fonts, layout constants
│   ├── ProcessorSSOT.h        — Processing constants
│   └── DSPSSOT.h              — DSP algorithm constants
├── DSP/
│   └── BULLsEYEProcessor.h    — TETRIS-compliant DSP core
└── Components/
    ├── StatusDisplayComponent.h/cpp
    ├── ModeSelectorComponent.h/cpp
    └── CircularMeterComponent.h/cpp
```

**Key characteristics:**
- JUCE 7+ framework (submodule at `modules/JUCE`)
- C++17 standard
- Formats: VST3 + AU
- Universal binary: x86_64 + arm64
- macOS 12.0+ deployment target
- 42 integration tests, 100% pass rate
- APVTS-based parameter management
- No current licensing or network dependencies

### Portable License Drop-In Kit (Guest)

```
portable-license-drop-in/
├── core/
│   ├── LICENSE_ENGINE.hpp              — Main engine (audio-safe atomic API)
│   ├── LICENSE_ENGINE_EXTRACTOR.hpp    — Source extraction layer
│   └── LICENSE_ENGINE_FACTORIZED.hpp   — DI/factory pattern variant
├── baseline/
│   └── 01_THREAD_SAFETY.md            — Audio thread safety laws
├── examples/
│   ├── VST3/VST3PluginExample.cpp
│   ├── AU/AUPluginExample.mm
│   └── AnalogMorphV3Plugin/AnalogMorphV3PluginExample.cpp
├── LICENSE_DROP_IN_LAWS.md
├── INTEGRATION_GUIDE.md
├── DEPENDENCIES_ASSESSMENT.md
├── LEARNING_CURVE_ANALYSIS.md
└── VERIFICATION_REPORT.md
```

**Key characteristics:**
- Header-only core (`LICENSE_ENGINE.hpp`)
- Two-tier API: atomic (audio thread) + blocking (UI thread)
- Platform-specific storage: Keychain (macOS), Registry (Windows), file (Linux)
- Optional network validation (OpenSSL on macOS)
- C++11 minimum, C++17 compatible

### ANMO macOS Deployment Laws (Dependencies Reference)

9 foundational laws governing macOS VST3/AU deployment:

| Law | Title | Impact on Integration |
|-----|-------|----------------------|
| LAW 1 | Bundle Integrity | License data must respect `.vst3`/`.component` bundle structure |
| LAW 2 | Dylib Linking | OpenSSL for HTTPS license validation → `Contents/Frameworks/` |
| LAW 3 | Architecture-Agnostic Binding | Homebrew paths auto-detect for OpenSSL |
| LAW 4 | Clean System Testing | License must work without Homebrew on user Macs |
| LAW 5 | Silent Codesigning | Signed bundles required, license engine must survive codesign |
| LAW 6 | Cross-Architecture Validation | License engine must work on both arm64 + x86_64 |
| LAW 7 | Release Tag Covenant | License version tracked in release tags |
| LAW 8 | Code Signing & Notarization | Hardened Runtime compatibility for license engine |
| LAW 9 | Rpath Management | OpenSSL dylibs must use `@rpath`, not absolute paths |

---

## 3. Integration Strategy

### 3.1 Approach Selection: Simple Drop-In

**Why Simple Drop-In (not Extraction or Factored):**

| Criteria | Simple Drop-In | Source Extraction | Factored DI |
|----------|---------------|-------------------|-------------|
| BULLsEYE is a new product | ✅ Perfect | ❌ Overkill | ❌ Overkill |
| No AnalogMorph dependency | ✅ Clean | ❌ Unnecessary coupling | ❌ Unnecessary |
| Header-only integration | ✅ Single include | ⚠️ Multiple includes | ⚠️ Complex setup |
| SSOT compliance | ✅ Config in SSOT | ⚠️ External config | ⚠️ Factory config |
| Build complexity | ✅ Minimal | ⚠️ Moderate | ❌ High |

### 3.2 Architecture Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                    BULLsEYE Plugin                           │
│                                                              │
│  ┌──────────────────────┐    ┌────────────────────────────┐ │
│  │  BULLsEYEProcessor   │    │    BULLsEYEEditor          │ │
│  │  (Audio Thread)       │    │    (UI Thread)             │ │
│  │                       │    │                            │ │
│  │  ┌─────────────────┐ │    │  ┌────────────────────┐    │ │
│  │  │ dspCore          │ │    │  │ CircularMeter      │    │ │
│  │  │ (BULLsEYEProc)  │ │    │  │ StatusDisplay      │    │ │
│  │  └─────────────────┘ │    │  │ ModeSelector       │    │ │
│  │                       │    │  │ ┌──────────────┐   │    │ │
│  │  ┌─────────────────┐ │    │  │ │LicenseStatus │   │    │ │
│  │  │ licenseEngine    │ │    │  │ │Component     │   │    │ │
│  │  │ .isLicensed      │◄├────┤  │ │(NEW)         │   │    │ │
│  │  │  Atomically()    │ │    │  │ └──────────────┘   │    │ │
│  │  └─────────────────┘ │    │  └────────────────────┘    │ │
│  └──────────────────────┘    └────────────────────────────┘ │
│                                                              │
│  ┌──────────────────────────────────────────────────────┐   │
│  │              SSOT Layer                                │   │
│  │  ModelSSOT.h  │  UISSOT.h  │  LicenseSSOT.h (NEW)   │   │
│  └──────────────────────────────────────────────────────┘   │
│                                                              │
│  ┌──────────────────────────────────────────────────────┐   │
│  │       portable-license-drop-in/core/                   │   │
│  │       LICENSE_ENGINE.hpp (header-only)                  │   │
│  └──────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────┘
```

### 3.3 Thread Safety Model

```
AUDIO THREAD (processBlock)          UI THREAD (editor, timer)
━━━━━━━━━━━━━━━━━━━━━━━━━━          ━━━━━━━━━━━━━━━━━━━━━━━━━
licenseEngine                        licenseEngine
  .isLicensedAtomically()   ←atomic→   .isLicensed()
  .isFeatureEnabledAtomically()        .activateLicense()
  .getLastKnownGoodState()             .tryRefreshStatus()
                                       .getLicenseStatusMessage()
                                       .initialize()
```

---

## 4. Phase 1: Foundation — CMake & Include Path Setup

### 4.1 CMakeLists.txt Changes

**File:** `CMakeLists.txt`

Add include path for the license engine and any new source files:

```cmake
# ========================================================================
# INCLUDE DIRECTORIES
# ========================================================================

target_include_directories(BULLsEYE PRIVATE
    Source
    ${CMAKE_CURRENT_SOURCE_DIR}/portable-license-drop-in   # NEW: License engine
)
```

Add the license SSOT header to sources:

```cmake
target_sources(BULLsEYE PRIVATE
    # Main files
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h

    # SSOT files
    Source/SSOT/ModelSSOT.h
    Source/SSOT/UISSOT.h
    Source/SSOT/ProcessorSSOT.h
    Source/SSOT/DSPSSOT.h
    Source/SSOT/LicenseSSOT.h          # NEW: License constants

    # DSP
    Source/DSP/BULLsEYEProcessor.h

    # Components
    Source/Components/StatusDisplayComponent.cpp
    Source/Components/StatusDisplayComponent.h
    Source/Components/ModeSelectorComponent.cpp
    Source/Components/ModeSelectorComponent.h
    Source/Components/CircularMeterComponent.cpp
    Source/Components/CircularMeterComponent.h
    Source/Components/LicenseStatusComponent.cpp  # NEW
    Source/Components/LicenseStatusComponent.h    # NEW
)
```

### 4.2 Conditional OpenSSL Linking (macOS — Per ANMO LAW 2 & 3)

If network license validation is required, add OpenSSL:

```cmake
# ========================================================================
# LICENSE ENGINE DEPENDENCIES (Optional: Network validation)
# ========================================================================

option(BULLSEYE_ENABLE_NETWORK_LICENSE "Enable network license validation" OFF)

if(BULLSEYE_ENABLE_NETWORK_LICENSE)
    if(APPLE)
        # LAW 3: Architecture-agnostic auto-detection
        if(EXISTS "/opt/homebrew/opt/openssl@3")
            set(OPENSSL_ROOT_DIR "/opt/homebrew/opt/openssl@3")
            message(STATUS "License Engine: Apple Silicon OpenSSL detected")
        elseif(EXISTS "/usr/local/opt/openssl@3")
            set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl@3")
            message(STATUS "License Engine: Intel OpenSSL detected")
        else()
            message(WARNING "OpenSSL not found - network license validation disabled")
            set(BULLSEYE_ENABLE_NETWORK_LICENSE OFF)
        endif()

        if(BULLSEYE_ENABLE_NETWORK_LICENSE)
            find_package(OpenSSL REQUIRED)
            target_link_libraries(BULLsEYE PRIVATE OpenSSL::SSL OpenSSL::Crypto)

            # LAW 2: Bundle dylibs into Contents/Frameworks/
            # LAW 9: Patch @rpath references
            add_custom_command(TARGET BULLsEYE POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                    "$<TARGET_BUNDLE_DIR:BULLsEYE>/Contents/Frameworks"
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${OPENSSL_ROOT_DIR}/lib/libssl.3.dylib"
                    "$<TARGET_BUNDLE_DIR:BULLsEYE>/Contents/Frameworks/"
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${OPENSSL_ROOT_DIR}/lib/libcrypto.3.dylib"
                    "$<TARGET_BUNDLE_DIR:BULLsEYE>/Contents/Frameworks/"
                COMMAND install_name_tool -change
                    "${OPENSSL_ROOT_DIR}/lib/libssl.3.dylib"
                    "@rpath/libssl.3.dylib"
                    "$<TARGET_FILE:BULLsEYE>"
                COMMAND install_name_tool -change
                    "${OPENSSL_ROOT_DIR}/lib/libcrypto.3.dylib"
                    "@rpath/libcrypto.3.dylib"
                    "$<TARGET_FILE:BULLsEYE>"
                COMMENT "Bundling OpenSSL dylibs (LAW 2/9 compliance)"
            )
        endif()
    elseif(WIN32)
        # Windows: WinHTTP is available by default, no OpenSSL needed
        target_link_libraries(BULLsEYE PRIVATE winhttp)
    endif()

    target_compile_definitions(BULLsEYE PRIVATE
        BULLSEYE_NETWORK_LICENSE=1
    )
endif()
```

### 4.3 New SSOT File: LicenseSSOT.h

**File:** `Source/SSOT/LicenseSSOT.h`

```cpp
#pragma once

#include <string>

/**
 * LicenseSSOT — Single Source of Truth for License Configuration
 *
 * All license-related constants live here.
 * No hardcoded values anywhere else in the codebase.
 */
namespace LicenseSSOT
{
    // ========================================================================
    // PRODUCT IDENTITY
    // ========================================================================
    constexpr const char* COMPANY_NAME     = "IQ";
    constexpr const char* PRODUCT_NAME     = "BULLsEYE";
    constexpr const char* PRODUCT_VERSION  = "2.0.0";
    constexpr const char* BUNDLE_ID        = "com.iq.bullseye";

    // ========================================================================
    // TRIAL CONFIGURATION
    // ========================================================================
    constexpr int TRIAL_DAYS                    = 14;    // Law: minimum 14 days for pro audio
    constexpr int SUBSCRIPTION_GRACE_PERIOD_DAYS = 7;    // Law: minimum 7 days
    constexpr int OFFLINE_VALIDATION_DAYS        = 30;   // Law: minimum 30 days
    constexpr bool ALLOW_OFFLINE_ACTIVATION      = true;

    // ========================================================================
    // VALIDATION TIMING
    // ========================================================================
    constexpr int SUBSCRIPTION_VALIDATION_INTERVAL_HOURS = 24;  // Daily
    constexpr int PERPETUAL_VALIDATION_INTERVAL_HOURS    = 168; // Weekly

    // ========================================================================
    // TRIAL RESTRICTIONS
    // ========================================================================
    constexpr bool TRIAL_MUTE_AUDIO     = false;  // false = allow audio in trial
    constexpr bool TRIAL_SHOW_WATERMARK = true;   // Show "TRIAL" overlay
    constexpr int  TRIAL_NOISE_BURST_INTERVAL_SEC = 0;  // 0 = disabled

    // ========================================================================
    // LICENSE STORAGE
    // ========================================================================
    constexpr const char* LICENSE_FILE_NAME = "BULLsEYE_License.dat";

    // macOS: ~/Library/Application Support/IQ/BULLsEYE/
    // Windows: %APPDATA%/IQ/BULLsEYE/
    // Linux: ~/.config/IQ/BULLsEYE/
    constexpr const char* LICENSE_STORAGE_SUBDIR = "IQ/BULLsEYE";

    // ========================================================================
    // UI STRINGS
    // ========================================================================
    constexpr const char* TRIAL_LABEL     = "TRIAL";
    constexpr const char* LICENSED_LABEL  = "Licensed";
    constexpr const char* EXPIRED_LABEL   = "License Expired";
    constexpr const char* ACTIVATE_BUTTON = "Activate License";
}
```

---

## 5. Phase 2: Core Engine — PluginProcessor Integration

### 5.1 PluginProcessor.h Changes

```cpp
#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include "SSOT/ModelSSOT.h"
#include "SSOT/LicenseSSOT.h"                              // NEW
#include "DSP/BULLsEYEProcessor.h"
#include "core/LICENSE_ENGINE.hpp"                           // NEW

class BULLsEYEProcessor : public juce::AudioProcessor
{
public:
    BULLsEYEProcessor();
    ~BULLsEYEProcessor() override;

    // ... existing public methods ...

    // ========================================================================
    // LICENSE ACCESS (for UI)                                        // NEW
    // ========================================================================
    bool isLicensedForUI() const { return licenseEngine.isLicensed(); }
    juce::String getLicenseStatus() const;
    bool activateLicense(const juce::String& key);
    int getTrialDaysRemaining() const;

private:
    juce::AudioProcessorValueTreeState apvts;
    BULLsEYEProcessorCore dspCore;
    bool wasPlaying{false};

    // ========================================================================
    // LICENSE ENGINE                                                  // NEW
    // ========================================================================
    LicenseEngine::LicenseEngine licenseEngine;

    void initializeLicenseEngine();                                   // NEW

    // ... existing private methods ...
};
```

### 5.2 PluginProcessor.cpp Changes

**Constructor — Initialize license engine:**

```cpp
BULLsEYEProcessor::BULLsEYEProcessor()
    : juce::AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true))
    , apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    initializeLicenseEngine();  // NEW: Initialize license on construction (UI thread)
}

void BULLsEYEProcessor::initializeLicenseEngine()
{
    LicenseEngine::LicenseConfig config;
    config.companyName                = LicenseSSOT::COMPANY_NAME;
    config.productName                = LicenseSSOT::PRODUCT_NAME;
    config.productVersion             = LicenseSSOT::PRODUCT_VERSION;
    config.trialDays                  = LicenseSSOT::TRIAL_DAYS;
    config.subscriptionGracePeriodDays = LicenseSSOT::SUBSCRIPTION_GRACE_PERIOD_DAYS;
    config.allowOfflineActivation      = LicenseSSOT::ALLOW_OFFLINE_ACTIVATION;

    if (!licenseEngine.initialize(config))
    {
        // Graceful degradation: start in trial mode
        DBG("License engine initialization failed — defaulting to trial mode");
    }
}
```

**processBlock — Add atomic license gate:**

```cpp
void BULLsEYEProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    // ================================================================
    // LICENSE GATE (atomic, lock-free — safe for audio thread)     NEW
    // ================================================================
    const bool licensed = licenseEngine.isLicensedAtomically();

    // If unlicensed AND trial muting is enabled, silence output
    if (!licensed && LicenseSSOT::TRIAL_MUTE_AUDIO)
    {
        buffer.clear();
        return;
    }

    // Transport state detection (existing code)
    bool isCurrentlyPlaying = false;
    if (auto* playHead = getPlayHead())
    {
        if (auto position = playHead->getPosition())
        {
            isCurrentlyPlaying = position->getIsPlaying();
            if (isCurrentlyPlaying && !wasPlaying)
                dspCore.reset();
            wasPlaying = isCurrentlyPlaying;
        }
    }

    contentTypeChanged();

    float* leftIn = buffer.getWritePointer(0);
    float* rightIn = buffer.getWritePointer(1);

    for (int i = 0; i < buffer.getNumSamples(); i++)
        dspCore.process(leftIn[i], rightIn[i]);
}
```

### 5.3 State Persistence — License survives DAW session reload

```cpp
void BULLsEYEProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    // Note: License state is persisted separately by the license engine
    // (file-based or Keychain storage), NOT in APVTS state
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}
```

---

## 6. Phase 3: UI — PluginEditor License Components

### 6.1 New Component: LicenseStatusComponent

**File:** `Source/Components/LicenseStatusComponent.h`

```cpp
#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "core/LICENSE_ENGINE.hpp"
#include "../SSOT/LicenseSSOT.h"
#include "../SSOT/UISSOT.h"

class LicenseStatusComponent : public juce::Component, private juce::Timer
{
public:
    explicit LicenseStatusComponent(LicenseEngine::LicenseEngine& engine);
    ~LicenseStatusComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void timerCallback() override;  // Refresh license status at 1 Hz
    void updateDisplay();

    LicenseEngine::LicenseEngine& licenseEngine;

    juce::Label statusLabel;
    juce::Label daysRemainingLabel;
    juce::TextButton activateButton { LicenseSSOT::ACTIVATE_BUTTON };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LicenseStatusComponent)
};
```

### 6.2 PluginEditor Integration

Add the license status component to the editor layout, below the existing meter components:

```cpp
// In BULLsEYEEditor constructor:
licenseStatusComponent = std::make_unique<LicenseStatusComponent>(
    processor.getLicenseEngine());
addAndMakeVisible(*licenseStatusComponent);

// In resized():
// Reserve bottom 30px strip for license status
auto area = getLocalBounds();
auto licenseArea = area.removeFromBottom(30);
licenseStatusComponent->setBounds(licenseArea);
// ... existing component layout uses remaining `area` ...
```

### 6.3 UISSOT.h Additions

```cpp
// In UISSOT.h — License UI constants
namespace LicenseUI
{
    constexpr int STATUS_BAR_HEIGHT = 30;
    constexpr float STATUS_FONT_SIZE = 11.0f;

    // Colors
    const juce::Colour TRIAL_COLOR    = juce::Colour(0xFFFF9800);  // Orange
    const juce::Colour LICENSED_COLOR = juce::Colour(0xFF4CAF50);  // Green
    const juce::Colour EXPIRED_COLOR  = juce::Colour(0xFFF44336);  // Red
}
```

---

## 7. Phase 4: macOS Deployment Compliance (ANMO Laws)

### 7.1 Law-by-Law Compliance Checklist

| ANMO Law | Requirement | BULLsEYE Compliance Action |
|----------|-------------|----------------------------|
| **LAW 1: Bundle Integrity** | `.vst3`/`.component` are directory bundles | License files stored outside bundle (Application Support). No license data inside bundle tree |
| **LAW 2: Dylib Linking** | Dylibs in `Contents/Frameworks/` with `@rpath` | OpenSSL bundled only if `BULLSEYE_ENABLE_NETWORK_LICENSE=ON`. CMake auto-copies and patches |
| **LAW 3: Architecture-Agnostic** | Auto-detect Homebrew paths | CMake conditional: `/opt/homebrew` vs `/usr/local` (see Phase 1) |
| **LAW 4: Clean System Testing** | Test without Homebrew | License engine uses Keychain (no Homebrew dependency). OpenSSL bundled inside `.vst3` |
| **LAW 5: Silent Codesigning** | All builds must be signed | License engine has no anti-tamper that conflicts with `codesign --deep` |
| **LAW 6: Cross-Architecture** | Test arm64 + x86_64 | License engine uses `std::atomic` (portable). Machine ID hashing is arch-independent |
| **LAW 7: Release Tag** | Tags must include license version | Tag format: `v2.1.0-macos-universal-licensed-feb2026` |
| **LAW 8: Notarization** | Hardened Runtime + notarization | License engine must not require disabled hardened runtime entitlements |
| **LAW 9: Rpath Management** | No absolute Homebrew paths in binary | `install_name_tool` patches applied in CMake post-build step |

### 7.2 Bundle Structure After Integration

```
BULLsEYE.vst3/
├── Contents/
│   ├── MacOS/
│   │   └── BULLsEYE                     ← Mach-O binary (universal)
│   ├── Frameworks/                       ← Only if network license enabled
│   │   ├── libssl.3.dylib               ← Bundled, @rpath patched
│   │   └── libcrypto.3.dylib            ← Bundled, @rpath patched
│   └── Resources/
│       └── (presets, etc.)
└── Info.plist
```

### 7.3 License Data Storage (Outside Bundle)

```
~/Library/Application Support/IQ/BULLsEYE/
├── BULLsEYE_License.dat                  ← Encrypted license file
└── license_cache.json                     ← Offline validation cache
```

**Critical:** License data MUST NOT be stored inside the `.vst3`/`.component` bundle per LAW 1 (bundle integrity) — bundles may be replaced during updates.

### 7.4 Hardened Runtime Entitlements

If network licensing is enabled, add entitlements:

```xml
<!-- BULLsEYE.entitlements -->
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "...">
<plist version="1.0">
<dict>
    <key>com.apple.security.network.client</key>
    <true/>                                          <!-- License server HTTPS -->
    <key>com.apple.security.files.user-selected.read-write</key>
    <true/>                                          <!-- License file storage -->
</dict>
</plist>
```

---

## 8. Phase 5: Cross-Platform Deployment

### 8.1 Windows

| Aspect | Action |
|--------|--------|
| License Storage | Windows Registry (`HKCU\SOFTWARE\IQ\BULLsEYE\License`) |
| Network | WinHTTP (system library, no bundling needed) |
| Crypto | CryptoAPI (system library) |
| Build | Static linking (`/MT`) per ANMO Windows Laws |
| Testing | `Dependency Walker` to verify zero external DLL dependencies |

### 8.2 Linux

| Aspect | Action |
|--------|--------|
| License Storage | `~/.config/IQ/BULLsEYE/license.dat` |
| Network | libcurl (link statically or document dependency) |
| Crypto | OpenSSL (link statically) |
| Build | Static linking recommended |
| Testing | Test on Ubuntu 22.04 LTS + Fedora |

### 8.3 Platform Preprocessor Gates

```cpp
// In LicenseSSOT.h or a platform header:
#if defined(__APPLE__)
    #define BULLSEYE_LICENSE_STORAGE_KEYCHAIN 1
#elif defined(_WIN32)
    #define BULLSEYE_LICENSE_STORAGE_REGISTRY 1
#else
    #define BULLSEYE_LICENSE_STORAGE_FILE 1
#endif
```

---

## 9. Phase 6: Testing & Validation

### 9.1 New Test Cases (add to `tests/`)

| Test ID | Category | Description | Thread |
|---------|----------|-------------|--------|
| LIC-001 | Init | License engine initializes with LicenseSSOT config | UI |
| LIC-002 | Init | Graceful degradation on init failure → trial mode | UI |
| LIC-003 | Audio | `isLicensedAtomically()` returns within 1μs | Audio |
| LIC-004 | Audio | No allocation in `processBlock` license path | Audio |
| LIC-005 | Audio | Licensed: audio passes through unchanged | Audio |
| LIC-006 | Audio | Trial (mute=true): buffer cleared | Audio |
| LIC-007 | Audio | Trial (mute=false): audio passes through | Audio |
| LIC-008 | UI | `isLicensed()` matches `isLicensedAtomically()` | UI |
| LIC-009 | UI | Status message updates correctly | UI |
| LIC-010 | UI | Activation dialog flow works | UI |
| LIC-011 | State | License persists across DAW session reload | UI |
| LIC-012 | State | License persists across plugin reinstall | UI |
| LIC-013 | Platform | macOS Keychain read/write works | UI |
| LIC-014 | Platform | Windows Registry read/write works | UI |
| LIC-015 | Platform | Linux file-based storage works | UI |
| LIC-016 | Deploy | `otool -L` shows no absolute Homebrew paths | Build |
| LIC-017 | Deploy | `codesign --verify --deep --strict` passes | Build |
| LIC-018 | Deploy | Plugin loads on clean Mac (no Homebrew) | Manual |
| LIC-019 | Offline | Offline validation works for 30+ days | UI |
| LIC-020 | Grace | Grace period allows continued use for 7 days | UI |

### 9.2 DAW Testing Matrix

| DAW | macOS arm64 | macOS x86_64 | Windows | Linux |
|-----|:-----------:|:------------:|:-------:|:-----:|
| REAPER | ○ | ○ | ○ | ○ |
| Logic Pro | ○ | ○ | N/A | N/A |
| Ableton Live | ○ | ○ | ○ | N/A |
| Bitwig | ○ | ○ | ○ | ○ |

○ = Needs testing

### 9.3 Deployment Verification Script

```bash
#!/bin/bash
# verify_license_integration.sh

BUNDLE="build/BULLsEYE_artefacts/Release/VST3/BULLsEYE.vst3"
BINARY="$BUNDLE/Contents/MacOS/BULLsEYE"

echo "=== LAW 1: Bundle Integrity ==="
test -f "$BINARY" && echo "PASS: Binary exists" || echo "FAIL: Binary missing"
test -d "$BUNDLE/Contents" && echo "PASS: Contents/ exists" || echo "FAIL"

echo ""
echo "=== LAW 2: Dylib References ==="
otool -L "$BINARY" | grep -E "homebrew|/usr/local" && echo "FAIL: Absolute paths found" || echo "PASS: No absolute paths"

echo ""
echo "=== LAW 5: Codesigning ==="
codesign --verify --deep --strict "$BUNDLE" 2>&1 && echo "PASS" || echo "FAIL"

echo ""
echo "=== LAW 6: Architecture ==="
lipo -info "$BINARY"

echo ""
echo "=== LAW 9: Rpath ==="
otool -L "$BINARY" | grep "@rpath" && echo "PASS: @rpath references found"
```

---

## 10. Phase 7: Release & Documentation

### 10.1 Documentation Updates

| File | Change |
|------|--------|
| `README.md` | Add "Licensing" section with trial/activation info |
| `REFERENCE.md` | Add `LicenseSSOT.h` constants reference |
| `CHANGELOG.md` | Add v2.1.0 license integration entry |
| `RELEASE_NOTES.md` | Add license feature announcement |
| `LAW_BOOK.md` | Add Law 32: License Engine Thread Safety |
| `docs/README.md` | Update documentation index |

### 10.2 Release Tag Format

```
v2.1.0-macos-universal-licensed-feb2026
```

With metadata:
```
ENVIRONMENTS TESTED:
- Apple Silicon (M3): macOS 14 Sonoma - PASS
- Intel (x86_64): macOS 13 Ventura - PASS

CRITICAL ADDITIONS:
- License engine integration (Portable License Drop-In Kit)
- Trial mode: 14 days, full functionality
- Offline validation: 30+ days
- macOS Keychain secure storage

BUILD ARTIFACTS:
- VST3: BULLsEYE.vst3 (universal binary)
- AU: BULLsEYE.component (universal binary)
- License Engine: v1.0.0 (header-only)

ARCHITECTURE:
- Universal binary (x86_64 + arm64)
- OpenSSL 3.x bundled (if network license enabled)
- Codesigned: Developer ID Application

COMMIT: [hash]
DATE: 2026-02-XX
STATUS: PRODUCTION STABLE
```

---

## 11. Dependency Matrix

| Dependency | Required? | Platform | Purpose | ANMO Law |
|------------|-----------|----------|---------|----------|
| JUCE 7+ | ✅ Yes | All | Audio framework | N/A |
| C++17 | ✅ Yes | All | Language standard | N/A |
| `LICENSE_ENGINE.hpp` | ✅ Yes | All | License engine (header-only) | N/A |
| `std::atomic` | ✅ Yes | All | Audio-safe license checks | N/A |
| `std::mutex` | ✅ Yes | All | UI thread license ops | N/A |
| OpenSSL 3.x | ⚠️ Optional | macOS/Linux | HTTPS license validation | LAW 2, 3, 9 |
| WinHTTP | ⚠️ Optional | Windows | HTTPS license validation | N/A |
| Foundation.framework | ⚠️ Optional | macOS | Machine ID, Keychain | LAW 1, 5 |
| IOKit.framework | ⚠️ Optional | macOS | Hardware identification | N/A |
| Security.framework | ⚠️ Optional | macOS | Keychain storage | LAW 5, 8 |
| GoogleTest | ✅ Yes | All | Test framework | N/A |

---

## 12. Risk Assessment

| Risk | Likelihood | Impact | Mitigation |
|------|-----------|--------|------------|
| Audio thread stall from blocking license call | Low | **Critical** | Only use `isLicensedAtomically()` in `processBlock`. Code review enforced. |
| OpenSSL dylib missing on user Mac | Medium | High | Bundle in `Frameworks/` per LAW 2. Verify with `otool -L`. |
| Absolute Homebrew path in binary | Medium | High | CMake `install_name_tool` post-build. LAW 9 verification script. |
| Codesign breaks with license engine | Low | High | License engine is header-only, no anti-tamper. Test `codesign --deep`. |
| License file corruption | Low | Medium | Graceful degradation to trial. `getLastKnownGoodState()` fallback. |
| JSON parse exception in license response | Medium | Medium | All JSON parsing wrapped in try/catch per `LICENSE_DROP_IN_LAWS.md`. |
| Race condition: UI reads while audio writes | Low | Critical | Atomic API separates threading. No shared mutable state. |
| Plugin rejected by DAW due to unsigned bundle | Medium | High | LAW 5/8: Sign all builds. Notarize for production. |

---

## 13. Integration Checklist

### Pre-Integration

- [ ] Read `portable-license-drop-in/LICENSE_DROP_IN_LAWS.md` thoroughly
- [ ] Read `ANMO-OSX-Staging/laws/osx/MACOS_VST3_DEPLOYMENT_LAWS.md` (all 9 laws)
- [ ] Review `portable-license-drop-in/INTEGRATION_GUIDE.md` examples
- [ ] Verify existing 42 tests pass before any changes

### Phase 1: Foundation

- [ ] Create `Source/SSOT/LicenseSSOT.h`
- [ ] Update `CMakeLists.txt` include paths
- [ ] Update `CMakeLists.txt` source files
- [ ] Add OpenSSL CMake conditional (if network license)
- [ ] Verify build succeeds with no functional changes

### Phase 2: PluginProcessor

- [ ] Add `#include "core/LICENSE_ENGINE.hpp"` to `PluginProcessor.h`
- [ ] Add `LicenseEngine::LicenseEngine licenseEngine` member
- [ ] Add `initializeLicenseEngine()` call in constructor
- [ ] Add `isLicensedAtomically()` gate in `processBlock()`
- [ ] Verify audio thread safety (no blocking calls in audio path)
- [ ] Add license accessor methods for UI

### Phase 3: UI

- [ ] Create `LicenseStatusComponent.h/cpp`
- [ ] Add license status bar to `BULLsEYEEditor`
- [ ] Add license colors to `UISSOT.h`
- [ ] Implement activation dialog
- [ ] Timer-based status refresh at 1 Hz

### Phase 4: macOS Deployment

- [ ] Bundle structure verified (`ls -R *.vst3`)
- [ ] `otool -L` shows no absolute Homebrew paths
- [ ] `@rpath` references correct for OpenSSL (if enabled)
- [ ] Codesign passes: `codesign --verify --deep --strict`
- [ ] Entitlements file created (if network license)
- [ ] Clean Mac test: plugin loads without Homebrew

### Phase 5: Cross-Platform

- [ ] Windows build succeeds
- [ ] Windows license storage (Registry) verified
- [ ] Linux build succeeds (if applicable)
- [ ] Linux license storage (file) verified

### Phase 6: Testing

- [ ] All 20 new license test cases pass
- [ ] All 42 existing integration tests still pass
- [ ] DAW testing on REAPER (macOS)
- [ ] DAW testing on Logic Pro (macOS)
- [ ] Verification script passes

### Phase 7: Release

- [ ] CHANGELOG.md updated
- [ ] README.md updated
- [ ] LAW_BOOK.md updated (Law 32)
- [ ] Release tag created with metadata
- [ ] Documentation index updated

---

## 14. File-by-File Change Map

| File | Action | Description |
|------|--------|-------------|
| `CMakeLists.txt` | **MODIFY** | Add include path, source files, optional OpenSSL |
| `Source/SSOT/LicenseSSOT.h` | **CREATE** | License configuration constants |
| `Source/SSOT/UISSOT.h` | **MODIFY** | Add license UI colors/fonts |
| `Source/PluginProcessor.h` | **MODIFY** | Add license engine member, accessor methods |
| `Source/PluginProcessor.cpp` | **MODIFY** | Initialize engine, add `processBlock` gate |
| `Source/PluginEditor.h` | **MODIFY** | Add LicenseStatusComponent |
| `Source/PluginEditor.cpp` | **MODIFY** | Layout license status bar |
| `Source/Components/LicenseStatusComponent.h` | **CREATE** | License status UI component |
| `Source/Components/LicenseStatusComponent.cpp` | **CREATE** | License status implementation |
| `tests/License/TestLicenseIntegration.cpp` | **CREATE** | 20 new license test cases |
| `tests/CMakeLists.txt` | **MODIFY** | Add license test sources |
| `build.sh` | **MODIFY** | Add `BULLSEYE_ENABLE_NETWORK_LICENSE` option |
| `BULLsEYE.entitlements` | **CREATE** | Hardened runtime entitlements (macOS) |
| `README.md` | **MODIFY** | Add licensing section |
| `CHANGELOG.md` | **MODIFY** | Add v2.1.0 entry |
| `LAW_BOOK.md` | **MODIFY** | Add Law 32: License Thread Safety |
| `REFERENCE.md` | **MODIFY** | Add LicenseSSOT constants |
| `docs/LICENSE_ENGINE_INTEGRATION_PLAN.md` | **CREATE** | This document |

---

**End of Integration Plan**

*This document should be reviewed against the latest versions of both the BULLsEYE codebase and the Portable License Drop-In Kit before implementation begins.*