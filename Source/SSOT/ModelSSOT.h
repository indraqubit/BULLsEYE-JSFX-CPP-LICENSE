#pragma once

#include <cstddef>

namespace ModelSSOT
{
    // ==========================================
    // PARAMETER IDENTIFIERS
    // ==========================================
    namespace ParameterIDs
    {
        constexpr const char* CONTENT_TYPE = "contentType";
    }

    // ==========================================
    // PARAMETER NAMES
    // ==========================================
    namespace ParameterNames
    {
        constexpr const char* CONTENT_TYPE = "Content Type";
    }

    // ==========================================
    // PARAMETER RANGES
    // ==========================================
    namespace ParameterRanges
    {
        constexpr float CONTENT_TYPE_MIN = 0.0f;
        constexpr float CONTENT_TYPE_MAX = 2.0f;
        constexpr float CONTENT_TYPE_DEFAULT = 1.0f;  // Music Drums (-8 LUFS)
    }

    // ==========================================
    // MODE ENUMS
    // ==========================================
    enum class ContentType
    {
        MusicNonDrums = 0,  // -11 LUFS
        MusicDrums = 1,      // -8 LUFS
        CinemaTrailer = 2    // -14 LUFS
    };

    constexpr int CONTENT_TYPE_COUNT = 3;

    // ==========================================
    // MODE HELPERS
    // ==========================================
    namespace Helpers
    {
        constexpr ContentType intToContentType(int value)
        {
            return static_cast<ContentType>(
                (value >= 0 && value < CONTENT_TYPE_COUNT) ? value : 1
            );
        }

        constexpr const char* contentTypeToString(ContentType type)
        {
            switch (type)
            {
                case ContentType::MusicNonDrums:  return "Music Non-drums";
                case ContentType::MusicDrums:     return "Music Drums";
                case ContentType::CinemaTrailer:   return "Cinema/Trailer";
                default:                           return "Unknown";
            }
        }

        constexpr double getTargetLUFS(ContentType type)
        {
            switch (type)
            {
                case ContentType::MusicNonDrums:  return -11.0;
                case ContentType::MusicDrums:      return -8.0;
                case ContentType::CinemaTrailer:   return -14.0;
                default:                           return -8.0;
            }
        }
    }
}
