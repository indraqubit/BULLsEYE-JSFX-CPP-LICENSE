#pragma once

#include <cmath>
#include "../SSOT/ModelSSOT.h"
#include "../SSOT/DSPSSOT.h"
#include "../SSOT/ProcessorSSOT.h"

/**
 * DSP Core - TETRIS Compliant
 * 
 * Processes audio samples based on your algorithm.
 * TODO: Customize for your plugin
 * 
 * TETRIS Principles:
 * - T: Thread Separation (no UI access)
 * - E: Encapsulation (private state, validated setters)
 * - T: Trivially Copyable (static_assert)
 * - R: Reference Processing (template process())
 * - I: Internal Double (process in double)
 * - S: Smoothing (if needed)
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
    
    // TODO: Add your setters with validation
    
    /**
     * Set mode
     */
    void setMode(ModelSSOT::YourMode mode) noexcept
    {
        // Validate mode
        const int modeInt = static_cast<int>(mode);
        if (modeInt < 0 || modeInt >= ModelSSOT::MODE_COUNT)
            return;  // Invalid mode, ignore
        
        if (mode != currentMode)
        {
            currentMode = mode;
            resetRuntimeState();
            calc();
        }
    }
    
    /**
     * Set enabled state
     */
    void setEnabled(bool enabled) noexcept
    {
        if (enabled != isEnabled)
        {
            isEnabled = enabled;
            calc();
        }
    }
    
    /**
     * Set parameter value
     */
    void setParameter(float value) noexcept
    {
        // Clamp to valid range
        value = DSPSSOT::Helpers::clamp(value, 0.0f, 1.0f);
        
        if (std::abs(value - parameterValue) > 0.0001f)
        {
            parameterValue = value;
            calc();
        }
    }
    
    // ========================================================================
    // SETTINGS (TETRIS - no calc, just state)
    // ========================================================================
    
    void setThreshold(double threshold) noexcept
    {
        threshold = DSPSSOT::Helpers::clamp(threshold, 
            DSPSSOT::Algorithm::MIN_THRESHOLD, 
            DSPSSOT::Algorithm::MAX_THRESHOLD);
        this->threshold = threshold;
    }
    
    void setRatio(double ratio) noexcept
    {
        ratio = DSPSSOT::Helpers::clamp(ratio,
            DSPSSOT::Algorithm::MIN_RATIO,
            DSPSSOT::Algorithm::MAX_RATIO);
        this->ratio = ratio;
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
    
    /**
     * Process sample in place
     * TODO: Implement your algorithm
     */
    template<typename SampleType>
    void process(SampleType& sample) noexcept
    {
        // Process internally in double (TETRIS Internal Double)
        double sampleDouble = static_cast<double>(sample);
        
        // Your processing here
        // if (isEnabled)
        // {
        //     // Apply your algorithm
        //     sampleDouble = yourAlgorithm(sampleDouble);
        // }
        // else
        // {
        //     // Bypass
        // }
        
        // Denormal flush (TETRIS - safe for denormals)
        if (std::abs(sampleDouble) < ProcessorSSOT::Processing::DENORMAL_THRESHOLD)
            sampleDouble = 0.0;
        
        sample = static_cast<SampleType>(sampleDouble);
    }
    
    // ========================================================================
    // GETTERS
    // ========================================================================
    
    bool getEnabled() const noexcept { return isEnabled; }
    ModelSSOT::YourMode getMode() const noexcept { return currentMode; }
    double getThreshold() const noexcept { return threshold; }
    double getRatio() const noexcept { return ratio; }
    
private:
    // ========================================================================
    // PRIVATE STATE (TETRIS - trivially copyable)
    // ========================================================================
    
    bool isEnabled{true};
    ModelSSOT::YourMode currentMode{ModelSSOT::YourMode::ModeA};
    float parameterValue{0.0f};
    
    // Your algorithm state here
    double threshold{0.0};
    double ratio{4.0};
    
    // ========================================================================
    // PRIVATE METHODS
    // ========================================================================
    
    /**
     * Calculate derived state (TETRIS Encapsulation)
     */
    void calc() noexcept
    {
        // TODO: Implement your calculation
        // effectiveValue = calculateEffectiveValue();
    }
    
    /**
     * Reset runtime state (not parameters)
     */
    void resetRuntimeState() noexcept
    {
        // TODO: Reset any runtime state (counters, filters, etc.)
        // Don't reset parameters (those are persistent)
    }
    
    /**
     * Your algorithm implementation
     */
    double yourAlgorithm(double sample) noexcept
    {
        // TODO: Implement your algorithm
        return sample;  // Pass through for now
    }
    
    // ========================================================================
    // TETRIS COMPLIANCE
    // ========================================================================
    
    static_assert(std::is_trivially_copyable_v<YourProcessor>,
        "YourProcessor must be trivially copyable per TETRIS");
};

// ============================================================================
// TETRIS COMPLIANCE CHECKLIST
// ============================================================================
/*
T - Thread Separation
   ✅ No UI dependencies
   ✅ No std::mutex or std::lock
   ✅ Only audio thread operations

E - Encapsulation
   ✅ Private state
   ✅ Validated setters
   ✅ calc() pattern

T - Trivially Copyable
   ✅ static_assert(std::is_trivially_copyable_v<YourProcessor>)
   ✅ Only trivial types (bool, int, float, double, enums)

R - Reference Processing
   ✅ template<typename SampleType> void process(SampleType& sample)
   ✅ In-place processing

I - Internal Double
   ✅ Process in double precision internally
   ✅ Convert back to sample type

S - Smoothing (if applicable)
   ✅ Linear interpolation for smooth transitions
   ✅ Time-based smoothing constants
*/
