#pragma once

/**
 * DSP Layer Single Source of Truth
 * 
 * Contains all DSP/algorithm-related constants:
 * - Algorithm parameters
 * - Filter settings
 * - Timer durations
 * - Processing thresholds
 * 
 * Usage: Include in DSP implementation files only.
 */
namespace DSPSSOT
{
    // ========================================================================
    // ALGORITHM CONSTANTS
    // ========================================================================
    
    namespace Algorithm
    {
        // TODO: Customize with your algorithm constants
        
        // Gain values (in dB or linear, depending on use)
        constexpr double MIN_GAIN_DB = -60.0;
        constexpr double MAX_GAIN_DB = 6.0;
        constexpr double DEFAULT_GAIN_DB = 0.0;
        
        // Linear gain conversion
        constexpr double DB_TO_LINEAR(double db) { return std::pow(10.0, db / 20.0); }
        constexpr double LINEAR_TO_DB(double linear) { return 20.0 * std::log10(linear); }
        
        // Threshold values
        constexpr double MIN_THRESHOLD = -60.0;
        constexpr double MAX_THRESHOLD = 0.0;
        
        // Ratio values
        constexpr double MIN_RATIO = 1.0;
        constexpr double MAX_RATIO = 20.0;
        constexpr double DEFAULT_RATIO = 4.0;
        
        // Attack/Release times (in milliseconds)
        constexpr double MIN_ATTACK_MS = 0.1;
        constexpr double MAX_ATTACK_MS = 100.0;
        constexpr double DEFAULT_ATTACK_MS = 10.0;
        
        constexpr double MIN_RELEASE_MS = 10.0;
        constexpr double MAX_RELEASE_MS = 1000.0;
        constexpr double DEFAULT_RELEASE_MS = 100.0;
    }
    
    // ========================================================================
    // TIMER CONSTANTS (for time-based features like Momentary)
    // ========================================================================
    
    namespace Timer
    {
        // Momentary mode duration (in seconds)
        constexpr double MOMENTARY_DURATION_SECONDS = 0.5;
        
        // Hold mode timeout (in seconds, if needed)
        constexpr double HOLD_TIMEOUT_SECONDS = 5.0;
        
        // Double-click interval (in seconds)
        constexpr double DOUBLE_CLICK_INTERVAL = 0.3;
    }
    
    // ========================================================================
    // FILTER CONSTANTS
    // ========================================================================
    
    namespace Filter
    {
        // Cutoff frequency range (in Hz)
        constexpr double MIN_CUTOFF = 20.0;
        constexpr double MAX_CUTOFF = 20000.0;
        constexpr double DEFAULT_CUTOFF = 1000.0;
        
        // Q factor range
        constexpr double MIN_Q = 0.1;
        constexpr double MAX_Q = 20.0;
        constexpr double DEFAULT_Q = 0.707;
        
        // Filter slope (in dB/octave)
        constexpr int SLOPE_12 = 12;
        constexpr int SLOPE_24 = 24;
        constexpr int SLOPE_48 = 48;
    }
    
    // ========================================================================
    // DSP HELPER FUNCTIONS
    // ========================================================================
    
    namespace Helpers
    {
        /**
         * Clamp value between min and max
         */
        template<typename T>
        constexpr T clamp(T value, T minVal, T maxVal)
        {
            return value < minVal ? minVal : (value > maxVal ? maxVal : value);
        }
        
        /**
         * Linear interpolation
         */
        constexpr double lerp(double a, double b, double t)
        {
            return a + t * (b - a);
        }
        
        /**
         * Convert samples to milliseconds
         */
        constexpr double samplesToMs(int samples, double sampleRate)
        {
            return static_cast<double>(samples) / sampleRate * 1000.0;
        }
        
        /**
         * Convert milliseconds to samples
         */
        constexpr int msToSamples(double ms, double sampleRate)
        {
            return static_cast<int>(ms / 1000.0 * sampleRate);
        }
        
        /**
         * Convert seconds to milliseconds
         */
        constexpr double secondsToMs(double seconds)
        {
            return seconds * 1000.0;
        }
        
        /**
         * Convert milliseconds to seconds
         */
        constexpr double msToSeconds(double ms)
        {
            return ms / 1000.0;
        }
    }
}
