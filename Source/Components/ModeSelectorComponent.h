#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "../SSOT/UISSOT.h"
#include "../SSOT/ModelSSOT.h"

/**
 * Mode Selector Component
 *
 * Dropdown selector for content type (Music Non-drums, Music Drums, Cinema/Trailer).
 * Updates target LUFS based on selection.
 */
class ModeSelectorComponent : public juce::Component,
                             public juce::ComboBox::Listener
{
public:
    // ========================================================================
    // CONSTRUCTOR / DESTRUCTOR
    // ========================================================================

    ModeSelectorComponent();
    ~ModeSelectorComponent() override = default;

    // ========================================================================
    // JUCE LIFECYCLE
    // ========================================================================

    void paint(juce::Graphics& g) override;
    void resized() override;

    // ========================================================================
    // SETUP
    // ========================================================================

    void setAPVTS(juce::AudioProcessorValueTreeState* apvts);

    // ========================================================================
    // STATE UPDATES
    // ========================================================================

    void setCurrentMode(int modeIndex);
    int getCurrentMode() const { return currentModeIndex; }

private:
    // ========================================================================
    // PRIVATE MEMBERS
    // ========================================================================

    juce::ComboBox modeComboBox;
    juce::Label modeLabel;

    juce::AudioProcessorValueTreeState* apvtsPtr{nullptr};
    int currentModeIndex{1};  // Default: Music Drums

    // ========================================================================
    // PRIVATE METHODS
    // ========================================================================

    void updateModeLabel();
    void onModeChanged(int newMode);

    // juce::ComboBoxListener
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModeSelectorComponent)
};
