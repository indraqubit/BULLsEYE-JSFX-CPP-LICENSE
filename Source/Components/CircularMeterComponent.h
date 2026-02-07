#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../SSOT/UISSOT.h"
#include "../SSOT/ModelSSOT.h"
#include "../SSOT/DSPSSOT.h"

/**
 * Circular Meter Component
 *
 * Displays LUFS-I as a filled arc (donut meter) with smooth animation.
 * Color indicates status: green (balanced), red (hot), blue (quiet).
 * More professional than LED strip, uses less vertical space.
 */
class CircularMeterComponent : public juce::Component,
                              public juce::Timer
{
public:
    // ========================================================================
    // CONSTRUCTOR / DESTRUCTOR
    // ========================================================================

    CircularMeterComponent();
    ~CircularMeterComponent() override;

    // ========================================================================
    // JUCE LIFECYCLE
    // ========================================================================

    void paint(juce::Graphics& g) override;
    void resized() override;

    // ========================================================================
    // STATE UPDATES
    // ========================================================================

    void setValues(double lufs, double truePeakDB, double deviationLU, ModelSSOT::ContentType contentType);

private:
    // ========================================================================
    // PRIVATE MEMBERS
    // ========================================================================

    // Current measurement values
    double currentLUFS{DSPSSOT::TruePeak::MIN_DISPLAY_DB};
    double currentTruePeak{DSPSSOT::TruePeak::MIN_DISPLAY_DB};
    double currentDeviation{0.0};
    ModelSSOT::ContentType currentContentType{ModelSSOT::ContentType::MusicDrums};

    // Animation
    float animatedLevel{0.0f};
    float targetLevel{0.0f};

    // ========================================================================
    // PRIVATE METHODS
    // ========================================================================

    void timerCallback() override;
    juce::Colour getStatusColor() const;
    juce::String getStatusText() const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CircularMeterComponent)
};
