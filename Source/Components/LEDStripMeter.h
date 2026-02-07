#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../SSOT/UISSOT.h"

/**
 * LED Strip Meter Component
 *
 * Calibrated LED strip meter for LUFS visualization.
 * Displays loudness level with color-coded LED segments and target marker.
 *
 * Optimization: LED data cached in resized(), recalculated only on size change.
 *
 * Calibration Presets:
 * - Streaming (Spotify/YouTube): TARGET=-14, WARNING=-9, FLOOR=-60
 * - Broadcast (EBU R128):      TARGET=-23, WARNING=-18, FLOOR=-60
 * - Cinema (Netflix):           TARGET=-27, WARNING=-22, FLOOR=-70
 * - Mastering:                  TARGET=-12, WARNING=-6, FLOOR=-60
 */
class LEDStripMeter : public juce::Component, public juce::Timer
{
public:
    // ========================================================================
    // CALIBRATION SETTINGS (BULLsEYE defaults for Streaming)
    // ========================================================================

    /// Meter floor level (dB)
    static constexpr float FLOOR = -60.0f;

    /// Target LUFS (default: streaming/YouTube/Spotify)
    static constexpr float TARGET = -14.0f;

    /// Meter ceiling (0 dB)
    static constexpr float CEILING = 0.0f;

    /// Number of LED segments
    static constexpr int NUM_LEDS = 20;

    // ========================================================================
    // CONSTRUCTOR / DESTRUCTOR
    // ========================================================================

    LEDStripMeter();
    ~LEDStripMeter() override;  // Fixes DI-5: explicit destructor to call stopTimer()

    // ========================================================================
    // STATE UPDATES
    // ========================================================================

    /**
     * Set the meter level (0.0 to 1.0 normalized)
     */
    void setLevel(float normalizedLevel);

    /**
     * Set the target LUFS value (for dynamic display)
     */
    void setTarget(float targetLUFS);

    // ========================================================================
    // JUCE LIFECYCLE
    // ========================================================================

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    // ========================================================================
    // PRIVATE TYPES
    // ========================================================================

    /// Cached LED data for optimized painting
    struct LEDData
    {
        float x;              // X position
        float threshold;       // Activation threshold (0-1)
        juce::Colour colour;  // LED color
    };

    // ========================================================================
    // PRIVATE MEMBERS
    // ========================================================================

    /// Current displayed level (for smooth animation)
    float currentLevel = 0.0f;

    /// Target level (set from processor)
    float targetLevel = 0.0f;

    /// Current target LUFS (may differ from calibration)
    float currentTarget = TARGET;

    /// Cached LED data (recalculated in resized())
    LEDData cachedLEDs[NUM_LEDS];

    /// Cached LED strip bounds
    juce::Rectangle<float> cachedLEDArea;

    /// Cached target marker position
    float cachedTargetX = 0.0f;

    // ========================================================================
    // PRIVATE METHODS
    // ========================================================================

    /**
     * Calculate LED colors (called once per target change)
     */
    void calculateLEDColors();

    /**
     * Update cached LED positions (called in resized())
     */
    void cacheLEDPositions();

    /**
     * Get LED color based on LUFS value
     */
    juce::Colour getLEDColor(float lufsDB) const;

    /**
     * Calculate normalized position for a given LUFS value
     */
    float lufsToNormalized(float lufsDB) const;

    /**
     * JUCE Timer callback for smooth animation
     */
    void timerCallback() override;

    // ========================================================================
    // JUCE MACROS
    // ========================================================================

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LEDStripMeter)
};
