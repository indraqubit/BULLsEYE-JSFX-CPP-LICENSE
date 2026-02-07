#pragma once

#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>

namespace UISSOT
{
    // ==========================================
    // COLORS
    // ==========================================
    namespace Colors
    {
        // Background colors
        inline juce::Colour background() { return juce::Colour(0xff1a1a1a); }
        inline juce::Colour backgroundDark() { return juce::Colour(0xff121212); }
        inline juce::Colour backgroundLight() { return juce::Colour(0xff2a2a2a); }

        // Text colors
        inline juce::Colour textPrimary() { return juce::Colour(0xffffffff); }
        inline juce::Colour textSecondary() { return juce::Colour(0xffaaaaaa); }
        inline juce::Colour textMuted() { return juce::Colour(0xff666666); }

        // Status colors
        inline juce::Colour balanced() { return juce::Colour(0xff4dff4d); }     // Green
        inline juce::Colour hot() { return juce::Colour(0xffff4d4d); }         // Red
        inline juce::Colour quiet() { return juce::Colour(0xff4d4dff); }       // Blue

        // UI element colors
        inline juce::Colour panel() { return juce::Colour(0xff252525); }
        inline juce::Colour border() { return juce::Colour(0xff3a3a3a); }
        inline juce::Colour highlight() { return juce::Colour(0xff4a9eff); }

        // Meter colors
        inline juce::Colour meterBackground() { return juce::Colour(0xff333333); }
        inline juce::Colour meterBalancedZone() { return juce::Colour(0x664dff4d); }  // Semi-transparent green
        inline juce::Colour meterCenterLine() { return juce::Colour(0xffcccccc); }

        // LED strip colors
        inline juce::Colour ledGreen() { return juce::Colour(0xff00ff00); }           // Safe zone
        inline juce::Colour ledLightGreen() { return juce::Colour(0xff88ff00); }       // Good zone
        inline juce::Colour ledYellow() { return juce::Colour(0xffffff00); }          // Loud zone
        inline juce::Colour ledOrange() { return juce::Colour(0xffff8800); }          // Very loud zone
        inline juce::Colour ledRed() { return juce::Colour(0xffff0000); }             // Danger zone
        inline juce::Colour ledTargetMarker() { return juce::Colour(0xff00ffff); }     // Cyan target line

        // Error state colors
        inline juce::Colour errorState() { return juce::Colour(0xffff00ff); }           // Yellow for invalid/error states
        inline juce::Colour overflow() { return juce::Colour(0xffff8800); }             // Orange for overflow/clipping
        inline juce::Colour underflow() { return juce::Colour(0xff8888ff); }            // Light blue for underflow/silence
    }

    // ==========================================
    // DIMENSIONS
    // ==========================================
    namespace Dimensions
    {
        constexpr int DEFAULT_WINDOW_WIDTH = 320;
        constexpr int DEFAULT_WINDOW_HEIGHT = 300;

        // Margins and padding
        constexpr int MARGIN_SMALL = 4;
        constexpr int MARGIN_MEDIUM = 8;
        constexpr int MARGIN_LARGE = 12;
        constexpr int MARGIN_XLARGE = 16;

        // Component dimensions
        constexpr int HEADER_HEIGHT = 30;
        constexpr int METER_HEIGHT = 20;
        constexpr int BUTTON_HEIGHT = 24;
        constexpr int DROPDOWN_HEIGHT = 28;

        // Bar meter
        constexpr int BAR_WIDTH = 276;
        constexpr int BAR_HEIGHT = 8;
        constexpr int BAR_RANGE_LU = 5;  // ±5 LU range

        // LED Strip Meter
        constexpr int LED_METER_HEIGHT = 40;
        constexpr int LED_METER_PADDING = 4;
        constexpr int LED_METER_LED_SPACING = 1;
        constexpr int LED_CORNER_RADIUS = 2;
        constexpr int LED_SHINE_CORNER_RADIUS = 1;
        constexpr float LED_SHINE_HEIGHT_RATIO = 0.3f;
        constexpr float LED_SHINE_ALPHA = 0.2f;
        constexpr float LED_INACTIVE_BRIGHTNESS = 0.15f;
        constexpr float LED_SMOOTHING_FACTOR = 0.25f;
        constexpr float LED_REPAINT_THRESHOLD = 0.001f;
        constexpr int LED_TARGET_MARKER_EXTRA = 3;
        constexpr int LED_TARGET_MARKER_WIDTH = 2;
        constexpr int LED_TARGET_LABEL_WIDTH = 24;
        constexpr int LED_TARGET_LABEL_HEIGHT = 12;
        constexpr int LED_TARGET_LABEL_OFFSET = 5;
        constexpr int LED_VALUE_WIDTH = 100;
        constexpr int LED_VALUE_HEIGHT = 20;

        // Edge case and validation thresholds
        constexpr double MIN_VALID_SAMPLE_RATE = 8000.0;    // Minimum valid sample rate
        constexpr double MAX_VALID_SAMPLE_RATE = 1000000.0; // Maximum valid sample rate (1 MHz)
        constexpr double MAX_DEVIATION_CLAMP = 50.0;        // Clamp extreme deviations to ±50 LU
        constexpr double MAX_PEAK_CLAMP = 1e10;            // Clamp extreme peak values
        constexpr double MAX_BLOCK_ACCUM = 1e100;           // Clamp extreme block accumulators
    }

    // ==========================================
    // STRINGS
    // ==========================================
    namespace Strings
    {
        inline juce::String pluginName() { return "BULLsEYE"; }
        inline juce::String contentTypeLabel() { return "Content Type"; }
        inline juce::String lufsLabel() { return "Live LUFS-I"; }
        inline juce::String mixEnergyLabel() { return "Mix Energy"; }
        inline juce::String truePeakLabel() { return "True Peak"; }
        inline juce::String statusBalanced() { return "Balanced"; }
        inline juce::String statusHot() { return "Hot"; }
        inline juce::String statusQuiet() { return "Quiet"; }
        inline juce::String noMeasurement() { return "--.-"; }
        inline juce::String signalBelowGate() { return "Below Gate"; }
        inline juce::String invalidSignal() { return "Invalid"; }
    }

    // ==========================================
    // TYPOGRAPHY
    // ==========================================
    namespace Typography
    {
        inline juce::FontOptions headerFont()
        {
            return juce::FontOptions().withHeight(16.0f).withStyle("bold");
        }

        inline juce::FontOptions labelFont()
        {
            return juce::FontOptions().withHeight(12.0f);
        }

        inline juce::FontOptions valueFont()
        {
            return juce::FontOptions().withHeight(14.0f).withStyle("bold");
        }

        inline juce::FontOptions meterFont()
        {
            return juce::FontOptions().withHeight(10.0f);
        }
    }

    // ==========================================
    // TIMING
    // ==========================================
    namespace Timing
    {
        constexpr int UI_REFRESH_RATE_HZ = 30;
        constexpr int METER_SMOOTHING_MS = 100;
    }
}
