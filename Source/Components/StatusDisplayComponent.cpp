#include "StatusDisplayComponent.h"

// ========================================================================
// CONSTRUCTOR
// ========================================================================

StatusDisplayComponent::StatusDisplayComponent()
{
}

// ========================================================================
// PAINT
// ========================================================================

void StatusDisplayComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    // Background panel (subtle)
    g.setColour(UISSOT::Colors::PANEL_BG().withAlpha(0.4f));
    g.fillRoundedRectangle(bounds.toFloat(), 4.0f);
    
    // Get status text and color
    const juce::String statusText = UISSOT::Helpers::getStatusText(currentStatus);
    const juce::Colour statusColour = UISSOT::Helpers::getStatusColour(currentStatus);
    
    // Status text
    g.setColour(statusColour);
    juce::Font statusFont = juce::FontOptions().withHeight(13.0f);
    statusFont.setBold(true);
    g.setFont(statusFont);
    g.drawFittedText(statusText, bounds, juce::Justification::centred, 1);
}

// ========================================================================
// RESIZED
// ========================================================================

void StatusDisplayComponent::resized()
{
    // Component fills allocated bounds
}

// ========================================================================
// STATE UPDATES
// ========================================================================

void StatusDisplayComponent::setStatus(bool isActive)
{
    if (currentStatus != isActive)
    {
        currentStatus = isActive;
        repaint();
    }
}
