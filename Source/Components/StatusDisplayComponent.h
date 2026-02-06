#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../SSOT/UISSOT.h"
#include "../SSOT/ModelSSOT.h"

/**
 * Status Display Component
 * 
 * Displays current status with text and color.
 * TODO: Customize for your plugin
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
    
    void setStatus(bool isActive);
    
private:
    // ========================================================================
    // PRIVATE STATE
    // ========================================================================
    
    bool currentStatus{false};
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StatusDisplayComponent)
};
