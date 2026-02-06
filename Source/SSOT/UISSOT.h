#pragma once

#include <juce_graphics/juce_graphics.h>  // For juce::Colour
#include <juce_core/juce_core.h>         // For juce::String

/**
 * UI Layer Single Source of Truth
 * 
 * Contains all UI-related constants:
 * - Colors (button states, status colors)
 * - Dimensions (button size, panel size)
 * - Strings (status text)
 * - Helper functions (status text/color getters)
 * 
 * Usage: Include in all UI components, never hardcode values.
 */
namespace UISSOT
{
    // ========================================================================
    // COLORS
    // ========================================================================
    
    namespace Colors
    {
        // Note: juce::Colour constructor is not constexpr in JUCE 8
        
        // TODO: Customize with your colors
        
        // Button colors
        inline juce::Colour PRIMARY_ON() { return juce::Colour::fromRGB(60, 180, 60); }
        inline juce::Colour PRIMARY_HL() { return juce::Colour::fromRGB(100, 220, 100); }
        inline juce::Colour SECONDARY_OFF() { return juce::Colour::fromRGB(180, 60, 60); }
        inline juce::Colour SECONDARY_HL() { return juce::Colour::fromRGB(220, 100, 100); }
        inline juce::Colour NEUTRAL() { return juce::Colour::fromRGB(100, 100, 100); }
        
        // Status colors
        inline juce::Colour STATUS_ACTIVE() { return juce::Colour::fromRGB(0, 200, 0); }
        inline juce::Colour STATUS_INACTIVE() { return juce::Colour::fromRGB(200, 0, 0); }
        
        // Special mode colors (e.g., Latch mode)
        inline juce::Colour SPECIAL_ON() { return juce::Colour::fromRGB(80, 200, 80); }
        inline juce::Colour SPECIAL_OFF() { return juce::Colour::fromRGB(150, 80, 80); }
        
        // UI colors (professional dark theme)
        inline juce::Colour BACKGROUND_DARK() { return juce::Colour::fromRGB(28, 28, 30); }
        inline juce::Colour BACKGROUND_MID() { return juce::Colour::fromRGB(35, 35, 38); }
        inline juce::Colour PANEL_BG() { return juce::Colour::fromRGB(40, 40, 44); }
        inline juce::Colour TEXT_PRIMARY() { return juce::Colour::fromRGB(255, 255, 255); }
        inline juce::Colour TEXT_SECONDARY() { return juce::Colour::fromRGB(180, 180, 180); }
        inline juce::Colour TEXT_TERTIARY() { return juce::Colour::fromRGB(140, 140, 140); }
        inline juce::Colour BRAND_ACCENT() { return juce::Colour::fromRGB(100, 150, 255); }
        inline juce::Colour DIVIDER() { return juce::Colour::fromRGB(60, 60, 65); }
    }
    
    // ========================================================================
    // DIMENSIONS
    // ========================================================================
    
    namespace Dimensions
    {
        // TODO: Customize with your dimensions
        
        // Button dimensions
        constexpr int BUTTON_DIAMETER = 140;
        constexpr int BUTTON_PRESS_OFFSET = 2;
        constexpr int SHADOW_OFFSET = 2;
        constexpr float HIGHLIGHT_ALPHA = 0.6f;
        
        // Panel dimensions (from JSFX @gfx W H)
        constexpr int PANEL_WIDTH = 450;
        constexpr int PANEL_HEIGHT = 320;
        
        // Layout spacing
        constexpr int PADDING_SMALL = 8;
        constexpr int PADDING_MEDIUM = 16;
        constexpr int PADDING_LARGE = 24;
        constexpr int SPACING_COMPONENT = 12;
        constexpr int SPACING_SECTION = 20;
        
        // Header
        constexpr int HEADER_HEIGHT = 50;
        constexpr int TITLE_FONT_SIZE = 24;
        constexpr int BRAND_FONT_SIZE = 11;
        
        // Component heights
        constexpr int STATUS_HEIGHT = 32;
        constexpr int MODE_SELECTOR_HEIGHT = 44;
        constexpr int TOGGLE_HEIGHT = 32;
        constexpr int DESCRIPTION_HEIGHT = 48;
    }
    
    // ========================================================================
    // STRINGS
    // ========================================================================
    
    namespace Strings
    {
        // TODO: Customize with your status strings
        
        constexpr const char* STATUS_ACTIVE_TEXT = "● Active";
        constexpr const char* STATUS_INACTIVE_TEXT = "● Inactive";
        
        // Your custom strings here
        constexpr const char* CUSTOM_STRING_1 = "Your custom string";
        constexpr const char* CUSTOM_STRING_2 = "Another custom string";
    }
    
    // ========================================================================
    // HELPER FUNCTIONS
    // ========================================================================
    
    namespace Helpers
    {
        /**
         * Get status text based on active state
         */
        inline juce::String getStatusText(bool isActive)
        {
            return isActive ? Strings::STATUS_ACTIVE_TEXT : Strings::STATUS_INACTIVE_TEXT;
        }
        
        /**
         * Get status color based on active state
         */
        inline juce::Colour getStatusColour(bool isActive)
        {
            return isActive ? Colors::STATUS_ACTIVE() : Colors::STATUS_INACTIVE();
        }
        
        /**
         * Get primary color based on state
         */
        inline juce::Colour getPrimaryColour(bool isOn)
        {
            return isOn ? Colors::PRIMARY_ON() : Colors::SECONDARY_OFF();
        }
        
        /**
         * Get highlight color based on state
         */
        inline juce::Colour getHighlightColour(bool isOn)
        {
            return isOn ? Colors::PRIMARY_HL() : Colors::SECONDARY_HL();
        }
        
        /**
         * Convert alpha to transparency
         */
        inline float alphaToAlpha(float alpha)
        {
            return alpha;  // Already in 0-1 range
        }
    }
}
