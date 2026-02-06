# Component Template

Use this template when creating new UI components.

## Template

```cpp
#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../SSOT/UISSOT.h"
#include "../SSOT/ModelSSOT.h"

/**
 * YourComponentName
 * 
 * Description of what this component does.
 * TODO: Customize for your plugin
 */
class YourComponentName : public juce::Component
{
public:
    // ========================================================================
    // CONSTRUCTOR / DESTRUCTOR
    // ========================================================================
    
    YourComponentName();
    ~YourComponentName() override = default;
    
    // ========================================================================
    // JUCE LIFECYCLE
    // ========================================================================
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // ========================================================================
    // MOUSE HANDLING (if needed)
    // ========================================================================
    
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    void mouseEnter(const juce::MouseEvent& e) override;
    void mouseExit(const juce::MouseEvent& e) override;
    
    // ========================================================================
    // SETUP (if needed)
    // ========================================================================
    
    void setAPVTS(juce::AudioProcessorValueTreeState* apvts);
    
    // ========================================================================
    // STATE UPDATES
    // ========================================================================
    
    void setState(bool state);
    
private:
    // ========================================================================
    // PRIVATE MEMBERS
    // ========================================================================
    
    juce::AudioProcessorValueTreeState* apvtsPtr{nullptr};
    bool currentState{false};
    
    // ========================================================================
    // PRIVATE METHODS
    // ========================================================================
    
    void updateState();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(YourComponentName)
};
```

## Usage

1. Copy the template to your component file
2. Replace `YourComponentName` with your component name
3. Customize the constructor, paint, and state methods
4. Add to `CMakeLists.txt`
5. Add to `PluginEditor.h` and `PluginEditor.cpp`

## Checklist

- [ ] Component inherits from `juce::Component`
- [ ] Uses UISSOT colors and dimensions
- [ ] Implements proper mouse handling
- [ ] Updates on parameter changes
- [ ] Added to CMakeLists.txt
- [ ] Integrated in PluginEditor
