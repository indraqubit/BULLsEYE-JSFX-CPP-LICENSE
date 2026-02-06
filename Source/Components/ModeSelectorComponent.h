#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "../SSOT/UISSOT.h"
#include "../SSOT/ModelSSOT.h"

/**
 * Mode Selector Component
 * 
 * Previous/Next buttons to cycle through modes.
 * TODO: Customize for your plugin
 */
class ModeSelectorComponent : public juce::Component
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
    
private:
    // ========================================================================
    // PRIVATE MEMBERS
    // ========================================================================
    
    juce::TextButton prevButton;
    juce::TextButton nextButton;
    juce::Label modeLabel;
    
    juce::AudioProcessorValueTreeState* apvtsPtr{nullptr};
    int currentModeIndex{0};
    
    // ========================================================================
    // PRIVATE METHODS
    // ========================================================================
    
    void updateButtons();
    void updateModeLabel();
    void onPrevButtonClicked();
    void onNextButtonClicked();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModeSelectorComponent)
};
