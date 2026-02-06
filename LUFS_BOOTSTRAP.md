Mode brutal ringkas ON.

```markdown
# Cursor Prompt: Integrate Calibrated LED Strip LUFS Meter (Universal)

## Context
Add calibrated LED strip LUFS meter to existing JUCE plugin with deferred bootstrap pattern. Works with any existing meter implementation (or none). Calibration is hardcoded at compile time.

## Configuration (EDIT BEFORE APPLYING)

Set your calibration values here:

```cpp
// CALIBRATION SETTINGS - Edit these values
const float LUFS_FLOOR = -60.0f;      // Meter floor level
const float LUFS_TARGET = -14.0f;     // Target LUFS (Streaming: -14, Broadcast: -23, Cinema: -27)
const float LUFS_WARNING = -9.0f;     // Warning threshold (typically target + 5dB)
const float LUFS_CEILING = 0.0f;      // Meter ceiling (0 dB)

// LED COUNT
const int NUM_LEDS = 20;              // Number of LED segments (10-40 recommended)
```

Common presets:
- Streaming (Spotify/YouTube): TARGET=-14, WARNING=-9
- Broadcast (EBU R128): TARGET=-23, WARNING=-18
- Cinema (Netflix): TARGET=-27, WARNING=-22
- Mastering: TARGET=-12, WARNING=-6

## Files to create

### 1. Source/LEDStripMeter.h

```cpp
#pragma once
#include <JuceHeader.h>

class LEDStripMeter : public juce::Component, public juce::Timer
{
public:
    // HARDCODED CALIBRATION - Edit these values
    static constexpr float FLOOR = -60.0f;
    static constexpr float TARGET = -14.0f;
    static constexpr float WARNING = -9.0f;
    static constexpr float CEILING = 0.0f;
    static constexpr int NUM_LEDS = 20;
    
    LEDStripMeter() 
    { 
        startTimerHz(30);
    }
    
    void setLevel(float newLevel) 
    {
        targetLevel = juce::jlimit(0.0f, 1.0f, newLevel);
    }
    
    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        
        // Background
        g.fillAll(juce::Colour(0xff1a1a1a));
        
        // Current LUFS value
        float lufsDB = juce::jmap(currentLevel, 0.0f, 1.0f, FLOOR, CEILING);
        g.setColour(juce::Colours::white);
        g.setFont(16.0f);
        g.drawText(juce::String(lufsDB, 1) + " LUFS", 
                   10, 5, 120, 25, juce::Justification::centredLeft);
        
        // Target reference
        g.setFont(12.0f);
        g.setColour(juce::Colours::grey);
        g.drawText("Target: " + juce::String(TARGET, 1), 
                   bounds.getWidth() - 110, 8, 100, 20,
                   juce::Justification::centredRight);
        
        // LED strip
        auto ledArea = bounds.withTrimmedTop(35).reduced(10, 5);
        float ledWidth = ledArea.getWidth() / NUM_LEDS;
        float ledHeight = ledArea.getHeight();
        
        for (int i = 0; i < NUM_LEDS; i++)
        {
            float ledThreshold = i / (float)NUM_LEDS;
            float ledLUFS = juce::jmap((float)i / NUM_LEDS, 0.0f, 1.0f, FLOOR, CEILING);
            float ledX = ledArea.getX() + (i * ledWidth);
            
            // Calibrated color zones
            juce::Colour ledColour;
            if (ledLUFS < TARGET - 6.0f)
                ledColour = juce::Colour(0xff00ff00);      // GREEN (safe)
            else if (ledLUFS < TARGET)
                ledColour = juce::Colour(0xff88ff00);      // LIGHT GREEN (good)
            else if (ledLUFS < WARNING)
                ledColour = juce::Colour(0xffffff00);      // YELLOW (loud)
            else if (ledLUFS < -3.0f)
                ledColour = juce::Colour(0xffff8800);      // ORANGE (very loud)
            else
                ledColour = juce::Colour(0xffff0000);      // RED (danger)
            
            auto ledRect = juce::Rectangle<float>(
                ledX + 2, ledArea.getY(), ledWidth - 4, ledHeight
            );
            
            bool isActive = currentLevel >= ledThreshold;
            float brightness = isActive ? 1.0f : 0.15f;
            
            g.setColour(ledColour.withAlpha(brightness));
            g.fillRoundedRectangle(ledRect, 3.0f);
            
            // Shine on active LEDs
            if (isActive)
            {
                g.setColour(juce::Colours::white.withAlpha(0.2f));
                g.fillRoundedRectangle(ledRect.withHeight(ledHeight * 0.3f), 2.0f);
            }
        }
        
        // Target marker line
        float targetNormalized = juce::jmap(TARGET, FLOOR, CEILING, 0.0f, 1.0f);
        float targetX = ledArea.getX() + (targetNormalized * ledArea.getWidth());
        
        g.setColour(juce::Colours::cyan.withAlpha(0.6f));
        g.drawLine(targetX, ledArea.getY() - 3, targetX, ledArea.getBottom() + 3, 2.0f);
        
        // Target label
        g.setFont(9.0f);
        g.drawText(juce::String(TARGET, 0), 
                   targetX - 12, ledArea.getBottom() + 5, 24, 12,
                   juce::Justification::centred);
    }
    
    void timerCallback() override
    {
        currentLevel += (targetLevel - currentLevel) * 0.25f;
        if (std::abs(targetLevel - currentLevel) > 0.001f)
            repaint();
    }
    
    void resized() override {}

private:
    float currentLevel = 0.0f;
    float targetLevel = 0.0f;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LEDStripMeter)
};
```

## Files to modify

### 2. PluginProcessor.h

Add or verify these exist:

```cpp
// LUFS (preferred - if available):
std::atomic<float> currentLUFS{-23.0f};
float getLUFSLevel() const { return currentLUFS.load(); }

// Normalized getter (REQUIRED for meter):
float getLUFSNormalized() const {
    float lufs = currentLUFS.load();
    // Use same FLOOR/CEILING as meter calibration
    return juce::jmap(lufs, -60.0f, 0.0f, 0.0f, 1.0f);
}

// OR if LUFS not available, use RMS fallback:
std::atomic<float> currentRMS{0.0f};
float getRMSLevel() const { return currentRMS.load(); }
```

### 3. PluginProcessor.cpp

In `processBlock()`:

```cpp
// LUFS (if available):
float lufs = calculateLUFS(buffer); // your existing function
currentLUFS.store(lufs);

// OR RMS fallback:
float rms = 0.0f;
for (int ch = 0; ch < buffer.getNumChannels(); ch++) {
    auto* data = buffer.getReadPointer(ch);
    for (int i = 0; i < buffer.getNumSamples(); i++)
        rms += data[i] * data[i];
}
rms = std::sqrt(rms / (buffer.getNumSamples() * buffer.getNumChannels()));
currentRMS.store(rms);
```

### 4. PluginEditor.h

```cpp
// Add Timer inheritance (if not present):
class PluginEditor : public juce::AudioProcessorEditor,
                      public juce::Timer
{
public:
    void timerCallback() override; // add if missing

private:
    std::unique_ptr<LEDStripMeter> ledMeter;
    bool guiReady = false;
    void bootstrapGui();
    
    // Keep all existing members
};
```

### 5. PluginEditor.cpp

**Constructor - add at END:**
```cpp
startTimer(1); // bootstrap trigger
```

**Add timerCallback():**
```cpp
void PluginEditor::timerCallback()
{
    if (!guiReady) {
        bootstrapGui();
        stopTimer();
        startTimerHz(30);
        return;
    }
    
    if (ledMeter) {
        // Use LUFS if available:
        ledMeter->setLevel(processorRef.getLUFSNormalized());
        
        // OR use RMS if LUFS unavailable:
        // ledMeter->setLevel(processorRef.getRMSLevel());
    }
}
```

**Add bootstrapGui():**
```cpp
void PluginEditor::bootstrapGui()
{
    ledMeter = std::make_unique<LEDStripMeter>();
    ledMeter->setBounds(20, 50, getWidth() - 40, 80);
    addAndMakeVisible(*ledMeter);
    
    guiReady = true;
    repaint();
}
```

**paint() - add at START:**
```cpp
if (!guiReady) {
    g.fillAll(juce::Colours::black);
    g.setColour(juce::Colours::white);
    g.drawText("Loading...", getLocalBounds(), juce::Justification::centred);
    return;
}
```

**resized() - add at END:**
```cpp
if (ledMeter)
    ledMeter->setBounds(20, 50, getWidth() - 40, 80);
```

**Destructor - add at START:**
```cpp
ledMeter.reset();
```

## Calibration Guide

Edit these constants in LEDStripMeter.h to match your use case:

### Streaming (Spotify/YouTube/Apple Music):
```cpp
static constexpr float FLOOR = -60.0f;
static constexpr float TARGET = -14.0f;
static constexpr float WARNING = -9.0f;
static constexpr float CEILING = 0.0f;
```

### Broadcast (EBU R128 / TV):
```cpp
static constexpr float FLOOR = -60.0f;
static constexpr float TARGET = -23.0f;
static constexpr float WARNING = -18.0f;
static constexpr float CEILING = 0.0f;
```

### Cinema (Netflix / Film):
```cpp
static constexpr float FLOOR = -70.0f;
static constexpr float TARGET = -27.0f;
static constexpr float WARNING = -22.0f;
static constexpr float CEILING = 0.0f;
```

### Mastering:
```cpp
static constexpr float FLOOR = -60.0f;
static constexpr float TARGET = -12.0f;
static constexpr float WARNING = -6.0f;
static constexpr float CEILING = 0.0f;
```

## Color Zone Logic

Based on calibration:
- **Below target - 6dB**: GREEN (safe zone)
- **Target - 6dB to target**: LIGHT GREEN (good zone)
- **Target to warning**: YELLOW (loud zone)
- **Warning to -3dB**: ORANGE (very loud)
- **-3dB to ceiling**: RED (danger/limiting)

Target marker (cyan line) automatically positioned based on TARGET constant.

## Detection & Adaptation

- If LUFS exists: use `getLUFSNormalized()`
- If only RMS exists: use `getRMSLevel()` (meter shows RMS instead)
- If no meter exists: create from scratch
- If existing meter present: add alongside (don't remove old code)

## Rules

- PRESERVE all existing code
- Use processor reference name that exists (processorRef/processor/audioProcessor)
- LUFS range must match calibration constants (FLOOR to CEILING)
- If editor already has Timer, merge timerCallback logic
- Position meter in available space (default: 20,50,width-40,80)

## Expected Result

1. Builds without errors
2. Shows "Loading..." for 1 frame
3. LED meter appears calibrated to your target
4. Target marker line visible
5. Color zones relative to target
6. Smooth 30fps animation
7. Current value + target reference displayed

Apply changes. Calibration is hardcoded - edit constants before compiling.
```

---

## Quick calibration reference card:

```cpp
// ADD THIS COMMENT BLOCK to LEDStripMeter.h for easy editing:

/*
 * CALIBRATION PRESETS
 * 
 * Streaming (Spotify/YouTube):  TARGET=-14, WARNING=-9,  FLOOR=-60
 * Broadcast (EBU R128):         TARGET=-23, WARNING=-18, FLOOR=-60
 * Cinema (Netflix):             TARGET=-27, WARNING=-22, FLOOR=-70
 * Mastering:                    TARGET=-12, WARNING=-6,  FLOOR=-60
 * 
 * Edit the constants below to match your use case.
 */

static constexpr float FLOOR = -60.0f;    // ← EDIT HERE
static constexpr float TARGET = -14.0f;   // ← EDIT HERE
static constexpr float WARNING = -9.0f;   // ← EDIT HERE
static constexpr float CEILING = 0.0f;    // ← EDIT HERE
static constexpr int NUM_LEDS = 20;       // ← EDIT HERE
```

---

**Prompt siap pakai. Lo tinggal:**
1. Paste ke Cursor
2. Edit calibration values di hasil generate
3. Build

Done.