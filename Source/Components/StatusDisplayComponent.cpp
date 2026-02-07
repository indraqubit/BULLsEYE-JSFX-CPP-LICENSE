#include "StatusDisplayComponent.h"

// ========================================================================
// CONSTRUCTOR
// ========================================================================

StatusDisplayComponent::StatusDisplayComponent()
{
    setOpaque(true);
}

// ========================================================================
// STATE UPDATES
// ========================================================================

void StatusDisplayComponent::setValues(double lufs, double truePeakDB, double deviationLU,
                                       ModelSSOT::ContentType contentType)
{
    // EDGE CASE: Validate input values for NaN and infinity
    if (std::isnan(lufs) || std::isinf(lufs))
        lufs = DSPSSOT::TruePeak::MIN_DISPLAY_DB;
    if (std::isnan(truePeakDB) || std::isinf(truePeakDB))
        truePeakDB = DSPSSOT::TruePeak::MIN_DISPLAY_DB;
    if (std::isnan(deviationLU) || std::isinf(deviationLU))
        deviationLU = 0.0;

    // Clamp to valid ranges
    lufs = std::max(lufs, DSPSSOT::TruePeak::MIN_DISPLAY_DB);
    lufs = std::min(lufs, DSPSSOT::TruePeak::MAX_DISPLAY_DB);
    truePeakDB = std::max(truePeakDB, DSPSSOT::TruePeak::MIN_DISPLAY_DB);
    truePeakDB = std::min(truePeakDB, DSPSSOT::TruePeak::MAX_DISPLAY_DB);

    currentLUFS = lufs;
    currentTruePeak = truePeakDB;
    currentDeviation = deviationLU;
    currentContentType = contentType;

    updateDeviationNormalized();
    repaint();
}

void StatusDisplayComponent::updateDeviationNormalized()
{
    // Map deviation to 0-1 range (0 = left edge, 0.5 = center, 1 = right edge)
    // Bar range is ±5 LU
    constexpr double maxDeviation = DSPSSOT::DeviationDisplay::BAR_RANGE_LU;

    // EDGE CASE: Handle NaN and infinity
    if (std::isnan(currentDeviation) || std::isinf(currentDeviation))
    {
        deviationNormalized = 0.5; // Default to center
    }
    else
    {
        deviationNormalized = 0.5 + (currentDeviation / (2.0 * maxDeviation));
        // Clamp to valid range but preserve direction indicator
        deviationNormalized = juce::jlimit(0.0, 1.0, deviationNormalized);
    }
}

juce::Colour StatusDisplayComponent::getStatusColor() const
{
    if (std::abs(currentDeviation) <= DSPSSOT::DeviationDisplay::BALANCED_RANGE_LU)
        return UISSOT::Colors::balanced();
    else if (currentDeviation > DSPSSOT::DeviationDisplay::BALANCED_RANGE_LU)
        return UISSOT::Colors::hot();
    else
        return UISSOT::Colors::quiet();
}

juce::String StatusDisplayComponent::getStatusText() const
{
    // Match JSFX: show status even if below gate threshold
    // (calculation happened, just quiet). Only hide if uninitialized.
    constexpr double NEG_INF_THRESHOLD = -900000000.0;  // Uninitialized sentinel
    if (currentLUFS <= NEG_INF_THRESHOLD)
    {
        return UISSOT::Strings::statusQuiet();
    }

    if (std::abs(currentDeviation) <= DSPSSOT::DeviationDisplay::BALANCED_RANGE_LU)
        return UISSOT::Strings::statusBalanced();
    else if (currentDeviation > DSPSSOT::DeviationDisplay::BALANCED_RANGE_LU)
        return UISSOT::Strings::statusHot();
    else
        return UISSOT::Strings::statusQuiet();
}

// ========================================================================
// JUCE LIFECYCLE
// ========================================================================

void StatusDisplayComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Background
    g.fillAll(UISSOT::Colors::background());

    // Padding
    bounds = bounds.reduced(UISSOT::Dimensions::MARGIN_MEDIUM);

    // Colors
    juce::Colour statusColor = getStatusColor();
    juce::String statusText = getStatusText();

    // Draw header (LUFS value)
    g.setFont(UISSOT::Typography::headerFont());
    g.setColour(UISSOT::Colors::textPrimary());

    // EDGE CASE: Handle signal below gate threshold
    // Match JSFX: show value even if quieter than -70 LUFS, only hide if uninitialized
    constexpr double NEG_INF_THRESHOLD = -900000000.0;  // Uninitialized sentinel
    juce::String lufsText;
    if (currentLUFS <= NEG_INF_THRESHOLD)
    {
        // Signal never measured (uninitialized)
        lufsText = UISSOT::Strings::noMeasurement();
    }
    else if (std::isnan(currentLUFS) || std::isinf(currentLUFS))
    {
        // Invalid measurement - show "--.-"
        lufsText = UISSOT::Strings::noMeasurement();
    }
    else
    {
        // Valid measurement - show actual value (even if below gate)
        lufsText = juce::String(currentLUFS, 1);
    }
    g.drawText("LUFS-I: " + lufsText, bounds, juce::Justification::centredTop);

    // Draw deviation status text
    g.setFont(UISSOT::Typography::labelFont());
    g.setColour(statusColor);

    // EDGE CASE: Handle signal below gate threshold for deviation display
    // Match JSFX: show deviation even if quieter than -70 LUFS
    juce::String devText;
    if (currentLUFS <= -900000000.0)
    {
        // Signal never measured (uninitialized) - no valid deviation
        devText = "--.- LU";
    }
    else if (std::isnan(currentDeviation) || std::isinf(currentDeviation))
    {
        // Invalid deviation
        devText = "--.- LU";
    }
    else
    {
        // Valid deviation - show status text and value (even if below gate)
        devText = statusText + " (" + juce::String(currentDeviation, 1) + " LU)";
    }
    g.drawText(devText, bounds, juce::Justification::centred);

    // Draw deviation bar
    auto barArea = bounds.withTrimmedTop(40).reduced(20, 0);
    float barWidth = barArea.getWidth();
    float barHeight = UISSOT::Dimensions::BAR_HEIGHT;
    float barY = barArea.getY();
    float centerX = barArea.getX() + barWidth * 0.5f;

    // Background bar
    g.setColour(UISSOT::Colors::meterBackground());
    g.fillRoundedRectangle(barArea.toFloat(), 4.0f);

    // Balanced zone (green, ±1 LU from center)
    float balancedHalfWidth = barWidth * (DSPSSOT::DeviationDisplay::BALANCED_RANGE_LU /
                                          DSPSSOT::DeviationDisplay::BAR_RANGE_LU) * 0.5f;
    auto balancedZone = juce::Rectangle<float>(
        centerX - balancedHalfWidth, barY, balancedHalfWidth * 2, barHeight
    );
    g.setColour(UISSOT::Colors::meterBalancedZone());
    g.fillRoundedRectangle(balancedZone, 4.0f);

    // Center line
    g.setColour(UISSOT::Colors::meterCenterLine());
    g.drawLine(centerX, barY - 2, centerX, barY + barHeight + 2, 2.0f);

    // Deviation indicator
    // EDGE CASE: Only show indicator when we have valid measurement (not uninitialized)
    if (currentLUFS > -900000000.0 && !std::isnan(currentDeviation) && !std::isinf(currentDeviation))
    {
        float indicatorX = barArea.getX() + deviationNormalized * barWidth;

        g.setColour(statusColor);
        g.fillRoundedRectangle(
            juce::Rectangle<float>(indicatorX - 3, barY - 2, 6, barHeight + 4),
            2.0f
        );
    }

    // True Peak value
    g.setFont(UISSOT::Typography::meterFont());
    g.setColour(UISSOT::Colors::textSecondary());
    juce::String tpText = UISSOT::Strings::truePeakLabel() + ": " +
                          juce::String(currentTruePeak, 1) + " dBTP";
    g.drawText(tpText, bounds, juce::Justification::centredBottom);
}

void StatusDisplayComponent::resized()
{
    // No dynamic layout changes needed
}
