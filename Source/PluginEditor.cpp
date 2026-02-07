#include "PluginEditor.h"

// ========================================================================
// CONSTRUCTOR / DESTRUCTOR
// ========================================================================

BULLsEYEEditor::BULLsEYEEditor(BULLsEYEProcessor& p)
    : juce::AudioProcessorEditor(p)
    , audioProcessor(p)
{
    setSize(UISSOT::Dimensions::DEFAULT_WINDOW_WIDTH,
            UISSOT::Dimensions::DEFAULT_WINDOW_HEIGHT);

    // Setup components
    addAndMakeVisible(statusDisplay);
    addAndMakeVisible(modeSelector);
    addAndMakeVisible(circularMeter);

    // Connect mode selector to APVTS
    modeSelector.setAPVTS(&audioProcessor.getAPVTS());

    // Start timer for meter updates
    startTimerHz(UISSOT::Timing::UI_REFRESH_RATE_HZ);
}

BULLsEYEEditor::~BULLsEYEEditor()
{
    stopTimer();
}

// ========================================================================
// JUCE LIFECYCLE
// ========================================================================

void BULLsEYEEditor::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Background
    g.fillAll(UISSOT::Colors::background());

    // Header - just plugin name
    g.setFont(UISSOT::Typography::headerFont());
    g.setColour(UISSOT::Colors::textPrimary());
    g.drawText(UISSOT::Strings::pluginName(), bounds.removeFromTop(UISSOT::Dimensions::HEADER_HEIGHT),
               juce::Justification::centred);
}

void BULLsEYEEditor::resized()
{
    auto bounds = getLocalBounds();

    // Header (painted in paint(), reserve space)
    bounds.removeFromTop(UISSOT::Dimensions::HEADER_HEIGHT);

    // Mode selector (label 20px + combo 28px + padding 8px = 56px)
    bounds.removeFromTop(UISSOT::Dimensions::MARGIN_SMALL);
    modeSelector.setBounds(bounds.removeFromTop(56));
    bounds.removeFromTop(UISSOT::Dimensions::MARGIN_SMALL);

    // Status display (LUFS-I, deviation bar, True Peak)
    statusDisplay.setBounds(bounds.removeFromTop(120));
    bounds.removeFromTop(UISSOT::Dimensions::MARGIN_SMALL);

    // Circular meter fills remaining space
    circularMeter.setBounds(bounds);
}

// ========================================================================
// TIMER (meter updates)
// ========================================================================

void BULLsEYEEditor::timerCallback()
{
    updateStatusDisplay();
    updateCircularMeter();
}

void BULLsEYEEditor::updateStatusDisplay()
{
    statusDisplay.setValues(
        audioProcessor.getIntegratedLUFS(),
        audioProcessor.getTruePeakDB(),
        audioProcessor.getDeviationLU(),
        audioProcessor.getContentType()
    );
}

void BULLsEYEEditor::updateCircularMeter()
{
    // Update circular meter with all current values
    circularMeter.setValues(
        audioProcessor.getIntegratedLUFS(),
        audioProcessor.getTruePeakDB(),
        audioProcessor.getDeviationLU(),
        audioProcessor.getContentType()
    );
}
