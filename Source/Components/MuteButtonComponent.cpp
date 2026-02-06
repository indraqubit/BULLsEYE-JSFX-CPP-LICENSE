#include "MuteButtonComponent.h"

// ========================================================================
// CONSTRUCTOR
// ========================================================================

MuteButtonComponent::MuteButtonComponent()
{
    setMouseCursor(juce::MouseCursor::PointingHandCursor);
}

// ========================================================================
// PAINT
// ========================================================================

void MuteButtonComponent::paint(juce::Graphics& g)
{
    const int diameter = UISSOT::Dimensions::BUTTON_DIAMETER;
    const int pressOffset = isMouseDown ? UISSOT::Dimensions::BUTTON_PRESS_OFFSET : 0;
    const int shadowOffset = UISSOT::Dimensions::SHADOW_OFFSET;
    
    // Get button bounds (centered)
    auto bounds = getLocalBounds();
    const int centerX = bounds.getCentreX();
    const int centerY = bounds.getCentreY();
    const int radius = diameter / 2;
    
    // Draw shadow
    g.setColour(juce::Colour::fromRGBA(0, 0, 0, 100));
    g.fillEllipse(centerX - radius + shadowOffset + pressOffset,
                  centerY - radius + shadowOffset + pressOffset,
                  diameter, diameter);
    
    // Draw button circle
    const juce::Colour buttonColour = getButtonColour();
    g.setColour(buttonColour);
    g.fillEllipse(centerX - radius + pressOffset,
                  centerY - radius + pressOffset,
                  diameter, diameter);
    
    // Draw highlight
    if (isMouseOver || isMouseDown)
    {
        const juce::Colour highlightColour = getHighlightColour();
        g.setColour(highlightColour.withAlpha(UISSOT::Dimensions::HIGHLIGHT_ALPHA));
        g.fillEllipse(centerX - radius + pressOffset,
                     centerY - radius + pressOffset,
                     diameter, diameter);
    }
    
    // Draw border
    g.setColour(juce::Colours::black.withAlpha(0.3f));
    g.drawEllipse(centerX - radius + pressOffset,
                  centerY - radius + pressOffset,
                  diameter, diameter, 2.0f);
    
    // Draw text
    const juce::String buttonText = getButtonText();
    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions().withHeight(16.0f));
    g.drawFittedText(buttonText,
                     centerX - radius + pressOffset,
                     centerY - radius + pressOffset,
                     diameter, diameter,
                     juce::Justification::centred, 1);
}

// ========================================================================
// RESIZED
// ========================================================================

void MuteButtonComponent::resized()
{
    // Component fills allocated bounds
}

// ========================================================================
// MOUSE HANDLING
// ========================================================================

void MuteButtonComponent::mouseEnter(const juce::MouseEvent& e)
{
    juce::ignoreUnused(e);
    isMouseOver = true;
    repaint();
}

void MuteButtonComponent::mouseDown(const juce::MouseEvent& e)
{
    juce::ignoreUnused(e);
    isMouseDown = true;
    
    // TODO: Implement mode-specific behavior
    updateParameter(!currentState);
    
    repaint();
}

void MuteButtonComponent::mouseUp(const juce::MouseEvent& e)
{
    juce::ignoreUnused(e);
    isMouseDown = false;
    repaint();
}

void MuteButtonComponent::mouseDrag(const juce::MouseEvent& e)
{
    juce::ignoreUnused(e);
    // Handle drag behavior if needed
}

void MuteButtonComponent::mouseExit(const juce::MouseEvent& e)
{
    juce::ignoreUnused(e);
    isMouseOver = false;
    
    if (isMouseDown)
    {
        isMouseDown = false;
        repaint();
    }
}

// ========================================================================
// SETUP
// ========================================================================

void MuteButtonComponent::setAPVTS(juce::AudioProcessorValueTreeState* apvts)
{
    apvtsPtr = apvts;
}

// ========================================================================
// STATE UPDATES
// ========================================================================

void MuteButtonComponent::setState(bool isActive)
{
    if (currentState != isActive)
    {
        currentState = isActive;
        repaint();
    }
}

void MuteButtonComponent::setMode(ModelSSOT::YourMode mode)
{
    if (currentMode != mode)
    {
        currentMode = mode;
        repaint();
    }
}

// ========================================================================
// PRIVATE METHODS
// ========================================================================

juce::Colour MuteButtonComponent::getButtonColour() const
{
    // TODO: Implement your color logic
    return currentState ? UISSOT::Colors::PRIMARY_ON() : UISSOT::Colors::SECONDARY_OFF();
}

juce::Colour MuteButtonComponent::getHighlightColour() const
{
    // TODO: Implement your highlight logic
    return currentState ? UISSOT::Colors::PRIMARY_HL() : UISSOT::Colors::SECONDARY_HL();
}

juce::String MuteButtonComponent::getButtonText() const
{
    // TODO: Implement your text logic
    return currentState ? "ON" : "OFF";
}

void MuteButtonComponent::updateParameter(bool newState)
{
    if (apvtsPtr != nullptr)
    {
        // TODO: Update your parameter
        // apvtsPtr->getParameter(ModelSSOT::ParameterIDs::YOUR_PARAM)
        //     ->setValueNotifyingHost(newState ? 1.0f : 0.0f);
    }
}
