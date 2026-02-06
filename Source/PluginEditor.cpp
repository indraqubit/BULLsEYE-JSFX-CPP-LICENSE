#include "PluginProcessor.h"
#include "PluginEditor.h"

// ========================================================================
// CONSTRUCTOR
// ========================================================================

YourEditor::YourEditor(YourProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Use dimensions from UISSOT
    setSize(UISSOT::Dimensions::PANEL_WIDTH, UISSOT::Dimensions::PANEL_HEIGHT);
    
    // Add components
    // TODO: Add your components
    addAndMakeVisible(statusDisplay);
    addAndMakeVisible(modeSelector);
    addAndMakeVisible(muteButton);
    addAndMakeVisible(invertToggle);
    addAndMakeVisible(modeDescription);
    
    // Set APVTS for components
    // TODO: Set APVTS for your components
    modeSelector.setAPVTS(&audioProcessor.getAPVTS());
    muteButton.setAPVTS(&audioProcessor.getAPVTS());
    invertToggle.setAPVTS(&audioProcessor.getAPVTS());
    
    // Connect to APVTS - listen to parameters
    // TODO: Add your parameter listeners
    audioProcessor.getAPVTS().addParameterListener(ModelSSOT::ParameterIDs::MUTE_STATE, this);
    audioProcessor.getAPVTS().addParameterListener(ModelSSOT::ParameterIDs::MUTE_MODE, this);
    audioProcessor.getAPVTS().addParameterListener(ModelSSOT::ParameterIDs::INVERT_ENABLED, this);
    
    // Initial update
    // TODO: Update your components
    updateStatusDisplay();
    updateModeSelector();
    updateMuteButton();
    updateInvertToggle();
    updateModeDescription();
}

// ========================================================================
// DESTRUCTOR
// ========================================================================

YourEditor::~YourEditor()
{
    // Remove listeners
    // TODO: Remove your parameter listeners
    audioProcessor.getAPVTS().removeParameterListener(ModelSSOT::ParameterIDs::MUTE_STATE, this);
    audioProcessor.getAPVTS().removeParameterListener(ModelSSOT::ParameterIDs::MUTE_MODE, this);
    audioProcessor.getAPVTS().removeParameterListener(ModelSSOT::ParameterIDs::INVERT_ENABLED, this);
}

// ========================================================================
// PAINT
// ========================================================================

void YourEditor::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    // Professional gradient background
    juce::ColourGradient gradient(
        UISSOT::Colors::BACKGROUND_DARK(),
        0.0f, 0.0f,
        UISSOT::Colors::BACKGROUND_MID(),
        0.0f, static_cast<float>(bounds.getHeight()),
        false
    );
    g.setGradientFill(gradient);
    g.fillAll();
    
    // Header section
    auto headerBounds = bounds.removeFromTop(UISSOT::Dimensions::HEADER_HEIGHT);
    
    // Header background
    g.setColour(UISSOT::Colors::PANEL_BG().withAlpha(0.6f));
    g.fillRoundedRectangle(headerBounds.toFloat(), 0.0f);
    
    // Divider line
    g.setColour(UISSOT::Colors::DIVIDER());
    g.fillRect(headerBounds.getX(), headerBounds.getBottom() - 1, 
               headerBounds.getWidth(), 1);
    
    // Title
    g.setColour(UISSOT::Colors::TEXT_PRIMARY());
    juce::Font titleFont = juce::FontOptions().withHeight(UISSOT::Dimensions::TITLE_FONT_SIZE);
    titleFont.setBold(true);
    g.setFont(titleFont);
    auto titleBounds = headerBounds.removeFromTop(headerBounds.getHeight() - 4);
    g.drawFittedText("Your Plugin Name", titleBounds, 
                     juce::Justification::centred, 1);
    
    // Brand
    g.setColour(UISSOT::Colors::TEXT_TERTIARY());
    g.setFont(juce::FontOptions().withHeight(UISSOT::Dimensions::BRAND_FONT_SIZE));
    auto brandBounds = headerBounds;
    brandBounds.removeFromTop(2);
    g.drawFittedText("Your Company", brandBounds,
                     juce::Justification::centred, 1);
}

// ========================================================================
// RESIZED
// ========================================================================

void YourEditor::resized()
{
    auto bounds = getLocalBounds();
    
    // Remove header area
    bounds.removeFromTop(UISSOT::Dimensions::HEADER_HEIGHT);
    bounds.removeFromTop(UISSOT::Dimensions::PADDING_MEDIUM);
    
    // TODO: Layout your components
    // Status display
    statusDisplay.setBounds(bounds.removeFromTop(UISSOT::Dimensions::STATUS_HEIGHT));
    bounds.removeFromTop(UISSOT::Dimensions::SPACING_COMPONENT);
    
    // Mode selector
    modeSelector.setBounds(bounds.removeFromTop(UISSOT::Dimensions::MODE_SELECTOR_HEIGHT));
    bounds.removeFromTop(UISSOT::Dimensions::SPACING_SECTION);
    
    // Main button (centered)
    const int buttonSize = UISSOT::Dimensions::BUTTON_DIAMETER + 20;
    const int buttonX = (bounds.getWidth() - buttonSize) / 2;
    muteButton.setBounds(buttonX, bounds.getY(), buttonSize, buttonSize);
    
    // Remaining area
    const int buttonBottom = bounds.getY() + buttonSize;
    auto remainingBounds = bounds.withTop(buttonBottom + UISSOT::Dimensions::SPACING_SECTION);
    
    // Invert toggle (centered)
    const int toggleWidth = 200;
    const int toggleX = (remainingBounds.getWidth() - toggleWidth) / 2;
    invertToggle.setBounds(toggleX, remainingBounds.getY(), 
                          toggleWidth, UISSOT::Dimensions::TOGGLE_HEIGHT);
    
    // Mode description
    const int descWidth = remainingBounds.getWidth() - (UISSOT::Dimensions::PADDING_LARGE * 2);
    const int descX = remainingBounds.getX() + UISSOT::Dimensions::PADDING_LARGE;
    modeDescription.setBounds(descX, invertToggle.getY() + UISSOT::Dimensions::TOGGLE_HEIGHT + UISSOT::Dimensions::SPACING_COMPONENT,
                             descWidth, UISSOT::Dimensions::DESCRIPTION_HEIGHT);
}

// ========================================================================
// PARAMETER CHANGED
// ========================================================================

void YourEditor::parameterChanged(const juce::String& parameterID, float newValue)
{
    juce::ignoreUnused(newValue);
    
    // Update components when parameters change
    // TODO: Update your components
    updateStatusDisplay();
    updateModeSelector();
    updateMuteButton();
    updateInvertToggle();
    updateModeDescription();
    
    repaint();
}

// ========================================================================
// HELPER METHODS
// ========================================================================

void YourEditor::updateStatusDisplay()
{
    // TODO: Implement
    const bool muteState = *audioProcessor.getAPVTS().getRawParameterValue(ModelSSOT::ParameterIDs::MUTE_STATE) > 0.5f;
    const bool invertEnabled = *audioProcessor.getAPVTS().getRawParameterValue(ModelSSOT::ParameterIDs::INVERT_ENABLED) > 0.5f;
    statusDisplay.setMuteState(muteState, invertEnabled);
}

void YourEditor::updateModeSelector()
{
    // TODO: Implement
    const int modeIndex = static_cast<int>(*audioProcessor.getAPVTS().getRawParameterValue(ModelSSOT::ParameterIDs::MUTE_MODE));
    modeSelector.setCurrentMode(modeIndex);
}

void YourEditor::updateMuteButton()
{
    // TODO: Implement
    const bool muteState = *audioProcessor.getAPVTS().getRawParameterValue(ModelSSOT::ParameterIDs::MUTE_STATE) > 0.5f;
    const int modeIndex = static_cast<int>(*audioProcessor.getAPVTS().getRawParameterValue(ModelSSOT::ParameterIDs::MUTE_MODE));
    const bool invertEnabled = *audioProcessor.getAPVTS().getRawParameterValue(ModelSSOT::ParameterIDs::INVERT_ENABLED) > 0.5f;
    
    muteButton.setMuteState(muteState);
    muteButton.setMode(static_cast<ModelSSOT::YourMode>(modeIndex));
    muteButton.setInvertEnabled(invertEnabled);
}

void YourEditor::updateInvertToggle()
{
    // TODO: Implement
    const bool invertEnabled = *audioProcessor.getAPVTS().getRawParameterValue(ModelSSOT::ParameterIDs::INVERT_ENABLED) > 0.5f;
    invertToggle.setInvertEnabled(invertEnabled);
}

void YourEditor::updateModeDescription()
{
    // TODO: Implement
    const int modeIndex = static_cast<int>(*audioProcessor.getAPVTS().getRawParameterValue(ModelSSOT::ParameterIDs::MUTE_MODE));
    modeDescription.setMode(static_cast<ModelSSOT::YourMode>(modeIndex));
}
