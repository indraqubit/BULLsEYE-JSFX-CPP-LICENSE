# License Drop-In Kit Laws & Best Practices

**Updated:** 2026-02-01 (Added JSON parsing patterns)  
**Related:** [Law 10: Third-Party Library Handling](../../laws/10_THIRD_PARTY_LIBRARY_HANDLING.md) | [Blindspots Analysis](../../osx/BLINDSPOT_FIX_PLAN.md)

---

## Overview

This document outlines the laws, rules, and best practices for implementing the Portable License Drop-In Kit in audio plugins and commercial software. These laws are designed to ensure consistent behavior, user privacy, and system stability across all implementations.

## IMPORTANT: Thread Safety Clarification

This license engine provides **two distinct API sets** for different threading contexts:

### Audio Thread API (Lock-Free, Non-Blocking)
These methods are **SAFE to call from audio thread** (`processBlock`):
- `isLicensedAtomically()` - Quick license check
- `getDaysRemainingAtomically()` - Days remaining
- `isInGracePeriodAtomically()` - Grace period check
- `isFeatureEnabledAtomically(index)` - Feature check

### UI/Background Thread API (May Block)
These methods **MUST be called from UI or background thread**:
- `initialize()` - One-time initialization
- `activateLicense()` - License activation
- `refreshLicenseStatus()` - Status refresh
- `getLicenseStatusMessage()` - Human-readable status
- `isLicensed()` / `isTrial()` / `isExpired()` - Full status checks

---

## Core Laws

### 1. Threading Law

**1.1 Audio Thread Safety**
- Use `isLicensedAtomically()` and related methods in `processBlock()`
- NEVER call blocking methods from audio thread (initialize, activateLicense, etc.)
- License state changes MUST use atomic operations for audio-thread reads
- NEVER perform file I/O, network operations, or heavy computation during audio processing

**1.2 Thread Contention**
- Background/UI thread operations use mutex protection (acceptable for non-audio threads)
- Audio thread uses lock-free atomic reads only
- Cache license status via `LicenseSnapshot` to avoid repeated validation during audio processing
- Use `try-or-skip` pattern (`tryActivateLicense`, `tryRefreshStatus`) for non-blocking operations

**1.3 Correct API Usage**
```cpp
// AUDIO THREAD - Use atomic API
void processBlock(AudioBuffer<float>& buffer, MidiBuffer&) override {
    if (!licenseEngine.isLicensedAtomically()) {
        buffer.clear();  // Silence for unlicensed
        return;
    }
    // Normal audio processing...
}

// UI THREAD - Use full API
void onCheckLicenseClicked() {
    if (licenseEngine.isLicensed()) {
        showValidLicenseUI();
    } else {
        showActivationDialog();
    }
}
```

### 2. Time & Lifecycle Law

**2.1 Initialization**
- Initialize the license engine ONCE during plugin construction
- Call `initialize()` from UI thread or plugin constructor
- NEVER reinitialize the license engine after audio processing begins
- Provide clear error handling for initialization failures

**2.2 License Validation Timing**
- Perform full license validation at initialization and periodically in background thread
- Use `refreshLicenseStatus()` for background validation (supports try-or-skip)
- Schedule full validations at appropriate intervals (daily for subscriptions, weekly for perpetual)
- Use atomic API (`isLicensedAtomically()`) for audio thread checks

**2.3 Grace Periods**
- Implement reasonable grace periods for subscription licenses (minimum 7 days, configurable)
- Allow continued operation during grace periods with appropriate notification
- Check grace period with `isInGracePeriodAtomically()` from audio thread
- Honor existing licenses during grace periods even if renewal fails

### 3. User Experience Law

**3.1 Trial Experience**
- Trial periods MUST be at least 14 days for professional audio software
- Trial limitations MUST be clearly communicated to users
- Feature limitations during trial MUST be consistent and predictable

**3.2 Licensing Feedback**
- Provide clear, actionable feedback about license status in the UI
- Use `getLicenseStatusMessage()` for human-readable status
- Display remaining trial/subscription days prominently
- Show clear instructions for license activation or renewal

**3.3 Offline Usage**
- Support offline license validation for at least 30 days
- Provide clear guidance for offline activation processes
- Honor previously validated licenses when network is unavailable
- Use `getLastKnownGoodState()` for drop-data fallback

### 4. Data Privacy & Security Law

**4.1 Machine Identification**
- Use hashed or obfuscated machine identifiers for privacy
- Machine ID is hashed using FNV-1a hash (pre-allocation, no heap in audio path)
- NEVER transmit raw hardware identifiers or personal data
- Allow users to reset their machine identification when necessary

**4.2 Data Minimization**
- Collect only the minimum data required for license validation
- Store license data securely on the local machine
- Implement proper encryption for license files (placeholder in this implementation)
- Use fixed-size storage arrays instead of vectors for audio-safe data

**4.3 User Consent**
- Obtain explicit consent for any data transmitted beyond license validation
- Provide clear privacy policies explaining data usage
- Allow users to review and delete their license data

### 5. Compatibility Law

**5.1 Framework Support**
- Ensure compatibility with major audio plugin frameworks (VST3, AU, AAX)
- Maintain consistent license behavior across different platforms
- Test license engine functionality on all supported operating systems

**5.2 Version Compatibility**
- Honor existing licenses when software is updated
- Provide migration paths for license format changes
- Maintain backward compatibility with older license files where possible

**5.3 System Integration**
- Respect system-level energy saving and power management
- Minimize resource usage during background license validation
- Avoid interfering with other software or system operations

---

## Implementation Guidelines

### License Check Integration

```cpp
// GOOD: Atomic license check in audio processing
void processBlock(AudioBuffer<float>& buffer, MidiBuffer&) override {
    if (!licenseEngine.isLicensedAtomically()) {
        buffer.clear(); // Silence output for unlicensed use
        return;
    }
    
    // Normal audio processing
    // ...
}

// GOOD: Drop-data fallback using last known good state
void processBlock(AudioBuffer<float>& buffer, MidiBuffer&) override {
    if (licenseEngine.isLicensedAtomically()) {
        // Normal processing
    } else {
        // Check if we have a last known good state
        LicenseInfo lastState;
        if (licenseEngine.getLastKnownGoodState(lastState)) {
            // Use degraded mode with last known good
            applyDegradedProcessing(buffer);
        } else {
            buffer.clear();
        }
    }
}

// BAD: Full license validation in audio processing
void processBlock(AudioBuffer<float>& buffer, MidiBuffer&) override {
    if (!licenseEngine.activateLicense(key)) { // BLOCKING!
        buffer.clear();
        return;
    }
    
    // Normal audio processing
    // ...
}
```

### UI Integration

```cpp
// GOOD: Background validation with try-or-skip
void timerCallback() {
    auto result = licenseEngine.tryRefreshStatus();
    if (result == LicenseResult::Success) {
        updateLicenseUI();
    } else if (result == LicenseResult::WouldBlock) {
        // Skip this refresh, use cached state
        // Audio continues with last known good
    }
}

// GOOD: Non-blocking activation attempt
void onActivateButtonClicked() {
    auto result = licenseEngine.tryActivateLicense(licenseKey);
    if (result == LicenseResult::Success) {
        showSuccessMessage();
    } else if (result == LicenseResult::WouldBlock) {
        showPleaseWaitMessage();  // Already busy, try again later
    } else {
        showErrorMessage();
    }
}

// GOOD: Blocking activation from UI thread (acceptable)
void onActivateButtonClicked() {
    if (licenseEngine.activateLicense(licenseKey)) {  // Blocking, but OK in UI thread
        showSuccessMessage();
    } else {
        showErrorMessage();
    }
}
```

### Error Handling

### JSON Parsing Patterns (2026-02-01 Update)

**Critical:** All JSON parsing MUST be wrapped in try/catch. This applies to:
- JWT payload parsing
- License response parsing
- Network callback JSON handling

```cpp
// GOOD: JSON::parse with try/catch wrapper
juce::var parseLicenseJson(const juce::String& jsonString, juce::String* errorReason = nullptr) {
    try {
        auto parsed = juce::JSON::parse(jsonString);
        if (!parsed.isObject()) {
            if (errorReason) *errorReason = "JSON root must be an object";
            return juce::var();
        }
        return parsed;
    } catch (const std::exception& e) {
        if (errorReason) *errorReason = juce::String("JSON parse exception: ") + e.what();
        return juce::var();
    }
}

// GOOD: JWT payload parsing with failure reason
bool parseJwtPayload(const juce::String& payloadB64, JWTPayload& out, juce::String* failureReason) {
    auto payloadJson = base64UrlDecode(payloadB64);
    if (payloadJson.isEmpty()) {
        if (failureReason) *failureReason = "Empty JWT payload";
        return false;
    }
    
    juce::var parsed;
    try {
        parsed = juce::JSON::parse(payloadJson);
    } catch (const std::exception& e) {
        if (failureReason) *failureReason = juce::String("JSON parse failed: ") + e.what();
        return false;
    }
    
    if (!parsed.isObject()) {
        if (failureReason) *failureReason = "JWT payload is not a JSON object";
        return false;
    }
    
    // ... extract claims safely
    return true;
}

// BAD: JSON::parse without try/catch (CRITICAL BLINDSPOT)
auto parsed = juce::JSON::parse(jsonString);  // May throw!
if (!parsed.isObject()) return false;
```

**Related Blindspots:**
- CRIT-01: JWTVerifier JSON parse (osx/BLINDSPOT_FIX_PLAN.md)
- CRIT-02: LicenseAPIClient JSON parsing (osx/BLINDSPOT_FIX_PLAN.md)

**See Also:** [Law 10: Third-Party Library Handling](../../laws/10_THIRD_PARTY_LIBRARY_HANDLING.md) - LAW 1: Exception Containment

---


```cpp
// GOOD: Graceful degradation for license failures
void initializePlugin() {
    try {
        LicenseConfig config = getLicenseConfig();
        if (!licenseEngine.initialize(config)) {
            // Fall back to trial mode
            showLicenseActivationDialog();
        }
    } catch (const std::exception& e) {
        // Log error and continue with trial mode
        logLicenseError(e.what());
        enableTrialMode();
    }
}

// GOOD: Use last known good state for drop-data fallback
void processBlock(AudioBuffer<float>& buffer, MidiBuffer&) override {
    if (!licenseEngine.isLicensedAtomically()) {
        LicenseInfo lastState;
        if (licenseEngine.getLastKnownGoodState(lastState)) {
            // Graceful degradation
            applyDegradedMode(buffer, lastState);
        } else {
            buffer.clear();
        }
        return;
    }
    
    // Normal processing...
}
```

---

## Compliance Checklist

### Before Release

- [ ] Audio thread uses atomic API (`isLicensedAtomically()`)
- [ ] UI/background thread uses appropriate blocking API
- [ ] No blocking operations in `processBlock()`
- [ ] Grace period implemented and tested for subscriptions
- [ ] Clear user feedback for license status
- [ ] Offline license support implemented (30+ days)
- [ ] Privacy policy compliance verified
- [ ] Cross-platform compatibility verified
- [ ] Memory leaks in license engine fixed
- [ ] Proper error handling implemented
- [ ] Documentation for developers updated

### During Development

- [ ] License status cached in `LicenseSnapshot` for audio thread use
- [ ] Background validation scheduled appropriately
- [ ] Machine identification properly hashed (FNV-1a)
- [ ] License data uses fixed-size storage (no vectors in hot path)
- [ ] User consent mechanisms in place
- [ ] Version compatibility tested
- [ ] Resource usage monitored
- [ ] Try-or-skip pattern used for non-blocking operations

### After Release

- [ ] Monitor license validation success rates
- [ ] Collect user feedback on license experience
- [ ] Track support issues related to licensing
- [ ] Update license engine as needed
- [ ] Maintain compatibility with existing licenses

---

## Enforcement

These laws are mandatory for all implementations of the License Drop-In Kit. Violations may result in:

1. Audio dropouts or crashes in host applications (if audio thread safety violated)
2. Inconsistent user experience across products
3. Privacy and security vulnerabilities
4. Rejection from plugin distributors and marketplaces
5. Loss of user trust and reputation damage

---

## Resources

For more information about specific implementation details, refer to:

- `core/LICENSE_ENGINE.hpp` - Core license engine implementation with audio-safe API
- `docs/audio-safety/AUDIO_THREAD_SAFETY_LAW.md` - Audio thread safety standards
- `examples/` directory - Reference implementations
- Plugin framework documentation for integration guidelines

---

## Quick Reference: Audio-Safe API

| Method | Thread | Blocking | Use Case |
|--------|--------|----------|----------|
| `isLicensedAtomically()` | Audio | No | Quick license check in processBlock |
| `getDaysRemainingAtomically()` | Audio | No | UI display of remaining days |
| `isInGracePeriodAtomically()` | Audio | No | Grace period check |
| `isFeatureEnabledAtomically()` | Audio | No | Feature gate in processBlock |
| `getLastKnownGoodState()` | Audio | No | Drop-data fallback |
| `isLicensed()` | UI | Yes | Full status check |
| `activateLicense()` | UI | Yes | License activation |
| `tryActivateLicense()` | UI | No* | Non-blocking activation |
| `tryRefreshStatus()` | UI | No* | Non-blocking refresh |

*Returns `WouldBlock` if mutex is busy, allowing skip

---

These laws are maintained to ensure a consistent, secure, and user-friendly licensing experience across all products using this kit.
