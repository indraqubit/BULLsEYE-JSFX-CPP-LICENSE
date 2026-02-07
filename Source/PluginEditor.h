#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "SSOT/UISSOT.h"
#include "SSOT/ModelSSOT.h"
#include "Components/StatusDisplayComponent.h"
#include "Components/ModeSelectorComponent.h"
#include "Components/CircularMeterComponent.h"

/**
 * BULLsEYE Audio Processor Editor
 *
 * Main UI for the LUFS-I meter with True Peak detection.
 * Displays loudness measurements via circular donut meter and allows content type selection.
 */
class BULLsEYEEditor : public juce::AudioProcessorEditor,
                       public juce::Timer
{
public:
    // ========================================================================
    // CONSTRUCTOR / DESTRUCTOR
    // ========================================================================

    explicit BULLsEYEEditor(BULLsEYEProcessor& p);
    ~BULLsEYEEditor() override;

    // ========================================================================
    // JUCE LIFECYCLE
    // ========================================================================

    void paint(juce::Graphics& g) override;
    void resized() override;

    // ========================================================================
    // TIMER (for meter updates)
    // ========================================================================

    void timerCallback() override;

private:
    // ========================================================================
    // PRIVATE MEMBERS
    // ========================================================================

    BULLsEYEProcessor& audioProcessor;

    // UI Components
    StatusDisplayComponent statusDisplay;
    ModeSelectorComponent modeSelector;
    CircularMeterComponent circularMeter;

    // ========================================================================
    // HELPER METHODS
    // ========================================================================

    void updateStatusDisplay();
    void updateModeSelector();
    void updateCircularMeter();

    // ========================================================================
    // JUCE MACROS
    // ========================================================================

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BULLsEYEEditor)
};
