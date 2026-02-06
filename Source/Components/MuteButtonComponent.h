#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "../SSOT/UISSOT.h"
#include "../SSOT/ModelSSOT.h"

/**
 * Main Button Component
 * 
 * Interactive button with visual states and mode-specific behavior.
 * TODO: Customize for your plugin
 */
class MuteButtonComponent : public juce::Component
{
public:
    // ========================================================================
    // CONSTRUCTOR / DESTRUCTOR
    // ========================================================================
    
    MuteButtonComponent();
    ~MuteButtonComponent() override = default;
    
    // ========================================================================
    // JUCE LIFECYCLE
    // ========================================================================
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // ========================================================================
    // MOUSE HANDLING
    // ========================================================================
    
    void mouseEnter(const juce::MouseEvent& e) override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseExit(const juce::MouseEvent& e) override;
    
    // ========================================================================
    // SETUP
    // ========================================================================
    
    void setAPVTS(juce::AudioProcessorValueTreeState* apvts);
    
    // ========================================================================
    // STATE UPDATES
    // ========================================================================
    
    void setState(bool isActive);
    void setMode(ModelSSOT::YourMode mode);
    
private:
    // ========================================================================
    // PRIVATE MEMBERS
    // ========================================================================
    
    juce::AudioProcessorValueTreeState* apvtsPtr{nullptr};
    
    bool currentState{true};
    ModelSSOT::YourMode currentMode{ModelSSOT::YourMode::ModeA};
    
    // Mouse state
    bool isMouseDown{false};
    bool isMouseOver{false};
    
    // ========================================================================
    // PRIVATE METHODS
    // ========================================================================
    
    juce::Colour getButtonColour() const;
    juce::Colour getHighlightColour() const;
    juce::String getButtonText() const;
    void updateParameter(bool newState);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MuteButtonComponent)
};
