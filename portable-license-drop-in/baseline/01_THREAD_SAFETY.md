# Audio Thread Safety Law (Drop-In Kit Specific)

## Overview

These laws are extracted from the parent AnalogMorphV333 project laws and adapted specifically for the portable license drop-in kit. They define absolute requirements for any license system interacting with real-time audio processing within the context of a drop-in license implementation.

## Law 1 — Atomic State Reading (Zero-Tolerance Violation)

### Audio Thread Must: (Non-Negotiable)

- **Read license state via single atomic operation only**
- **Use lock-free atomic primitives** (`std::atomic`, `juce::Atomic`)
- **Use proper memory ordering** (`memory_order_acquire/release`)
- **Cache decision locally** for the duration of `processBlock()`
- **Never read from file system or network** during audio processing

### Forbidden Operations (Zero Tolerance)

- ❌ Any form of locking (`std::mutex`, `juce::CriticalSection`)
- ❌ Multiple atomic reads without caching
- ❌ Complex decision logic in audio thread
- ❌ Atomic operations on complex data types
- ❌ Memory allocations of any kind in audio thread

### Implementation Requirement

The drop-in license engine MUST provide a single atomic read operation:

```cpp
// ✅ CORRECT: Single atomic read, cached decision
void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    // Read license state ONCE at beginning of processBlock
    const auto isLicensed = licenseManager->isLicensed();
    
    // Use cached decision throughout entire processBlock
    if (!isLicensed) {
        buffer.clear();
        return;
    }
    
    // Normal audio processing with cached decision
    // ... your DSP code here
}

// ❌ FORBIDDEN: Multiple atomic reads
void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    // First read
    if (!licenseManager->isLicensed()) {
        buffer.clear();
        return;
    }
    
    // ... some processing ...
    
    // Second read - could be different from first!
    if (!licenseManager->isLicensed()) {
        // Inconsistent state!
        applyMutedProcessing(buffer);
    }
}
```

## Law 2 — Memory Allocation Ban (Absolute Prohibition)

### Audio Thread Must Not: (Zero Tolerance)

- **Allocate heap memory** (`new`, `malloc`, `std::vector::push_back`, `juce::String`)
- **Use containers that may allocate** (`std::map`, `std::vector`)
- **Perform implicit allocations** (`juce::String::operator+`, lambda captures)
- **Call virtual functions** that might allocate

### Allowed Operations (Only These)

- **Stack-allocated primitive types**
- **Fixed-size arrays** with known compile-time bounds
- **Pre-allocated buffers** from initialization phase
- **Read-only access** to pre-initialized data

### Implementation Requirement

The drop-in license engine MUST provide zero-allocation license checks:

```cpp
// ✅ CORRECT: Pre-allocated buffer for output restrictions
class AudioGate {
private:
    float fadeBuffer[8192]; // Pre-allocated for maximum buffer size
    
public:
    void processAudio(juce::AudioBuffer<float>& buffer, bool isLicensed) noexcept
    {
        if (!isLicensed) {
            // Use pre-allocated buffer, no allocation
            for (int ch = 0; ch < buffer.getNumChannels(); ++ch) {
                std::fill_n(fadeBuffer, buffer.getNumSamples(), 0.0f);
                buffer.copyFrom(ch, 0, fadeBuffer, 0, buffer.getNumSamples());
            }
            return;
        }
        
        // Normal processing when licensed
        // ... your DSP code here
    }
};
```

## Law 3 — Deterministic Audio Output (Absolute Requirement)

### Audio Output Must: (Non-Negotiable)

- **Be deterministic** for given input and license state
- **Same input → same output** regardless of timing
- **Have zero audio artifacts** during license state changes
- **Synchronize state changes** to audio buffer boundaries

### Implementation Requirement

The drop-in license engine MUST ensure deterministic behavior:

```cpp
// ✅ CORRECT: State change synchronized to buffer boundaries
class LicenseStateManager {
private:
    std::atomic<LicenseState> currentState{LicenseState::Unlicensed};
    std::atomic<bool> stateChangePending{false};
    bool lastState = false;
    
public:
    void processBlock(juce::AudioBuffer<float>& buffer, bool isLicensed) noexcept
    {
        // Check for state change at buffer boundary
        const auto newState = isLicensed;
        if (newState != lastState) {
            // Handle state change between buffers
            handleLicenseStateChange(newState);
            lastState = newState;
        }
        
        // Use current state for entire buffer
        if (!isLicensed) {
            buffer.clear();
            return;
        }
        
        // Normal processing with consistent state
        // ... your DSP code here
    }
    
private:
    void handleLicenseStateChange(bool newState) {
        // Handle state change in background thread, not audio thread
        juce::MessageManager::callAsync([this, newState]() {
            // Update UI with new state
        });
    }
};
```

## Law 4 — Thread Communication Rules (Strict Requirements)

### Cross-Thread Communication Must: (Non-Negotiable)

- **Use atomic primitives** for simple state flags
- **Marshal complex updates** to UI thread via `MessageManager`
- **Avoid thread contention** through proper synchronization
- **Never block audio thread** waiting for other threads

### Implementation Requirement

The drop-in license engine MUST follow strict thread communication rules:

```cpp
// ✅ CORRECT: Thread-safe state update with UI marshaling
class LicenseManager {
private:
    std::atomic<LicenseState> licenseState{LicenseState::Unlicensed};
    std::atomic<bool> isUpdating{false};
    
public:
    void updateLicenseState(LicenseState newState) {
        // Check for in-progress update
        bool expected = false;
        if (!isUpdating.compare_exchange_strong(expected, true)) {
            return; // Update already in progress
        }
        
        try {
            // Update atomic state first
            licenseState.store(newState, std::memory_order_release);
            
            // Marshal UI update to message thread
            juce::MessageManager::callAsync([this, newState]() {
                if (statusCallback) {
                    const bool isLicensed = (newState == LicenseState::Licensed);
                    statusCallback(isLicensed, getLicenseMessage(newState));
                }
            });
            
            // Log update in background thread (non-blocking)
            logLicenseStateChange(newState);
            
        } catch (...) {
            // Handle errors without affecting state
        }
        
        // Clear update flag
        isUpdating.store(false);
    }
    
    LicenseState getLicenseState() const noexcept {
        // Single atomic read with proper memory ordering
        return licenseState.load(std::memory_order_acquire);
    }
};
```

## Law 5 — License State Change Timing (Synchronization)

### State Changes Must: (Non-Negotiable)

- **Occur at audio buffer boundaries** only
- **Be atomic and instantaneous** from audio perspective
- **Not introduce artifacts** during transition
- **Be consistent** across all audio channels

### Implementation Requirement

The drop-in license engine MUST synchronize license state changes to audio buffer boundaries:

```cpp
// ✅ CORRECT: State change queue for buffer boundaries
class AudioProcessor {
private:
    std::atomic<bool> shouldProcessAudio{false};
    bool processingState = false;
    
    // State change queue for thread-safe updates
    juce::AbstractFifo stateChangeQueue{16};
    struct StateChange { bool isLicensed; };
    
public:
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
    {
        // Check for pending state changes at buffer boundary
        checkStateChanges();
        
        // Use current state for entire buffer
        const auto isLicensed = shouldProcessAudio.load(std::memory_order_acquire);
        
        if (!isLicensed) {
            buffer.clear();
            return;
        }
        
        // Normal processing with consistent state
        // ... your DSP code here
    }
    
private:
    void checkStateChanges() {
        int start1, size1, start2, size2;
        stateChangeQueue.prepareToWrite(1, start1, size1, start2, size2);
        
        if (size1 > 0) {
            const auto change = stateChangeQueue.read(start1)[0];
            processingState = change.isLicensed;
            shouldProcessAudio.store(change.isLicensed, std::memory_order_release);
            stateChangeQueue.finishedRead(size1);
        }
    }
};
```

## Law 6 — Audio Glitch Prevention (Critical Requirement)

### Audio Path Must: (Non-Negotiable)

- **Never glitch** during license state transitions
- **Maintain constant output latency** regardless of license state
- **Handle sample-accurate timing** without introduction of artifacts
- **Preserve phase continuity** when possible

### Implementation Requirement

The drop-in license engine MUST provide glitch-free license state transitions:

```cpp
// ✅ CORRECT: Smooth transitions to prevent artifacts
class GlitchFreeLicenseGate {
private:
    struct Smoother {
        float currentValue = 0.0f;
        float targetValue = 0.0f;
        float smoothingCoeff = 0.999f; // Very slow smoothing for license changes
        
        void setTarget(float target) noexcept {
            targetValue = target;
        }
        
        float getNextValue() noexcept {
            currentValue = targetValue + (currentValue - targetValue) * smoothingCoeff;
            return currentValue;
        }
    };
    
    Smoother gainSmoother;
    
public:
    void processBlock(juce::AudioBuffer<float>& buffer, bool isLicensed)
    {
        // Set target gain based on license state
        const float targetGain = isLicensed ? 1.0f : 0.0f;
        gainSmoother.setTarget(targetGain);
        
        // Apply smooth gain transition to avoid glitches
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch) {
            auto* channelData = buffer.getWritePointer(ch);
            
            for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
                const float gain = gainSmoother.getNextValue();
                channelData[sample] *= gain;
            }
        }
    }
};
```

## Law 7 — Resource Consumption Limits (Strict Constraints)

### Audio Thread Must: (Non-Negotiable)

- **Use constant O(1) memory** regardless of license complexity
- **Execute in constant time** independent of license state
- **Have bounded CPU usage** with strict limits
- **Never perform recursive operations** in audio path

### Implementation Requirement

The drop-in license engine MUST have bounded resource consumption:

```cpp
// ✅ CORRECT: Bounded resource usage
class LicenseAwareProcessor {
private:
    static constexpr int MAX_LICENSE_FEATURES = 16;
    static constexpr int MAX_PROCESSING_STEPS = 32;
    
    // Fixed-size pre-allocated arrays
    std::array<bool, MAX_LICENSE_FEATURES> enabledFeatures;
    std::array<void*, MAX_PROCESSING_STEPS> processingChain;
    
public:
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
    {
        // Fixed number of processing steps - O(1) complexity
        const auto numSteps = std::min(MAX_PROCESSING_STEPS, getActiveStepCount());
        
        for (int step = 0; step < numSteps; ++step) {
            if (isStepEnabled(step)) {
                processStep(buffer, midiMessages, step);
            }
        }
    }
    
private:
    bool isStepEnabled(int stepIndex) const noexcept {
        // O(1) check - bounded by MAX_LICENSE_FEATURES
        if (stepIndex >= MAX_PROCESSING_STEPS) return false;
        
        // Simple boolean array lookup
        return enabledFeatures[stepIndex % MAX_LICENSE_FEATURES];
    }
    
    int getActiveStepCount() const noexcept {
        // Return pre-calculated count - O(1)
        return activeStepCount;
    }
};
```

## Drop-In Specific Considerations

### License Validation Timing

- License validation MUST occur outside audio processing thread
- License checks MUST NOT perform file I/O or network operations
- Background validation MUST NOT affect audio thread

### UI Integration Rules

- License UI updates MUST be marshaled to message thread
- License dialogs MUST NOT block audio thread
- License status MUST be displayed clearly in UI

### State Persistence

- License state MUST be persisted atomically
- License file corruption MUST be handled gracefully

## Implementation Guidelines

1. **Single atomic read per processBlock()** - All license state reads must happen once
2. **Pre-allocated buffers** - All memory for audio processing must be pre-allocated
3. **State change queues** - Use lock-free FIFOs for state changes
4. **Smoothing transitions** - Use parameter smoothing for volume/gain changes
5. **Bounded operations** - All license operations must be O(1) complexity

## Compliance Checklist

Before releasing any audio processing code with drop-in license integration:

- [ ] Law 1: Single atomic read per processBlock()?
- [ ] Law 2: Zero heap allocations in audio thread?
- [ ] Law 3: Deterministic output for given input?
- [ ] Law 4: Thread communication uses atomics/MessageManager?
- [ ] Law 5: State changes synchronized to buffer boundaries?
- [ ] Law 6: Glitch-free transitions implemented?
- [ ] Law 7: Bounded resource consumption (O(1))?

Any violation of these laws will result in audio glitches, crashes, and unpredictable plugin behavior.