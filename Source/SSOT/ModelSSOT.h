#pragma once

#include <juce_core/juce_core.h>  // For juce::String

/**
 * Model Layer Single Source of Truth
 * 
 * Contains all model-related constants:
 * - Parameter IDs and names
 * - Mode enums
 * - Helper functions for mode logic
 * 
 * Usage: Include in all files that need parameter definitions or mode logic.
 * Never hardcode parameter IDs or mode values.
 */
namespace ModelSSOT
{
    // ========================================================================
    // PARAMETER DEFINITIONS
    // ========================================================================
    
    namespace ParameterIDs
    {
        // TODO: Replace with your parameter IDs
        constexpr const char* MUTE_STATE = "muteState";
        constexpr const char* MUTE_MODE = "muteMode";
        constexpr const char* INVERT_ENABLED = "invertEnabled";
    }
    
    namespace ParameterNames
    {
        // TODO: Replace with your parameter names
        constexpr const char* MUTE_STATE = "Mute State";
        constexpr const char* MUTE_MODE = "Mute Mode";
        constexpr const char* INVERT_ENABLED = "Invert";
    }
    
    // ========================================================================
    // MODE ENUMS
    // ========================================================================
    
    // TODO: Replace with your modes
    enum class YourMode
    {
        ModeA = 0,
        ModeB = 1,
        ModeC = 2,
        ModeD = 3
    };
    
    constexpr int MODE_COUNT = 4;
    
    // ========================================================================
    // HELPER FUNCTIONS
    // ========================================================================
    
    namespace Helpers
    {
        /**
         * Get mode name for display
         * TODO: Customize with your mode names
         */
        inline juce::String getModeName(YourMode mode)
        {
            switch (mode)
            {
                case YourMode::ModeA: return "Mode A";
                case YourMode::ModeB: return "Mode B";
                case YourMode::ModeC: return "Mode C";
                case YourMode::ModeD: return "Mode D";
                default: return "Unknown";
            }
        }
        
        /**
         * Get mode description for UI display
         * TODO: Customize with your mode descriptions
         */
        inline juce::String getModeDescription(YourMode mode)
        {
            switch (mode)
            {
                case YourMode::ModeA: 
                    return "Description for Mode A";
                case YourMode::ModeB: 
                    return "Description for Mode B";
                case YourMode::ModeC: 
                    return "Description for Mode C";
                case YourMode::ModeD: 
                    return "Description for Mode D";
                default: return "";
            }
        }
        
        /**
         * Calculate effective parameter value with invert logic
         * TODO: Customize with your invert logic
         */
        inline bool getEffectiveValue(bool value, bool invertEnabled)
        {
            return invertEnabled ? !value : value;
        }
        
        /**
         * Validate mode index
         */
        inline bool isValidMode(int modeIndex)
        {
            return modeIndex >= 0 && modeIndex < MODE_COUNT;
        }
        
        /**
         * Convert mode index to enum
         */
        inline YourMode intToMode(int modeIndex)
        {
            if (isValidMode(modeIndex))
                return static_cast<YourMode>(modeIndex);
            return YourMode::ModeA;  // Default
        }
        
        /**
         * Convert enum to mode index
         */
        inline int modeToInt(YourMode mode)
        {
            return static_cast<int>(mode);
        }
    }
}
