# DSP Template

Use this template when creating new DSP processor classes.

## TETRIS Principles Checklist

```
T - Thread Separation
   [ ] No UI dependencies
   [ ] No std::mutex or std::lock
   [ ] Only audio thread operations

E - Encapsulation
   [ ] Private state
   [ ] Validated setters
   [ ] calc() pattern

T - Trivially Copyable
   [ ] static_assert(std::is_trivially_copyable_v<YourProcessor>)
   [ ] Only trivial types (bool, int, float, double, enums)

R - Reference Processing
   [ ] template<typename SampleType> void process(SampleType& sample)
   [ ] In-place processing

I - Internal Double
   [ ] Process in double precision internally
   [ ] Convert back to sample type

S - Smoothing (if applicable)
   [ ] Linear interpolation for smooth transitions
   [ ] Time-based smoothing constants
```

## Template

```cpp
#pragma once

#include <cmath>
#include "../SSOT/ModelSSOT.h"
#include "../SSOT/DSPSSOT.h"
#include "../SSOT/ProcessorSSOT.h"

/**
 * YourProcessor - TETRIS Compliant DSP Core
 * 
 * Processes audio samples based on your algorithm.
 * TODO: Customize for your plugin
 */
class YourProcessor
{
public:
    // ========================================================================
    // CONSTRUCTOR
    // ========================================================================
    
    YourProcessor() = default;
    
    // ========================================================================
    // SETTERS (TETRIS Encapsulation)
    // ========================================================================
    
    void setMode(ModelSSOT::YourMode mode) noexcept
    {
        // Validate mode
        const int modeInt = static_cast<int>(mode);
        if (modeInt < 0 || modeInt >= ModelSSOT::MODE_COUNT)
            return;
        
        if (mode != currentMode)
        {
            currentMode = mode;
            resetRuntimeState();
            calc();
        }
    }
    
    void setEnabled(bool enabled) noexcept
    {
        if (enabled != isEnabled)
        {
            isEnabled = enabled;
            calc();
        }
    }
    
    // ========================================================================
    // RESET (Runtime state only, not parameters)
    // ========================================================================
    
    void reset() noexcept
    {
        resetRuntimeState();
    }
    
    // ========================================================================
    // PROCESSING (TETRIS Reference Processing)
    // ========================================================================
    
    template<typename SampleType>
    void process(SampleType& sample) noexcept
    {
        // Process in double (TETRIS Internal Double)
        double sampleDouble = static_cast<double>(sample);
        
        // Your processing here
        if (isEnabled)
        {
            sampleDouble = yourAlgorithm(sampleDouble);
        }
        
        // Denormal flush
        if (std::abs(sampleDouble) < ProcessorSSOT::Processing::DENORMAL_THRESHOLD)
            sampleDouble = 0.0;
        
        sample = static_cast<SampleType>(sampleDouble);
    }
    
    // ========================================================================
    // GETTERS
    // ========================================================================
    
    bool getEnabled() const noexcept { return isEnabled; }
    ModelSSOT::YourMode getMode() const noexcept { return currentMode; }
    
private:
    // ========================================================================
    // PRIVATE STATE (TETRIS - trivially copyable)
    // ========================================================================
    
    bool isEnabled{true};
    ModelSSOT::YourMode currentMode{ModelSSOT::YourMode::ModeA};
    
    // Your state here
    
    // ========================================================================
    // PRIVATE METHODS
    // ========================================================================
    
    void calc() noexcept
    {
        // Calculate derived state
    }
    
    void resetRuntimeState() noexcept
    {
        // Reset runtime state (counters, filters, etc.)
        // Don't reset parameters
    }
    
    double yourAlgorithm(double sample) noexcept
    {
        // Your algorithm implementation
        return sample;
    }
    
    // ========================================================================
    // TETRIS COMPLIANCE
    // ========================================================================
    
    static_assert(std::is_trivially_copyable_v<YourProcessor>,
        "YourProcessor must be trivially copyable per TETRIS");
};
```

## Usage

1. Copy the template to your DSP file
2. Replace `YourProcessor` with your class name
3. Replace `YourMode` with your mode enum
4. Implement your algorithm in `yourAlgorithm()`
5. Add validation and state management
6. Test for thread safety
