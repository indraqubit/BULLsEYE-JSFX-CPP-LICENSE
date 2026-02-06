# SSOT Template Guide

**Single Source of Truth patterns for JUCE migration**

---

## Overview

SSOT (Single Source of Truth) centralizes all constants in dedicated files to avoid hardcoded values throughout the codebase.

## SSOT Files

| File | Namespace | Purpose |
|------|-----------|---------|
| `ModelSSOT.h` | `ModelSSOT` | Parameter IDs, names, modes |
| `UISSOT.h` | `UISSOT` | Colors, dimensions, strings |
| `DSPSSOT.h` | `DSPSSOT` | DSP constants, algorithm values |
| `ProcessorSSOT.h` | `ProcessorSSOT` | Buffer sizes, processing settings |

---

## ModelSSOT Template

```cpp
#pragma once
#include <juce_core/juce_core.h>

namespace ModelSSOT
{
    namespace ParameterIDs
    {
        constexpr const char* PARAM1 = "param1";
        constexpr const char* PARAM2 = "param2";
        constexpr const char* PARAM3 = "param3";
    }

    namespace ParameterNames
    {
        constexpr const char* PARAM1 = "Parameter 1";
        constexpr const char* PARAM2 = "Parameter 2";
        constexpr const char* PARAM3 = "Parameter 3";
    }

    enum class YourMode
    {
        Mode1 = 0,
        Mode2 = 1,
        Mode3 = 2
    };

    constexpr int MODE_COUNT = 3;

    namespace Helpers
    {
        inline YourMode getModeFromIndex(int index)
        {
            return static_cast<YourMode>(juce::jlimit(0, MODE_COUNT - 1, index));
        }

        inline int getIndexFromMode(YourMode mode)
        {
            return static_cast<int>(mode);
        }
    }
}
```

---

## UISSOT Template

```cpp
#pragma once
#include <juce_graphics/juce_graphics.h>

namespace UISSOT
{
    namespace Colors
    {
        inline juce::Colour BACKGROUND() {
            return juce::Colour::fromRGB(30, 30, 30);
        }

        inline juce::Colour TEXT() {
            return juce::Colour::fromRGB(200, 200, 200);
        }

        inline juce::Colour ACCENT() {
            return juce::Colour::fromRGB(0, 150, 255);
        }

        inline juce::Colour SUCCESS() {
            return juce::Colour::fromRGB(0, 200, 100);
        }

        inline juce::Colour WARNING() {
            return juce::Colour::fromRGB(255, 180, 0);
        }

        inline juce::Colour DANGER() {
            return juce::Colour::fromRGB(255, 50, 50);
        }
    }

    namespace Dimensions
    {
        constexpr float COMPONENT_HEIGHT = 60.0f;
        constexpr float COMPONENT_WIDTH = 200.0f;
        constexpr float PADDING = 10.0f;
        constexpr float MARGIN = 20.0f;
    }

    namespace Strings
    {
        constexpr const char* PLUGIN_NAME = "Your Plugin";
        constexpr const char* COMPANY_NAME = "Your Company";
    }
}
```

---

## DSPSSOT Template

```cpp
#pragma once

namespace DSPSSOT
{
    namespace Algorithm
    {
        constexpr float THRESHOLD = -6.0f;
        constexpr float RATIO = 4.0f;
        constexpr float ATTACK = 0.01f;
        constexpr float RELEASE = 0.1f;
        constexpr float MAKEUP_GAIN = 3.0f;
    }

    namespace Filter
    {
        constexpr float CUTOFF_FREQUENCY = 1000.0f;
        constexpr float Q_VALUE = 0.707f;
    }

    namespace Helpers
    {
        inline float dbToLinear(float db)
        {
            return std::pow(10.0f, db / 20.0f);
        }

        inline float linearToDb(float linear)
        {
            return 20.0f * std::log10(linear + 1e-6f);
        }
    }
}
```

---

## ProcessorSSOT Template

```cpp
#pragma once

namespace ProcessorSSOT
{
    namespace Audio
    {
        constexpr int MAX_BUFFER_SIZE = 512;
        constexpr int MIN_BUFFER_SIZE = 32;
        constexpr double SAMPLE_RATE = 44100.0;
    }

    namespace Parameters
    {
        constexpr float PARAM_MIN = 0.0f;
        constexpr float PARAM_MAX = 1.0f;
        constexpr float PARAM_DEFAULT = 0.5f;
    }

    namespace Smoothing
    {
        constexpr float SMOOTHING_TIME = 0.05f;
    }
}
```

---

## Usage Guidelines

### DO

- ✅ Use SSOT constants everywhere in your code
- ✅ Update SSOT files when constants change
- ✅ Add helper functions for complex calculations
- ✅ Use `constexpr` for compile-time constants

### DON'T

- ❌ Hardcode values in .cpp files
- ❌ Duplicate constants across files
- ❌ Use magic numbers without explanation

---

## Example Usage

```cpp
#include "Source/SSOT/ModelSSOT.h"
#include "Source/SSOT/UISSOT.h"

void yourFunction()
{
    // Using SSOT constants
    auto color = UISSOT::Colors::ACCENT();
    auto mode = ModelSSOT::Helpers::getModeFromIndex(0);
}
```

---

**Template Version:** 1.0.0
**Last Updated:** 2026-02-06
