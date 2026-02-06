#include "ModeDescriptionComponent.h"
#include "../SSOT/UISSOT.h"

// ========================================================================
// CONSTRUCTOR
// ========================================================================

ModeDescriptionComponent::ModeDescriptionComponent()
{
}

// ========================================================================
// PAINT
// ========================================================================

void ModeDescriptionComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    // Get mode description
    const juce::String description = ModelSSOT::Helpers::getModeDescription(currentMode);
    
    // Background panel
    g.setColour(UISSOT::Colors::PANEL_BG().withAlpha(0.3f));
    g.fillRoundedRectangle(bounds.toFloat(), 4.0f);
    
    // Description text
    g.setColour(UISSOT::Colors::TEXT_SECONDARY());
    g.setFont(juce::FontOptions().withHeight(11.5f));
    g.drawFittedText(description,
                     bounds,
                     juce::Justification::centred,
                     3);
}

// ========================================================================
// RESIZED
// ========================================================================

void ModeDescriptionComponent::resized()
{
    // Component fills allocated bounds
}

// ========================================================================
// STATE UPDATES
// ========================================================================

void ModeDescriptionComponent::setMode(ModelSSOT::YourMode mode)
{
    if (currentMode != mode)
    {
        currentMode = mode;
        repaint();
    }
}
