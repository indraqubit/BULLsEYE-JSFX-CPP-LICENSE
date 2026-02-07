#include "CircularMeterComponent.h"

// ========================================================================
// CONSTRUCTOR / DESTRUCTOR
// ========================================================================

CircularMeterComponent::CircularMeterComponent()
{
    startTimerHz(UISSOT::Timing::UI_REFRESH_RATE_HZ);
}

CircularMeterComponent::~CircularMeterComponent()
{
    stopTimer();
}

// ========================================================================
// STATE UPDATES
// ========================================================================

void CircularMeterComponent::setValues(double lufs, double truePeakDB, double deviationLU,
                                       ModelSSOT::ContentType contentType)
{
    // Validate and clamp input values
    if (std::isnan(lufs) || std::isinf(lufs))
        lufs = DSPSSOT::TruePeak::MIN_DISPLAY_DB;
    if (std::isnan(truePeakDB) || std::isinf(truePeakDB))
        truePeakDB = DSPSSOT::TruePeak::MIN_DISPLAY_DB;
    if (std::isnan(deviationLU) || std::isinf(deviationLU))
        deviationLU = 0.0;

    lufs = std::max(lufs, DSPSSOT::TruePeak::MIN_DISPLAY_DB);
    lufs = std::min(lufs, DSPSSOT::TruePeak::MAX_DISPLAY_DB);

    currentLUFS = lufs;
    currentTruePeak = truePeakDB;
    currentDeviation = deviationLU;
    currentContentType = contentType;

    // Calculate normalized level for arc (0-1 range)
    targetLevel = static_cast<float>(
        (lufs - DSPSSOT::TruePeak::MIN_DISPLAY_DB) / (-DSPSSOT::TruePeak::MIN_DISPLAY_DB)
    );
    targetLevel = juce::jlimit(0.0f, 1.0f, targetLevel);
}

// ========================================================================
// PRIVATE METHODS
// ========================================================================

juce::Colour CircularMeterComponent::getStatusColor() const
{
    if (std::abs(currentDeviation) <= DSPSSOT::DeviationDisplay::BALANCED_RANGE_LU)
        return UISSOT::Colors::balanced();
    else if (currentDeviation > DSPSSOT::DeviationDisplay::BALANCED_RANGE_LU)
        return UISSOT::Colors::hot();
    else
        return UISSOT::Colors::quiet();
}

juce::String CircularMeterComponent::getStatusText() const
{
    // Match JSFX: show status even if below gate threshold
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

void CircularMeterComponent::timerCallback()
{
    // Smooth animation towards target level
    float diff = targetLevel - animatedLevel;
    if (std::abs(diff) < 0.001f)
    {
        animatedLevel = targetLevel;
    }
    else
    {
        animatedLevel += diff * 0.6f;  // Faster exponential decay (60% per frame @ 30 FPS ≈ 50ms response)
    }
    repaint();
}

// ========================================================================
// JUCE LIFECYCLE
// ========================================================================

void CircularMeterComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Background
    g.fillAll(UISSOT::Colors::background());

    // Meter dimensions (centered circle)
    float meterSize = std::min(bounds.getWidth(), bounds.getHeight()) - 20.0f;
    float centerX = bounds.getCentreX();
    float centerY = bounds.getCentreY();
    float radius = meterSize / 2.0f;

    // Colors
    juce::Colour statusColor = getStatusColor();

    // ========================================================================
    // Draw Meter Ring (Donut)
    // ========================================================================

    // Outer and inner radii for donut effect
    auto circle = juce::Rectangle<float>::leftTopRightBottom(
        centerX - radius, centerY - radius,
        centerX + radius, centerY + radius
    );

    // Background ring (gray, inactive)
    g.setColour(UISSOT::Colors::meterBackground());
    g.drawEllipse(circle, 12.0f);

    // Filled arc (active meter)
    // Arc spans from -135° to +135° (270° total, like a speedometer)
    // -135° is bottom-left, 0° is right, +135° is bottom-right
    // Match JSFX: show arc even if below gate threshold
    if (currentLUFS > -900000000.0 && animatedLevel > 0.0f)
    {
        g.setColour(statusColor);

        // Calculate arc angle (0-1 → -135° to +135°)
        float startAngle = -135.0f * DSPSSOT::Math::PI / 180.0f;
        float arcSpan = 270.0f * DSPSSOT::Math::PI / 180.0f;
        float arcLength = arcSpan * animatedLevel;

        juce::Path arcPath;
        arcPath.addCentredArc(
            centerX, centerY,
            radius - 6.0f, radius - 6.0f,  // Inner and outer radius
            0.0f,
            startAngle,
            startAngle + arcLength,
            true
        );

        g.strokePath(arcPath, juce::PathStrokeType(12.0f, juce::PathStrokeType::curved));
    }

    // Outline ring
    g.setColour(UISSOT::Colors::meterCenterLine());
    g.drawEllipse(circle, 1.0f);

    // ========================================================================
    // Center Text (Status)
    // ========================================================================

    juce::String statusText = getStatusText();

    // Status text in center (large)
    g.setFont(UISSOT::Typography::valueFont());
    g.setColour(statusColor);

    auto textBounds = juce::Rectangle<float>(
        centerX - 50, centerY - 15,
        100, 30
    );
    g.drawText(statusText, textBounds, juce::Justification::centred);

    // ========================================================================
    // Deviation Text Below Center
    // ========================================================================

    if (currentLUFS > -900000000.0 && !std::isnan(currentDeviation) && !std::isinf(currentDeviation))
    {
        g.setFont(UISSOT::Typography::labelFont());
        g.setColour(UISSOT::Colors::textSecondary());

        juce::String deviationText = "(" + juce::String(currentDeviation >= 0 ? "+" : "") + 
                                     juce::String(currentDeviation, 1) + " LU)";

        auto deviationBounds = juce::Rectangle<float>(
            centerX - 40, centerY + 12,
            80, 18
        );
        g.drawText(deviationText, deviationBounds, juce::Justification::centred);
    }
}

void CircularMeterComponent::resized()
{
    // No dynamic layout changes needed
}
