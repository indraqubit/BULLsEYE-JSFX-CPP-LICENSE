#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "SSOT/UISSOT.h"
#include "SSOT/ModelSSOT.h"
#include "Components/StatusDisplayComponent.h"
#include "Components/ModeSelectorComponent.h"
#include "Components/MuteButtonComponent.h"
#include "Components/InvertToggleComponent.h"
#include "Components/ModeDescriptionComponent.h"

/**
 * Main Audio Processor Editor
 * 
 * Handles UI display and user interaction.
 * TODO: Customize for your plugin
 */
class YourEditor : public juce::AudioProcessorEditor,
                   public juce::AudioProcessorValueTreeState::Listener
{
public:
    // ========================================================================
    // CONSTRUCTOR / DESTRUCTOR
    // ========================================================================
    
    YourEditor(YourProcessor& p);
    ~YourEditor() override;
    
    // ========================================================================
    // JUCE LIFECYCLE
    // ========================================================================
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // ========================================================================
    // APVTS LISTENER
    // ========================================================================
    
    void parameterChanged(const juce::String& parameterID, float newValue) override;
    
private:
    // ========================================================================
    // PRIVATE MEMBERS
    // ========================================================================
    
    YourProcessor& audioProcessor;
    
    // TODO: Add your UI components
    StatusDisplayComponent statusDisplay;
    ModeSelectorComponent modeSelector;
    MuteButtonComponent muteButton;
    InvertToggleComponent invertToggle;
    ModeDescriptionComponent modeDescription;
    
    // ========================================================================
    // HELPER METHODS
    // ========================================================================
    
    void updateStatusDisplay();
    void updateModeSelector();
    void updateMuteButton();
    void updateInvertToggle();
    void updateModeDescription();
    
    // ========================================================================
    // JUCE MACROS
    // ========================================================================
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(YourEditor)
};
