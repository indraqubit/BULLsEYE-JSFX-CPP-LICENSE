#include "ModeSelectorComponent.h"
#include "../SSOT/UISSOT.h"

// ========================================================================
// CONSTRUCTOR
// ========================================================================

ModeSelectorComponent::ModeSelectorComponent()
{
    // Setup previous button
    prevButton.setButtonText("<");
    prevButton.onClick = [this] { onPrevButtonClicked(); };
    addAndMakeVisible(prevButton);
    
    // Setup next button
    nextButton.setButtonText(">");
    nextButton.onClick = [this] { onNextButtonClicked(); };
    addAndMakeVisible(nextButton);
    
    // Setup mode label
    modeLabel.setJustificationType(juce::Justification::centred);
    modeLabel.setFont(juce::FontOptions().withHeight(14.0f));
    modeLabel.setColour(juce::Label::textColourId, UISSOT::Colors::TEXT_PRIMARY());
    addAndMakeVisible(modeLabel);
    
    // Initial state
    setCurrentMode(0);
}

// ========================================================================
// SETUP
// ========================================================================

void ModeSelectorComponent::setAPVTS(juce::AudioProcessorValueTreeState* apvts)
{
    apvtsPtr = apvts;
}

// ========================================================================
// STATE UPDATES
// ========================================================================

void ModeSelectorComponent::setCurrentMode(int modeIndex)
{
    if (modeIndex >= 0 && modeIndex < ModelSSOT::MODE_COUNT)
    {
        currentModeIndex = modeIndex;
        updateButtons();
        updateModeLabel();
    }
}

// ========================================================================
// PRIVATE METHODS
// ========================================================================

void ModeSelectorComponent::updateButtons()
{
    // Disable buttons at boundaries
    prevButton.setEnabled(currentModeIndex > 0);
    nextButton.setEnabled(currentModeIndex < ModelSSOT::MODE_COUNT - 1);
}

void ModeSelectorComponent::updateModeLabel()
{
    const ModelSSOT::YourMode mode = static_cast<ModelSSOT::YourMode>(currentModeIndex);
    modeLabel.setText(ModelSSOT::Helpers::getModeName(mode), juce::dontSendNotification);
}

void ModeSelectorComponent::onPrevButtonClicked()
{
    if (apvtsPtr == nullptr || currentModeIndex <= 0)
        return;
    
    // Decrement mode
    const int newMode = currentModeIndex - 1;
    apvtsPtr->getParameter(ModelSSOT::ParameterIDs::YOUR_MODE)
        ->setValueNotifyingHost(static_cast<float>(newMode) / static_cast<float>(ModelSSOT::MODE_COUNT - 1));
}

void ModeSelectorComponent::onNextButtonClicked()
{
    if (apvtsPtr == nullptr || currentModeIndex >= ModelSSOT::MODE_COUNT - 1)
        return;
    
    // Increment mode
    const int newMode = currentModeIndex + 1;
    apvtsPtr->getParameter(ModelSSOT::ParameterIDs::YOUR_MODE)
        ->setValueNotifyingHost(static_cast<float>(newMode) / static_cast<float>(ModelSSOT::MODE_COUNT - 1));
}

// ========================================================================
// JUCE LIFECYCLE
// ========================================================================

void ModeSelectorComponent::paint(juce::Graphics& g)
{
    // Transparent background (parent handles it)
}

void ModeSelectorComponent::resized()
{
    const int buttonWidth = 40;
    const int buttonHeight = 30;
    const int labelWidth = 150;
    const int spacing = 10;
    
    juce::Rectangle<int> bounds = getLocalBounds();
    const int totalWidth = buttonWidth + spacing + labelWidth + spacing + buttonWidth;
    const int startX = (bounds.getWidth() - totalWidth) / 2;
    const int centerY = (bounds.getHeight() - buttonHeight) / 2;
    
    // Previous button
    prevButton.setBounds(startX, centerY, buttonWidth, buttonHeight);
    
    // Mode label (center)
    modeLabel.setBounds(startX + buttonWidth + spacing, centerY, labelWidth, buttonHeight);
    
    // Next button
    nextButton.setBounds(startX + buttonWidth + spacing + labelWidth + spacing, centerY, buttonWidth, buttonHeight);
}
