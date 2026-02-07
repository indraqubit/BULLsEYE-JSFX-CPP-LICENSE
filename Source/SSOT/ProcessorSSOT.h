#pragma once

namespace ProcessorSSOT
{
    // ==========================================
    // BUFFER AND LATENCY PARAMETERS
    // ==========================================
    namespace Buffer
    {
        constexpr int MAX_BUFFER_SIZE = 8192;
        constexpr int MIN_BUFFER_SIZE = 64;
    }

    // ==========================================
    // LATENCY
    // ==========================================
    namespace Latency
    {
        // K-weighting filters introduce minimal latency
        // (biquad implementation dependent)
        constexpr int MIN_LATENCY_SAMPLES = 0;
        constexpr int MAX_LATENCY_SAMPLES = 0;
    }

    // ==========================================
    // CHANNEL CONFIGURATION
    // ==========================================
    namespace Channels
    {
        constexpr int MAX_INPUT_CHANNELS = 2;
        constexpr int MAX_OUTPUT_CHANNELS = 2;
        constexpr int DEFAULT_INPUT_CHANNELS = 2;
        constexpr int DEFAULT_OUTPUT_CHANNELS = 2;

        // Supported configurations
        constexpr bool SUPPORTS_STEREO = true;
        constexpr bool SUPPORTS_MONO = true;
        constexpr bool SUPPORTS_SURROUND = false;
    }

    // ==========================================
    // SAMPLE RATE
    // ==========================================
    namespace SampleRate
    {
        constexpr double MIN_SAMPLE_RATE = 44100.0;
        constexpr double MAX_SAMPLE_RATE = 192000.0;
        constexpr double DEFAULT_SAMPLE_RATE = 48000.0;
        constexpr double SUPPORTED_RATES[] = {44100.0, 48000.0, 88200.0, 96000.0, 176400.0, 192000.0};
        constexpr int SUPPORTED_RATE_COUNT = 6;
    }

    // ==========================================
    // BIT DEPTH
    // ==========================================
    namespace BitDepth
    {
        constexpr int PROCESS_BITS = 64;  // Double precision internal
        constexpr int INPUT_BITS = 32;    // Float input
        constexpr int OUTPUT_BITS = 32;    // Float output
    }

    // ==========================================
    // INITIALIZATION
    // ==========================================
    namespace Initialization
    {
        // Initial state values
        constexpr double INITIAL_LUFS = -90.0;  // Below gate threshold
        constexpr double INITIAL_TRUE_PEAK_DB = -120.0;
        constexpr double INITIAL_DEVIATION_DB = 0.0;
    }

    // ==========================================
    // SMOOTHING
    // ==========================================
    namespace Smoothing
    {
        // Parameter smoothing coefficient
        constexpr double PARAMETER_SMOOTHING_COEF = 0.25;

        // Meter smoothing (for visual display)
        constexpr double METER_SMOOTHING_COEF = 0.3;
    }

    // ==========================================
    // THREAD SAFETY
    // ==========================================
    namespace ThreadSafety
    {
        // Audio thread safety rules
        constexpr bool ALLOW_ALLOCATIONS_IN_PROCESS = false;
        constexpr bool ALLOW_UI_ACCESS_IN_PROCESS = false;
        constexpr bool ALLOW_LOCKS_IN_PROCESS = false;
    }

    // ==========================================
    // PERFORMANCE CONSTANTS
    // ==========================================
    namespace Performance
    {
        // Expected CPU usage (per channel, at 48kHz)
        // Based on optimized implementation with cached coefficients
        constexpr double CPU_PERCENT_PER_CHANNEL = 0.08;  // ~0.08% per channel

        // True Peak atomic batching
        constexpr int TRUE_PEAK_BATCH_SIZE = 100;  // Update atomic every N samples

        // DSP operations per sample (optimized count)
        constexpr int OPS_PER_SAMPLE_KWEIGHTING = 18;    // 2 biquads = 10 mults + 8 adds
        constexpr int OPS_PER_SAMPLE_ENERGY = 5;          // 2 mults + 2 adds + 1 comparison
        constexpr int OPS_PER_SAMPLE_TRUE_PEAK = 120;     // 4 interp * 8 mults + 4 abs + 4 max + overhead

        // Estimated total operations per stereo sample
        constexpr int OPS_PER_STEREO_SAMPLE = (OPS_PER_SAMPLE_KWEIGHTING * 2) +
                                               OPS_PER_SAMPLE_ENERGY +
                                               (OPS_PER_SAMPLE_TRUE_PEAK * 2);

        // Memory footprint
        constexpr size_t STATE_SIZE_BYTES = sizeof(double) * 24;  // Filter states + buffers
        constexpr size_t CACHE_LINE_PADDING = 64;  // L1 cache line size

        // Worst-case CPU at 192kHz stereo (estimated)
        constexpr double MAX_CPU_PERCENT_AT_192KHZ = 0.35;
    }
}
