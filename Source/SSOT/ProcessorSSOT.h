#pragma once

/**
 * Processor Layer Single Source of Truth
 * 
 * Contains all processor-related constants:
 * - Buffer sizes
 * - Sample rates
 * - Processing thresholds
 * - Thread safety settings
 * 
 * Usage: Include in files that handle audio processing or parameter management.
 */
namespace ProcessorSSOT
{
    // ========================================================================
    // AUDIO PROCESSING CONSTANTS
    // ========================================================================
    
    namespace Audio
    {
        // Sample rate range (from JSFX)
        constexpr double MIN_SAMPLE_RATE = 44100.0;
        constexpr double MAX_SAMPLE_RATE = 192000.0;
        
        // Buffer size range (from JSFX)
        constexpr int MIN_BUFFER_SIZE = 32;
        constexpr int MAX_BUFFER_SIZE = 2048;
        constexpr int DEFAULT_BUFFER_SIZE = 512;
    }
    
    // ========================================================================
    // PROCESSING CONSTANTS
    // ========================================================================
    
    namespace Processing
    {
        // Denormal handling threshold
        // Value below this is treated as zero (denormal protection)
        constexpr double DENORMAL_THRESHOLD = 1.0e-15;
        
        // Smoothing time constant (if needed)
        // 30ms is a common smoothing time
        constexpr double SMOOTHING_TIME_MS = 30.0;
        
        // Master bypass threshold
        // Value above this means bypass is active
        constexpr float BYPASS_THRESHOLD = 0.5f;
    }
    
    // ========================================================================
    // THREAD SAFETY CONSTANTS
    // ========================================================================
    
    namespace Threading
    {
        // Lock-free ring buffer size for parameter updates
        constexpr int PARAM_UPDATE_BUFFER_SIZE = 64;
        
        // Maximum time to wait for lock (in milliseconds)
        constexpr int LOCK_TIMEOUT_MS = 100;
        
        // Atomic access alignment (usually 4 or 8 bytes)
        constexpr size_t ATOMIC_ALIGNMENT = 8;
    }
    
    // ========================================================================
    // LATENCY CONSTANTS
    // ========================================================================
    
    namespace Latency
    {
        // Plugin latency in samples (0 for no latency)
        constexpr int PLUGIN_LATENCY = 0;
        
        // Input/output latency compensation
        constexpr int INPUT_LATENCY = 0;
        constexpr int OUTPUT_LATENCY = 0;
    }
}
