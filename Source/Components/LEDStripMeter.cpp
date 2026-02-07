#include "LEDStripMeter.h"

// ========================================================================
// CONSTRUCTOR
// ========================================================================

LEDStripMeter::LEDStripMeter()
{
    startTimerHz(UISSOT::Timing::UI_REFRESH_RATE_HZ);
    calculateLEDColors();
    cacheLEDPositions();  // Fixes DI-6: initialize positions even if resized() not called yet
}

// ========================================================================
// DESTRUCTOR
// ========================================================================

LEDStripMeter::~LEDStripMeter()
{
    // Fixes DI-5: explicit stop to prevent timer callback during destruction
    stopTimer();
}

// ========================================================================
// STATE UPDATES
// ========================================================================

void LEDStripMeter::setLevel(float normalizedLevel)
{
    targetLevel = juce::jlimit(0.0f, 1.0f, normalizedLevel);
}

void LEDStripMeter::setTarget(float targetLUFS)
{
    currentTarget = targetLUFS;
    calculateLEDColors();
    cacheLEDPositions();
    repaint();
}

// ========================================================================
// PRIVATE METHODS
// ========================================================================

void LEDStripMeter::calculateLEDColors()
{
    // Pre-calculate LED colors based on current target
    for (int i = 0; i < NUM_LEDS; i++)
    {
        float ledLUFS = juce::jmap(static_cast<float>(i) / NUM_LEDS, 0.0f, 1.0f, FLOOR, CEILING);
        cachedLEDs[i].colour = getLEDColor(ledLUFS);
    }
}

void LEDStripMeter::cacheLEDPositions()
{
    auto bounds = getLocalBounds().toFloat();
    cachedLEDArea = bounds.reduced(UISSOT::Dimensions::LED_METER_PADDING);

    // Cache LED positions and thresholds
    float ledWidth = cachedLEDArea.getWidth() / NUM_LEDS;

    for (int i = 0; i < NUM_LEDS; i++)
    {
        cachedLEDs[i].x = cachedLEDArea.getX() + (i * ledWidth);
        cachedLEDs[i].threshold = static_cast<float>(i) / NUM_LEDS;
    }

    // Cache target marker position
    float targetNormalized = lufsToNormalized(currentTarget);
    cachedTargetX = cachedLEDArea.getX() + (targetNormalized * cachedLEDArea.getWidth());
}

juce::Colour LEDStripMeter::getLEDColor(float lufsDB) const
{
    // Fixes DI-2: WARNING is now calculated dynamically based on currentTarget
    // instead of fixed at -9.0f, so color zones adapt to different content types
    auto warning = currentTarget + 5.0f;

    // Color zones based on calibration
    if (lufsDB < currentTarget - 6.0f)
    {
        // Green (safe zone) - use UISSOT balanced color
        return UISSOT::Colors::ledGreen();
    }
    else if (lufsDB < currentTarget)
    {
        // Light green (good zone)
        return UISSOT::Colors::ledLightGreen();
    }
    else if (lufsDB < warning)
    {
        // Yellow (loud zone)
        return UISSOT::Colors::ledYellow();
    }
    else if (lufsDB < -3.0f)
    {
        // Orange (very loud zone)
        return UISSOT::Colors::ledOrange();
    }
    else
    {
        // Red (danger/limiting zone)
        return UISSOT::Colors::ledRed();
    }
}

float LEDStripMeter::lufsToNormalized(float lufsDB) const
{
    return juce::jmap(lufsDB, FLOOR, CEILING, 0.0f, 1.0f);
}

// ========================================================================
// JUCE LIFECYCLE
// ========================================================================

void LEDStripMeter::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Background
    g.fillAll(UISSOT::Colors::meterBackground());

    // Use cached LED area
    const auto& ledArea = cachedLEDArea;
    float ledHeight = ledArea.getHeight();

    // Draw LED segments using cached data
    for (int i = 0; i < NUM_LEDS; i++)
    {
        const LEDData& led = cachedLEDs[i];

        // LED rectangle with rounded corners
        auto ledRect = juce::Rectangle<float>(
            led.x + UISSOT::Dimensions::LED_METER_LED_SPACING,
            ledArea.getY(),
            (ledArea.getWidth() / NUM_LEDS) - (UISSOT::Dimensions::LED_METER_LED_SPACING * 2),
            ledHeight
        );

        // Determine if LED is active (using cached threshold)
        bool isActive = currentLevel >= led.threshold;

        // Calculate brightness (active LEDs are bright, inactive are dim)
        float brightness = isActive ? 1.0f : UISSOT::Dimensions::LED_INACTIVE_BRIGHTNESS;

        // Draw LED using cached color
        g.setColour(led.colour.withAlpha(brightness));
        g.fillRoundedRectangle(ledRect, UISSOT::Dimensions::LED_CORNER_RADIUS);

        // Add shine effect on active LEDs
        if (isActive)
        {
            g.setColour(juce::Colours::white.withAlpha(UISSOT::Dimensions::LED_SHINE_ALPHA));
            g.fillRoundedRectangle(
                ledRect.withHeight(ledHeight * UISSOT::Dimensions::LED_SHINE_HEIGHT_RATIO),
                UISSOT::Dimensions::LED_SHINE_CORNER_RADIUS
            );
        }
    }

    // Draw target marker line using cached position
    g.setColour(UISSOT::Colors::ledTargetMarker());
    g.drawLine(
        cachedTargetX,
        ledArea.getY() - UISSOT::Dimensions::LED_TARGET_MARKER_EXTRA,
        cachedTargetX,
        ledArea.getBottom() + UISSOT::Dimensions::LED_TARGET_MARKER_EXTRA,
        UISSOT::Dimensions::LED_TARGET_MARKER_WIDTH
    );

    // Draw target label
    g.setFont(UISSOT::Typography::meterFont());
    g.setColour(UISSOT::Colors::textSecondary());

    juce::String targetLabel = juce::String(currentTarget, 0);
    g.drawText(
        targetLabel,
        cachedTargetX - UISSOT::Dimensions::LED_TARGET_LABEL_WIDTH / 2,
        ledArea.getBottom() + UISSOT::Dimensions::LED_TARGET_LABEL_OFFSET,
        UISSOT::Dimensions::LED_TARGET_LABEL_WIDTH,
        UISSOT::Dimensions::LED_TARGET_LABEL_HEIGHT,
        juce::Justification::centred
    );

    // Draw current LUFS value
    float currentLUFS = juce::jmap(currentLevel, 0.0f, 1.0f, FLOOR, CEILING);
    g.setFont(UISSOT::Typography::valueFont());
    g.setColour(UISSOT::Colors::textPrimary());

    juce::String lufsText = (currentLUFS > FLOOR + 5.0f) ?
        juce::String(currentLUFS, 1) : UISSOT::Strings::noMeasurement();
    g.drawText(
        lufsText + " LUFS",
        bounds.getX() + UISSOT::Dimensions::MARGIN_SMALL,
        bounds.getY() + UISSOT::Dimensions::MARGIN_SMALL,
        UISSOT::Dimensions::LED_VALUE_WIDTH,
        UISSOT::Dimensions::LED_VALUE_HEIGHT,
        juce::Justification::centredLeft
    );
}

void LEDStripMeter::resized()
{
    // Cache LED positions for optimized painting
    cacheLEDPositions();
}

// ========================================================================
// TIMER (smooth animation)
// ========================================================================

void LEDStripMeter::timerCallback()
{
    // Smooth interpolation towards target level
    float diff = targetLevel - currentLevel;
    float threshold = UISSOT::Dimensions::LED_REPAINT_THRESHOLD;

    if (std::abs(diff) < threshold)
    {
        // Fixes DI-4: snap to target when close enough (prevents asymptotic freezing)
        currentLevel = targetLevel;
    }
    else
    {
        // Smooth exponential decay towards target
        currentLevel += diff * UISSOT::Dimensions::LED_SMOOTHING_FACTOR;
    }

    // Repaint if there's any change
    if (std::abs(targetLevel - currentLevel) > 0.0f || currentLevel != targetLevel)
    {
        repaint();
    }
}
