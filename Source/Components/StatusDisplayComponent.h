#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../SSOT/UISSOT.h"
#include "../SSOT/ModelSSOT.h"
#include "../SSOT/DSPSSOT.h"

/**
 * Status Display Component
 *
 * Displays LUFS-I measurement, True Peak, and deviation bar.
 * Visualizes loudness status with color-coded feedback.
 */
class StatusDisplayComponent : public juce::Component
{
public:
    // ========================================================================
    // CONSTRUCTOR / DESTRUCTOR
    // ========================================================================

    StatusDisplayComponent();
    ~StatusDisplayComponent() override = default;

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

    // Deviation bar calculation
    double deviationNormalized{0.5};  // 0.5 = center (balanced)

    // ========================================================================
    // PRIVATE METHODS
    // ========================================================================

    void updateDeviationNormalized();
    juce::Colour getStatusColor() const;
    juce::String getStatusText() const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StatusDisplayComponent)
};
