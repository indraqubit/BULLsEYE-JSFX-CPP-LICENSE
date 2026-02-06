#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "../SSOT/UISSOT.h"
#include "../SSOT/ModelSSOT.h"

/**
 * Invert Toggle Component
 * 
 * Toggle switch for invert functionality.
 * TODO: Customize for your plugin
 */
class InvertToggleComponent : public juce::Component
{
public:
    // ========================================================================
    // CONSTRUCTOR / DESTRUCTOR
    // ========================================================================
    
    InvertToggleComponent();
    ~InvertToggleComponent() override = default;
    
    // ========================================================================
    // JUCE LIFECYCLE
    // ========================================================================
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // ========================================================================
    // MOUSE HANDLING
    // ========================================================================
    
    void mouseDown(const juce::MouseEvent& e) override;
    
    // ========================================================================
    // SETUP
    // ========================================================================
    
    void setAPVTS(juce::AudioProcessorValueTreeState* apvts);
    
    // ========================================================================
    // STATE UPDATES
    // ========================================================================
    
    void setInvertEnabled(bool enabled);
    
private:
    // ========================================================================
    // PRIVATE MEMBERS
    // ========================================================================
    
    juce::AudioProcessorValueTreeState* apvtsPtr{nullptr};
    bool currentInvertEnabled{false};
    
    // ========================================================================
    // PRIVATE METHODS
    // ========================================================================
    
    void updateParameter(bool newState);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InvertToggleComponent)
};
