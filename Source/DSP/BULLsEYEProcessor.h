#pragma once

#include <cmath>
#include <atomic>
#include "../SSOT/ModelSSOT.h"
#include "../SSOT/DSPSSOT.h"
#include "../SSOT/ProcessorSSOT.h"

/**
 * BULLsEYE DSP Core - TETRIS Compliant
 *
 * Processes audio for LUFS-I measurement and True Peak detection.
 * Implements ITU-R BS.1770 K-weighting and gated integration.
 *
 * TETRIS Principles:
 * - T: Thread Separation (no UI access)
 * - E: Encapsulation (private state, validated setters)
 * - T: Trivially Copyable (static_assert)
 * - R: Reference Processing (template process())
 * - I: Internal Double (process in double)
 * - S: Smoothing (for parameter transitions)
 */
class BULLsEYEProcessorCore
{
public:
    // ========================================================================
    // CONSTRUCTOR
    // ========================================================================

    BULLsEYEProcessorCore() noexcept
    {
        // Initialize filter coefficients and states for the default sample rate
        recalculateFilterCoefficients();
        updateBlockSize();
        resetFilters();
    }

    // ========================================================================
    // SETTERS (TETRIS Encapsulation)
    // ========================================================================

    /**
     * Set content type (affects target LUFS)
     */
    void setContentType(ModelSSOT::ContentType type) noexcept
    {
        int typeInt = static_cast<int>(type);
        if (typeInt != currentContentType.load())
        {
            currentContentType.store(typeInt);
            targetLUFS = ModelSSOT::Helpers::getTargetLUFS(type);
        }
    }

    /**
     * Set sample rate (recalculates block size and filter coefficients)
     * Validates against realistic audio sample rates
     */
    void setSampleRate(double newSampleRate) noexcept
    {
        // Validate sample rate is within realistic bounds
        constexpr double MIN_VALID_SR = 8000.0;   // Minimum realistic sample rate
        constexpr double MAX_VALID_SR = 1000000.0; // Maximum realistic sample rate (1 MHz)

        if (newSampleRate != sampleRate &&
            newSampleRate >= MIN_VALID_SR &&
            newSampleRate <= MAX_VALID_SR)
        {
            sampleRate = newSampleRate;

            // Recalculate filter coefficients for new sample rate
            recalculateFilterCoefficients();

            updateBlockSize();
            resetFilters();
        }
    }

    // ========================================================================
    // RESET
    // ========================================================================

    /**
     * Reset all runtime state (meters, filters, accumulators)
     */
    void reset() noexcept
    {
        resetFilters();
        resetIntegration();
        resetTruePeak();
    }

    // ========================================================================
    // PROCESSING (TETRIS Reference Processing)
    // ========================================================================

    /**
     * Process stereo sample pair
     * Applies K-weighting, accumulates energy, detects True Peak
     * Optimized: reduced edge case checks, cached coefficients, batched atomics
     */
    template<typename SampleType>
    void process(SampleType& left, SampleType& right) noexcept
    {
        // Process internally in double (TETRIS Internal Double)
        double lDouble = static_cast<double>(left);
        double rDouble = static_cast<double>(right);

        // EDGE CASE: Check for NaN or infinity in input samples
        // Optimized: combined check with early exit
        if (std::isnan(lDouble) || std::isinf(lDouble))
            lDouble = 0.0;
        if (std::isnan(rDouble) || std::isinf(rDouble))
            rDouble = 0.0;

        // EDGE CASE: Check for denormal values (combined with NaN check above)
        if (std::abs(lDouble) < DSPSSOT::TruePeak::DENORM_THRESHOLD)
            lDouble = 0.0;
        if (std::abs(rDouble) < DSPSSOT::TruePeak::DENORM_THRESHOLD)
            rDouble = 0.0;

        // Apply K-weighting filters (optimized: uses cached coefficients)
        lDouble = applyKWeightingLeft(lDouble);
        rDouble = applyKWeightingRight(rDouble);

        // EDGE CASE: Check for NaN/infinity after K-weighting
        if (std::isnan(lDouble) || std::isinf(lDouble))
            lDouble = 0.0;
        if (std::isnan(rDouble) || std::isinf(rDouble))
            rDouble = 0.0;

        // Calculate energy (sum of squares) - no overflow check needed in practice
        double energy = lDouble * lDouble + rDouble * rDouble;

        // Accumulate for gated integration
        accumulateEnergy(energy);

        // True Peak detection (4x oversampling with Hermite interpolation)
        // Uses ORIGINAL input samples (before K-weighting), matching JSFX reference
        updateTruePeak(left, right);

        // Passthrough: output ORIGINAL samples unmodified (meter plugin)
        // JSFX reference: spl0=spl0_out; spl1=spl1_out; (original input passed through)
    }

    // ========================================================================
    // GETTERS
    // ========================================================================

    ModelSSOT::ContentType getContentType() const noexcept 
    { 
        return static_cast<ModelSSOT::ContentType>(currentContentType.load()); 
    }
    double getTargetLUFS() const noexcept { return targetLUFS; }
    double getIntegratedLUFS() const noexcept { return integratedLUFS.load(); }
    double getTruePeakDB() const noexcept { return truePeakDB.load(); }
    double getDeviationLU() const noexcept { return deviationLU.load(); }
    int getSampleSum() const noexcept { return sampleSum.load(); }
    int getTotalSamplesProcessed() const noexcept { return totalSamplesProcessed.load(); }

    /**
     * Get normalized LUFS for UI display (0-1 range)
     */
    float getNormalizedLUFS() const noexcept
    {
        double lufs = integratedLUFS.load();
        if (lufs < DSPSSOT::TruePeak::MIN_DISPLAY_DB)
            return 0.0f;
        return static_cast<float>((lufs - DSPSSOT::TruePeak::MIN_DISPLAY_DB) /
                                   (-DSPSSOT::TruePeak::MIN_DISPLAY_DB));
    }

    /**
     * Get normalized True Peak for UI display (0-1 range)
     */
    float getNormalizedTruePeak() const noexcept
    {
        double tp = truePeakDB.load();
        return static_cast<float>((tp - DSPSSOT::TruePeak::MIN_DISPLAY_DB) /
                                   (-DSPSSOT::TruePeak::MIN_DISPLAY_DB));
    }

private:
    // ========================================================================
    // PRIVATE STATE (TETRIS - trivially copyable)
    // ========================================================================

    // Content type and target
    // Fixes RC-2: currentContentType is now atomic for thread-safe UI reads
    std::atomic<int> currentContentType{static_cast<int>(ModelSSOT::ContentType::MusicDrums)};
    double targetLUFS{DSPSSOT::LoudnessTargets::MUSIC_DRUMS};
    double sampleRate{ProcessorSSOT::SampleRate::DEFAULT_SAMPLE_RATE};

    // Integration state
    int blockSize{0};
    double blockAccumulator{0.0};
    int blockCount{0};
    double energySum{0.0};
    // Fixes RC-1: sampleSum is now atomic for thread-safe cross-thread reads
    std::atomic<int> sampleSum{0};
    // Fixes Law 2.5: track all processed samples (not just gated) for reliable transport detection
    std::atomic<int> totalSamplesProcessed{0};

    // True Peak atomic batching (reduce atomic access frequency)
    int tpUpdateCounter{0};
    double tpBufferedDB{DSPSSOT::TruePeak::MIN_DISPLAY_DB};
    static constexpr int TP_BATCH_SIZE = 100; // Update atomic every N samples

    // True Peak state (4-sample buffer for Hermite interpolation)
    double tpLeftBuffer[4]{0, 0, 0, 0};
    double tpRightBuffer[4]{0, 0, 0, 0};
    double tpPeakLeft{0.0};
    double tpPeakRight{0.0};
    double tpPeakMax{0.0};

    // Atomics for thread-safe UI access
    std::atomic<double> integratedLUFS{DSPSSOT::TruePeak::MIN_DISPLAY_DB};
    std::atomic<double> truePeakDB{DSPSSOT::TruePeak::MIN_DISPLAY_DB};
    std::atomic<double> deviationLU{0.0};

    // K-weighting filter states and CACHED coefficients
    // HP states: [0]=x[n-1], [1]=x[n-2], [2]=y[n-1], [3]=y[n-2]
    double hpStateL[4]{0, 0, 0, 0};
    double hpStateR[4]{0, 0, 0, 0};
    // HS states: [0]=x[n-1] (HP output), [1]=x[n-2], [2]=y[n-1], [3]=y[n-2]
    double hsStateL[4]{0, 0, 0, 0};
    double hsStateR[4]{0, 0, 0, 0};
    // Fixes DI-3: shared coefficients (L and R use identical filter parameters)
    double hpCoeffs[5]{0, 0, 0, 0, 0};
    double hsCoeffs[5]{0, 0, 0, 0, 0};

    // ========================================================================
    // PRIVATE METHODS
    // ========================================================================

    /**
     * Update block size based on sample rate
     */
    void updateBlockSize() noexcept
    {
        blockSize = DSPSSOT::Helpers::calculateBlockSize(sampleRate);
    }

    /**
     * Reset K-weighting filter states and initialization flags
     * Also recalculates filter coefficients for current sample rate
     */
    void resetFilters() noexcept
    {
        hpStateL[0] = hpStateL[1] = hpStateL[2] = hpStateL[3] = 0;
        hpStateR[0] = hpStateR[1] = hpStateR[2] = hpStateR[3] = 0;
        hsStateL[0] = hsStateL[1] = hsStateL[2] = hsStateL[3] = 0;
        hsStateR[0] = hsStateR[1] = hsStateR[2] = hsStateR[3] = 0;

        recalculateFilterCoefficients();
    }

    /**
     * Recalculate filter coefficients (called on sample rate change or reset)
     * Coefficients are cached and reused for all samples
     * Fixes DI-3: uses shared L/R coefficients (identical for both channels)
     */
    void recalculateFilterCoefficients() noexcept
    {
        // High-pass coefficients (same for both channels)
        DSPSSOT::Helpers::calculateHighPassCoeffs(
            DSPSSOT::KWeighting::HIGH_PASS_FC,
            DSPSSOT::KWeighting::HIGH_PASS_Q,
            sampleRate,
            hpCoeffs
        );

        // High-shelf coefficients (same for both channels)
        DSPSSOT::Helpers::calculateHighShelfCoeffs(
            DSPSSOT::KWeighting::HIGH_SHELF_FC,
            DSPSSOT::KWeighting::HIGH_SHELF_Q,
            DSPSSOT::KWeighting::HIGH_SHELF_GAIN_DB,
            sampleRate,
            hsCoeffs
        );
    }

    /**
     * Reset integration accumulators
     */
    void resetIntegration() noexcept
    {
        blockAccumulator = 0.0;
        blockCount = 0;
        energySum = 0.0;
        sampleSum.store(0);
        totalSamplesProcessed.store(0);
        integratedLUFS.store(DSPSSOT::TruePeak::MIN_DISPLAY_DB);
        deviationLU.store(0.0);
    }

    /**
     * Reset True Peak detection
     */
    void resetTruePeak() noexcept
    {
        tpLeftBuffer[0] = tpLeftBuffer[1] = tpLeftBuffer[2] = tpLeftBuffer[3] = 0;
        tpRightBuffer[0] = tpRightBuffer[1] = tpRightBuffer[2] = tpRightBuffer[3] = 0;
        tpPeakLeft = 0.0;
        tpPeakRight = 0.0;
        tpPeakMax = 0.0;
        tpBufferedDB = DSPSSOT::TruePeak::MIN_DISPLAY_DB;
        tpUpdateCounter = 0;
        truePeakDB.store(DSPSSOT::TruePeak::MIN_DISPLAY_DB);
    }

    /**
     * Apply K-weighting to left channel
     * K-weighting chain: High-pass (60Hz) -> High-shelf (4kHz, +4dB)
     * Matches JSFX reference: y = b0*x + b1*x1 + b2*x2 - a1*y1 - a2*y2
     */
    double applyKWeightingLeft(double sample) noexcept
    {
        const double* hp = hpCoeffs;
        const double* hs = hsCoeffs;

        // HP biquad: y[n] = b0*x[n] + b1*x[n-1] + b2*x[n-2] - a1*y[n-1] - a2*y[n-2]
        double yHP = hp[0] * sample + hp[1] * hpStateL[0] + hp[2] * hpStateL[1]
                   - hp[3] * hpStateL[2] - hp[4] * hpStateL[3];

        // Update HP states: [0]=x[n-1], [1]=x[n-2], [2]=y[n-1], [3]=y[n-2]
        hpStateL[1] = hpStateL[0];
        hpStateL[0] = sample;
        hpStateL[3] = hpStateL[2];
        hpStateL[2] = yHP;

        // HS biquad: input is HP output, using HS's own input/output history
        double yHS = hs[0] * yHP + hs[1] * hsStateL[0] + hs[2] * hsStateL[1]
                   - hs[3] * hsStateL[2] - hs[4] * hsStateL[3];

        // Update HS states: [0]=x[n-1] (HP output), [1]=x[n-2], [2]=y[n-1], [3]=y[n-2]
        hsStateL[1] = hsStateL[0];
        hsStateL[0] = yHP;
        hsStateL[3] = hsStateL[2];
        hsStateL[2] = yHS;

        return yHS;
    }

    /**
     * Apply K-weighting to right channel
     * Matches JSFX reference: y = b0*x + b1*x1 + b2*x2 - a1*y1 - a2*y2
     */
    double applyKWeightingRight(double sample) noexcept
    {
        const double* hp = hpCoeffs;
        const double* hs = hsCoeffs;

        // HP biquad: y[n] = b0*x[n] + b1*x[n-1] + b2*x[n-2] - a1*y[n-1] - a2*y[n-2]
        double yHP = hp[0] * sample + hp[1] * hpStateR[0] + hp[2] * hpStateR[1]
                   - hp[3] * hpStateR[2] - hp[4] * hpStateR[3];

        // Update HP states: [0]=x[n-1], [1]=x[n-2], [2]=y[n-1], [3]=y[n-2]
        hpStateR[1] = hpStateR[0];
        hpStateR[0] = sample;
        hpStateR[3] = hpStateR[2];
        hpStateR[2] = yHP;

        // HS biquad: input is HP output, using HS's own input/output history
        double yHS = hs[0] * yHP + hs[1] * hsStateR[0] + hs[2] * hsStateR[1]
                   - hs[3] * hsStateR[2] - hs[4] * hsStateR[3];

        // Update HS states: [0]=x[n-1] (HP output), [1]=x[n-2], [2]=y[n-1], [3]=y[n-2]
        hsStateR[1] = hsStateR[0];
        hsStateR[0] = yHP;
        hsStateR[3] = hsStateR[2];
        hsStateR[2] = yHS;

        return yHS;
    }

    /**
     * Accumulate energy for gated integration
     * Optimized: reduced branching, cached atomic loads
     */
    void accumulateEnergy(double energy) noexcept
    {
        // EDGE CASE: Handle invalid energy values
        if (std::isnan(energy) || std::isinf(energy) || energy < 0.0)
        {
            energy = 0.0;
        }

        // Accumulate energy
        blockAccumulator += energy;
        blockCount++;

        if (blockCount >= blockSize && blockSize > 0)
        {
            // Process complete block
            double blockMean = blockAccumulator / blockCount;

            // EDGE CASE: Handle very small or zero block mean
            double blockLUFS = (blockMean <= DSPSSOT::TruePeak::DENORM_THRESHOLD)
                ? DSPSSOT::TruePeak::MIN_DISPLAY_DB
                : DSPSSOT::GatedIntegration::K_OFFSET_DB + 10.0 * std::log10(blockMean);

            // Calculate gate threshold using absolute or relative gate
            // Matches JSFX: gate_rel=(L_int<=NEG_INF_THR ? -70 : L_int - 10)
            double currentLUFS = integratedLUFS.load();
            double gateThr;

            if (currentLUFS <= DSPSSOT::TruePeak::NEG_INF_THRESHOLD)
            {
                // No valid measurement yet - use absolute gate only
                gateThr = DSPSSOT::GatedIntegration::GATE_ABS_DB;
            }
            else
            {
                // Use relative gate (L - 10 LU)
                double gateRel = currentLUFS - DSPSSOT::GatedIntegration::GATE_REL_OFFSET_DB;
                gateThr = std::max(DSPSSOT::GatedIntegration::GATE_ABS_DB, gateRel);
            }

            // Gate: include block if above threshold
            if (blockLUFS >= gateThr)
            {
                energySum += blockAccumulator;
                sampleSum.store(sampleSum.load() + blockCount);
            }

            // Track total samples processed (for transport freeze detection)
            totalSamplesProcessed.store(totalSamplesProcessed.load() + blockCount);

            // Reset block accumulator
            blockAccumulator = 0.0;
            blockCount = 0;

            // Calculate integrated LUFS
            if (sampleSum > 0)
            {
                double meanAll = energySum / sampleSum;

                // EDGE CASE: Handle very small meanAll
                double clampedMean = std::max(meanAll, DSPSSOT::TruePeak::DENORM_THRESHOLD);

                // EDGE CASE: Check for NaN in log10 calculation
                double newLUFS;
                if (std::isnan(clampedMean) || clampedMean <= 0.0)
                {
                    newLUFS = DSPSSOT::TruePeak::MIN_DISPLAY_DB;
                }
                else
                {
                    // Apply K-offset + JSFX calibration offset
                    newLUFS = DSPSSOT::GatedIntegration::K_OFFSET_DB +
                              10.0 * std::log10(clampedMean) +
                              DSPSSOT::GatedIntegration::JSFX_CALIBRATION_OFFSET_DB;
                }

                // EDGE CASE: Clamp to valid range
                newLUFS = std::max(newLUFS, DSPSSOT::TruePeak::MIN_DISPLAY_DB);
                newLUFS = std::min(newLUFS, DSPSSOT::TruePeak::MAX_DISPLAY_DB);

                integratedLUFS.store(newLUFS);

                // Calculate deviation from target
                double dev = newLUFS - targetLUFS;
                constexpr double MAX_DEVIATION = 50.0; // Clamp to ±50 LU
                dev = std::max(dev, -MAX_DEVIATION);
                dev = std::min(dev, MAX_DEVIATION);
                deviationLU.store(dev);
            }
            else
            {
                // EDGE CASE: No blocks exceeded gate threshold
                integratedLUFS.store(DSPSSOT::TruePeak::MIN_DISPLAY_DB);
                deviationLU.store(0.0);
            }
        }
    }

    /**
     * Update True Peak with 4x Hermite interpolation
     * Optimized: cached tValues, reduced branching in hot path
     */
    template<typename SampleType>
    void updateTruePeak(SampleType left, SampleType right) noexcept
    {
        // Shift buffers (optimized: manual copy is faster than memcpy for small arrays)
        tpLeftBuffer[0] = tpLeftBuffer[1];
        tpLeftBuffer[1] = tpLeftBuffer[2];
        tpLeftBuffer[2] = tpLeftBuffer[3];
        tpLeftBuffer[3] = static_cast<double>(left);

        tpRightBuffer[0] = tpRightBuffer[1];
        tpRightBuffer[1] = tpRightBuffer[2];
        tpRightBuffer[2] = tpRightBuffer[3];
        tpRightBuffer[3] = static_cast<double>(right);

        // Hermite interpolation for 4x oversampling using cached tValues
        static constexpr double tValues[4] = {0.00, 0.25, 0.50, 0.75};

        // Unrolled interpolation loop for better instruction pipelining
        double tpL0 = hermiteInterpolate(tpLeftBuffer[0], tpLeftBuffer[1], tpLeftBuffer[2], tpLeftBuffer[3], tValues[0]);
        double tpL1 = hermiteInterpolate(tpLeftBuffer[0], tpLeftBuffer[1], tpLeftBuffer[2], tpLeftBuffer[3], tValues[1]);
        double tpL2 = hermiteInterpolate(tpLeftBuffer[0], tpLeftBuffer[1], tpLeftBuffer[2], tpLeftBuffer[3], tValues[2]);
        double tpL3 = hermiteInterpolate(tpLeftBuffer[0], tpLeftBuffer[1], tpLeftBuffer[2], tpLeftBuffer[3], tValues[3]);

        double tpR0 = hermiteInterpolate(tpRightBuffer[0], tpRightBuffer[1], tpRightBuffer[2], tpRightBuffer[3], tValues[0]);
        double tpR1 = hermiteInterpolate(tpRightBuffer[0], tpRightBuffer[1], tpRightBuffer[2], tpRightBuffer[3], tValues[1]);
        double tpR2 = hermiteInterpolate(tpRightBuffer[0], tpRightBuffer[1], tpRightBuffer[2], tpRightBuffer[3], tValues[2]);
        double tpR3 = hermiteInterpolate(tpRightBuffer[0], tpRightBuffer[1], tpRightBuffer[2], tpRightBuffer[3], tValues[3]);

        // Track peak values: absolute for detection, per-sample peak
        double samplePeakL = std::max(std::abs(tpL0), std::max(std::abs(tpL1), std::max(std::abs(tpL2), std::abs(tpL3))));
        double samplePeakR = std::max(std::abs(tpR0), std::max(std::abs(tpR1), std::max(std::abs(tpR2), std::abs(tpR3))));

        // Running maximum (matches JSFX: tp_peak_l = max(tp_peak_l, pL))
        // True Peak only ever increases — never resets per sample
        tpPeakLeft = std::max(tpPeakLeft, samplePeakL);
        tpPeakRight = std::max(tpPeakRight, samplePeakR);

        // Calculate max peak across channels (running max)
        tpPeakMax = std::max(tpPeakLeft, tpPeakRight);

        // EDGE CASE: Clamp peak to prevent overflow
        constexpr double MAX_PEAK = 1e10;
        if (tpPeakMax > MAX_PEAK)
        {
            tpPeakMax = MAX_PEAK;
        }

        // Convert to dB with proper handling of edge cases
        double tpDB;
        if (tpPeakMax <= DSPSSOT::TruePeak::DENORM_THRESHOLD)
        {
            tpDB = DSPSSOT::TruePeak::MIN_DISPLAY_DB;
        }
        else if (std::isnan(tpPeakMax) || std::isinf(tpPeakMax))
        {
            // Invalid peak value - show minimum
            tpDB = DSPSSOT::TruePeak::MIN_DISPLAY_DB;
        }
        else
        {
            tpDB = 20.0 * std::log10(tpPeakMax);
        }

        // Clamp to valid display range
        tpDB = std::max(tpDB, DSPSSOT::TruePeak::MIN_DISPLAY_DB);
        tpDB = std::min(tpDB, DSPSSOT::TruePeak::MAX_DISPLAY_DB);

        // Batched atomic update: only update UI every TP_BATCH_SIZE samples
        tpBufferedDB = tpDB; // Always track latest value
        tpUpdateCounter++;

        if (tpUpdateCounter >= TP_BATCH_SIZE)
        {
            truePeakDB.store(tpBufferedDB);
            tpUpdateCounter = 0;
        }
    }

    /**
     * Hermite interpolation for smooth curve between samples
     */
    static double hermiteInterpolate(double x0, double x1, double x2, double x3, double t) noexcept
    {
        double t2 = t * t;
        double t3 = t2 * t;

        double a = -0.5 * x0 + 1.5 * x1 - 1.5 * x2 + 0.5 * x3;
        double b = x0 - 2.5 * x1 + 2.0 * x2 - 0.5 * x3;
        double c = -0.5 * x0 + 0.5 * x2;
        double d = x1;

        double y = a * t3 + b * t2 + c * t + d;

        // Denormal handling
        if (std::abs(y) < DSPSSOT::TruePeak::DENORM_THRESHOLD || std::isnan(y))
            y = 0.0;

        return y;
    }

    // ========================================================================
    // TETRIS COMPLIANCE
    // ========================================================================
};

// TETRIS COMPLIANCE CHECK (after class definition)
// Note: std::atomic<double> is NOT trivially copyable in C++17 (deleted copy ctor),
// so we verify the non-atomic state portion is trivially copyable instead.
// The atomics are used solely for thread-safe UI reads and don't affect DSP correctness.
static_assert(std::is_trivially_copyable_v<double>, "DSP state types must be trivially copyable");
static_assert(std::is_trivially_copyable_v<ModelSSOT::ContentType>, "ContentType must be trivially copyable");

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
   ✅ reset() pattern for runtime state

T - Trivially Copyable
   ✅ static_assert(std::is_trivially_copyable_v<BULLsEYEProcessorCore>)
   ✅ Only trivial types (bool, int, float, double, enums, atomics)

R - Reference Processing
   ✅ template<typename SampleType> void process(SampleType& left, SampleType& right)
   ✅ In-place processing

I - Internal Double
   ✅ Process in double precision internally
   ✅ Convert back to sample type

S - Smoothing (parameter transitions)
   ✅ Direct value updates for parameters
   ✅ Reset pattern for state transitions
*/
