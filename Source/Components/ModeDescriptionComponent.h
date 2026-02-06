#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../SSOT/ModelSSOT.h"

/**
 * Mode Description Component
 * 
 * Text component displaying mode description.
 * TODO: Customize for your plugin
 */
class ModeDescriptionComponent : public juce::Component
{
public:
    // ========================================================================
    // CONSTRUCTOR / DESTRUCTOR
    // ========================================================================
    
    ModeDescriptionComponent();
    ~ModeDescriptionComponent() override = default;
    
    // ========================================================================
    // JUCE LIFECYCLE
    // ========================================================================
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // ========================================================================
    // STATE UPDATES
    // ========================================================================
    
    void setMode(ModelSSOT::YourMode mode);
    
private:
    // ========================================================================
    // PRIVATE STATE
    // ========================================================================
    
    ModelSSOT::YourMode currentMode{ModelSSOT::YourMode::ModeA};
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModeDescriptionComponent)
};
