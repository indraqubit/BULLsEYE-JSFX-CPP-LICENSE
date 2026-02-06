#include "InvertToggleComponent.h"

// ========================================================================
// CONSTRUCTOR
// ========================================================================

InvertToggleComponent::InvertToggleComponent()
{
    setMouseCursor(juce::MouseCursor::PointingHandCursor);
}

// ========================================================================
// PAINT
// ========================================================================

void InvertToggleComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    // Draw label
    g.setColour(UISSOT::Colors::TEXT_PRIMARY());
    g.setFont(juce::FontOptions().withHeight(13.0f));
    const int labelWidth = 60;
    g.drawFittedText("Invert:",
                     bounds.removeFromLeft(labelWidth),
                     juce::Justification::centredLeft, 1);
    
    // Draw toggle switch area
    const int switchWidth = 50;
    const int switchHeight = 24;
    const int switchX = bounds.getX();
    const int switchY = (bounds.getHeight() - switchHeight) / 2;
    
    // Background
    const juce::Colour bgColour = currentInvertEnabled 
        ? UISSOT::Colors::PRIMARY_ON() 
        : UISSOT::Colors::NEUTRAL();
    g.setColour(bgColour);
    g.fillRoundedRectangle(switchX, switchY, switchWidth, switchHeight, switchHeight / 2.0f);
    
    // Border
    g.setColour(juce::Colours::black.withAlpha(0.3f));
    g.drawRoundedRectangle(switchX, switchY, switchWidth, switchHeight, switchHeight / 2.0f, 1.5f);
    
    // Toggle circle
    const int circleSize = switchHeight - 4;
    const int circleY = switchY + 2;
    const int circleX = currentInvertEnabled 
        ? switchX + switchWidth - circleSize - 2
        : switchX + 2;
    
    g.setColour(juce::Colours::white);
    g.fillEllipse(circleX, circleY, circleSize, circleSize);
    
    // Circle border
    g.setColour(juce::Colours::black.withAlpha(0.2f));
    g.drawEllipse(circleX, circleY, circleSize, circleSize, 1.0f);
    
    // ON/OFF text
    g.setColour(UISSOT::Colors::TEXT_SECONDARY());
    g.setFont(juce::FontOptions().withHeight(10.5f));
    const juce::String indicatorText = currentInvertEnabled ? "ON" : "OFF";
    g.drawFittedText(indicatorText,
                     switchX + switchWidth + 10,
                     switchY,
                     40,
                     switchHeight,
                     juce::Justification::centredLeft, 1);
}

// ========================================================================
// RESIZED
// ========================================================================

void InvertToggleComponent::resized()
{
    // Component fills allocated bounds
}

// ========================================================================
// MOUSE HANDLING
// ========================================================================

void InvertToggleComponent::mouseDown(const juce::MouseEvent& e)
{
    juce::ignoreUnused(e);
    updateParameter(!currentInvertEnabled);
}

// ========================================================================
// SETUP
// ========================================================================

void InvertToggleComponent::setAPVTS(juce::AudioProcessorValueTreeState* apvts)
{
    apvtsPtr = apvts;
}

// ========================================================================
// STATE UPDATES
// ========================================================================

void InvertToggleComponent::setInvertEnabled(bool enabled)
{
    if (currentInvertEnabled != enabled)
    {
        currentInvertEnabled = enabled;
        repaint();
    }
}

// ========================================================================
// PRIVATE METHODS
// ========================================================================

void InvertToggleComponent::updateParameter(bool newState)
{
    if (apvtsPtr != nullptr)
    {
        apvtsPtr->getParameter(ModelSSOT::ParameterIDs::INVERT_ENABLED)
            ->setValueNotifyingHost(newState ? 1.0f : 0.0f);
    }
}
