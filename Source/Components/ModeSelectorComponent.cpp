#include "ModeSelectorComponent.h"

// ========================================================================
// CONSTRUCTOR
// ========================================================================

ModeSelectorComponent::ModeSelectorComponent()
{
    // Setup label
    modeLabel.setText(UISSOT::Strings::contentTypeLabel(),
                      juce::NotificationType::dontSendNotification);
    modeLabel.setFont(UISSOT::Typography::labelFont());
    modeLabel.setColour(juce::Label::textColourId, UISSOT::Colors::textSecondary());
    addAndMakeVisible(modeLabel);

    // Setup combo box
    modeComboBox.addItem(ModelSSOT::Helpers::contentTypeToString(
        ModelSSOT::ContentType::MusicNonDrums), 1);
    modeComboBox.addItem(ModelSSOT::Helpers::contentTypeToString(
        ModelSSOT::ContentType::MusicDrums), 2);
    modeComboBox.addItem(ModelSSOT::Helpers::contentTypeToString(
        ModelSSOT::ContentType::CinemaTrailer), 3);
    modeComboBox.setSelectedId(2);  // Default: Music Drums
    modeComboBox.addListener(this);
    addAndMakeVisible(modeComboBox);
}

// ========================================================================
// SETUP
// ========================================================================

void ModeSelectorComponent::setAPVTS(juce::AudioProcessorValueTreeState* apvts)
{
    apvtsPtr = apvts;

    // Restore saved value if available
    if (apvtsPtr != nullptr)
    {
        auto* param = apvtsPtr->getRawParameterValue(ModelSSOT::ParameterIDs::CONTENT_TYPE);
        if (param != nullptr)
        {
            int savedValue = static_cast<int>(param->load());
            modeComboBox.setSelectedId(savedValue + 1);  // +1 because IDs are 1-indexed
            currentModeIndex = savedValue;
        }
    }
}

void ModeSelectorComponent::updateModeLabel()
{
    modeLabel.setText(UISSOT::Strings::contentTypeLabel(),
                      juce::NotificationType::dontSendNotification);
}

// ========================================================================
// STATE UPDATES
// ========================================================================

void ModeSelectorComponent::setCurrentMode(int modeIndex)
{
    modeComboBox.setSelectedId(modeIndex + 1);
    currentModeIndex = modeIndex;
}

void ModeSelectorComponent::onModeChanged(int newMode)
{
    currentModeIndex = newMode;

    // Update APVTS if connected
    // Fixes DI-1: use setValueNotifyingHost() for proper undo/redo and host automation
    if (apvtsPtr != nullptr)
    {
        auto* param = apvtsPtr->getParameter(ModelSSOT::ParameterIDs::CONTENT_TYPE);
        if (param != nullptr)
        {
            param->beginChangeGesture();
            param->setValueNotifyingHost(static_cast<float>(newMode) / 2.0f);  // Normalize to 0-1
            param->endChangeGesture();
        }
    }
}

// ========================================================================
// JUCE LIFECYCLE
// ========================================================================

void ModeSelectorComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();

    // Background
    g.fillAll(UISSOT::Colors::backgroundLight());
}

void ModeSelectorComponent::resized()
{
    auto bounds = getLocalBounds();
    bounds.reduce(UISSOT::Dimensions::MARGIN_SMALL, UISSOT::Dimensions::MARGIN_SMALL);

    // Label at top
    modeLabel.setBounds(bounds.removeFromTop(20));

    // Combo box below label
    modeComboBox.setBounds(bounds.removeFromTop(28));
}

// ========================================================================
// COMBOBOX LISTENER
// ========================================================================

void ModeSelectorComponent::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == &modeComboBox)
    {
        int newMode = modeComboBox.getSelectedId() - 1;  // Convert from 1-indexed
        onModeChanged(newMode);
    }
}
