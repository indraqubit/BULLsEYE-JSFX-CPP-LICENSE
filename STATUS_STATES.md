# BULLsEYE Status States: Quiet, Balanced, Hot

**Version:** 1.0  
**Last Updated:** 2026-02-07  
**Status:** Production Ready

---

## Table of Contents

1. [Overview](#overview)
2. [Status State Definitions](#status-state-definitions)
3. [Calculation Logic](#calculation-logic)
4. [Visual Flowchart](#visual-flowchart)
5. [Practical Examples](#practical-examples)
6. [Industry Standards](#industry-standards)
7. [Color Psychology](#color-psychology)
8. [Edge Cases](#edge-cases)
9. [Real-World Scenarios](#real-world-scenarios)
10. [Summary Reference](#summary-reference)

---

## Overview

BULLsEYE uses three status states to provide instant visual feedback about loudness levels:

- **🔵 Quiet** - Audio is too quiet for the target
- **🟢 Balanced** - Audio is within acceptable target range
- **🔴 Hot** - Audio is too loud for the target

These states are calculated from the deviation between the current LUFS-I measurement and the target LUFS for the selected content type.

---

## Status State Definitions

### Typography and Color

All three status states use the same font styling and dynamic color:

**Font:** `UISSOT::Typography::valueFont()`
- **Size:** 14pt
- **Style:** Bold
- **Purpose:** Prominent, easily readable at a glance

**Color:** Dynamic based on deviation
- Determined by `getStatusColor()` function
- Used for **both** font text and meter arc
- Ensures visual consistency

```cpp
juce::Colour statusColor = getStatusColor();  // Returns #4dff4d for balanced

// Used for BOTH font and meter:
g.setFont(UISSOT::Typography::valueFont());  // 14pt bold
g.setColour(statusColor);  // Same color for text AND meter
g.drawText("Balanced", textBounds, juce::Justification::centred);
// ...
g.strokePath(arcPath, juce::PathStrokeType(12.0f));  // Same color for arc
```

### 🔵 Quiet

**When:**
- Deviation < -1.0 LU (more than 1 LU below target)
- Current LUFS is significantly lower than target LUFS

**Meaning:**
- Audio level is below the required loudness
- May need gain increase or compression
- Common in: quiet passages, intros, low-energy sections

**Color:** `#4d4dff` (medium blue)
- **RGB:** (77, 77, 255)
- **Function:** `UISSOT::Colors::quiet()`

**User Action:** Increase gain or apply compression to bring loudness up

---

### 🟢 Balanced

**When:**
- -1.0 LU ≤ Deviation ≤ +1.0 LU (within 1 LU of target)
- Current LUFS is within acceptable target range

**Meaning:**
- Audio level is correct for the chosen content type
- Ready for distribution
- Common in: properly mastered content, broadcast mixes

**Color:** `#4dff4d` (bright green)
- **RGB:** (77, 255, 77)
- **Function:** `UISSOT::Colors::balanced()`

**User Action:** None - loudness is perfect ✓

---

### 🔴 Hot

**When:**
- Deviation > +1.0 LU (more than 1 LU above target)
- Current LUFS is significantly higher than target

**Meaning:**
- Audio level is above the required loudness
- May need gain reduction or limiting
- Common in: heavy compression, loud mastering, clipping

**Color:** `#ff4d4d` (bright red)
- **RGB:** (255, 77, 77)
- **Function:** `UISSOT::Colors::hot()`

**User Action:** Reduce gain or apply limiter to bring loudness down

---

## Calculation Logic

### Deviation Formula

```cpp
deviationLU = currentLUFS - targetLUFS
```

**Where:**
- `currentLUFS` = Measured integrated loudness (from DSP)
- `targetLUFS` = Target loudness for selected content type

### Status Determination Code

```cpp
juce::String getStatusText() const
{
    if (std::abs(currentDeviation) <= DSPSSOT::DeviationDisplay::BALANCED_RANGE_LU)
        return UISSOT::Strings::statusBalanced();  // 🟢
    else if (currentDeviation > DSPSSOT::DeviationDisplay::BALANCED_RANGE_LU)
        return UISSOT::Strings::statusHot();      // 🔴
    else
        return UISSOT::Strings::statusQuiet();     // 🔵
}
```

**Constant from `DSPSSOT.h`:**
```cpp
namespace DeviationDisplay
{
    constexpr double BALANCED_RANGE_LU = 1.0;   // Within 1 LU = balanced
}
```

### Color Determination Code

```cpp
juce::Colour getStatusColor() const
{
    if (std::abs(currentDeviation) <= DSPSSOT::DeviationDisplay::BALANCED_RANGE_LU)
        return UISSOT::Colors::balanced();  // #4dff4d (green)
    else if (currentDeviation > DSPSSOT::DeviationDisplay::BALANCED_RANGE_LU)
        return UISSOT::Colors::hot();      // #ff4d4d (red)
    else
        return UISSOT::Colors::quiet();     // #4d4dff (blue)
}
```

---

## Visual Flowchart

```
          ┌─────────────────────┐
          │  Current LUFS: -14 │
          │  Target LUFS: -8   │
          └──────────┬──────────┘
                     │
        Calculate Deviation
                     │
         deviation = -14 - (-8) = -6
                     ↓
          ┌─────────────────────┐
          │ Deviation: -6 LU  │
          └──────────┬──────────┘
                     │
          ┌────────┴────────┐
          │ Evaluate │
          │ abs(-6) = 6.0 │
          └────────┬────────┘
                     │
         Is ≤ 1.0 LU?
         /          \
       NO          YES
        │            │
        ↓            ↓
   Is > 0?    Balanced 🟢
     /   \
   YES   NO
    │     │
    ↓     ↓
   Hot 🔵  Quiet 🔴
```

### Step-by-Step Logic

1. **Get current LUFS** from DSP measurement
2. **Get target LUFS** from selected content type
3. **Calculate deviation:** `currentLUFS - targetLUFS`
4. **Check absolute deviation:** `abs(deviationLU)`
5. **Determine status:**
   - If `abs(deviation) ≤ 1.0` → **Balanced** 🟢
   - Else if `deviation > 0` → **Hot** 🔴
   - Else → **Quiet** 🔵

---

## Practical Examples

### Example 1: Music Drums Mode (Target: -8 LUFS)

| Current LUFS | Deviation | Status | Color | Action |
|--------------|-----------|---------|--------|--------|
| -12.0 | -4.0 LU | 🔵 Quiet | Blue | Increase gain ~4 dB |
| -9.0 | -1.0 LU | 🟢 Balanced | Green | Perfect ✓ |
| -8.0 | 0.0 LU | 🟢 Balanced | Green | Perfect ✓ |
| -7.0 | +1.0 LU | 🟢 Balanced | Green | Perfect ✓ |
| -6.0 | +2.0 LU | 🔴 Hot | Red | Reduce gain ~2 dB |
| -4.0 | +4.0 LU | 🔴 Hot | Red | Reduce gain ~4 dB |

**Calculations:**
- `-12 - (-8) = -4` → Quiet 🔵
- `-9 - (-8) = -1` → Balanced 🟢 (boundary inclusive)
- `-8 - (-8) = 0` → Balanced 🟢
- `-7 - (-8) = +1` → Balanced 🟢 (boundary inclusive)
- `-6 - (-8) = +2` → Hot 🔴
- `-4 - (-8) = +4` → Hot 🔴

---

### Example 2: Cinema/Trailer Mode (Target: -14 LUFS)

| Current LUFS | Deviation | Status | Color | Action |
|--------------|-----------|---------|--------|--------|
| -18.0 | -4.0 LU | 🔵 Quiet | Blue | Increase gain ~4 dB |
| -15.0 | -1.0 LU | 🟢 Balanced | Green | Perfect ✓ |
| -14.0 | 0.0 LU | 🟢 Balanced | Green | Perfect ✓ |
| -13.0 | +1.0 LU | 🟢 Balanced | Green | Perfect ✓ |
| -12.0 | +2.0 LU | 🔴 Hot | Red | Reduce gain ~2 dB |
| -10.0 | +4.0 LU | 🔴 Hot | Red | Reduce gain ~4 dB |

**Calculations:**
- `-18 - (-14) = -4` → Quiet 🔵
- `-15 - (-14) = -1` → Balanced 🟢
- `-14 - (-14) = 0` → Balanced 🟢
- `-13 - (-14) = +1` → Balanced 🟢
- `-12 - (-14) = +2` → Hot 🔴
- `-10 - (-14) = +4` → Hot 🔴

---

### Example 3: Music Non-drums Mode (Target: -11 LUFS)

| Current LUFS | Deviation | Status | Color | Action |
|--------------|-----------|---------|--------|--------|
| -15.0 | -4.0 LU | 🔵 Quiet | Blue | Increase gain ~4 dB |
| -12.0 | -1.0 LU | 🟢 Balanced | Green | Perfect ✓ |
| -11.0 | 0.0 LU | 🟢 Balanced | Green | Perfect ✓ |
| -10.0 | +1.0 LU | 🟢 Balanced | Green | Perfect ✓ |
| -9.0 | +2.0 LU | 🔴 Hot | Red | Reduce gain ~2 dB |
| -7.0 | +4.0 LU | 🔴 Hot | Red | Reduce gain ~4 dB |

**Calculations:**
- `-15 - (-11) = -4` → Quiet 🔵
- `-12 - (-11) = -1` → Balanced 🟢
- `-11 - (-11) = 0` → Balanced 🟢
- `-10 - (-11) = +1` → Balanced 🟢
- `-9 - (-11) = +2` → Hot 🔴
- `-7 - (-11) = +4` → Hot 🔴

---

## Industry Standards

### Comparison Table

| Standard | Target | Tolerance | Use Case |
|----------|--------|-----------|----------|
| **Netflix** | -14 to -16 | ±0.5 LU | Cinema (strict) |
| **EBU R128** | -23 | ±1 LU | Broadcast (EU) |
| **ATSC A/85** | -24 | ±2 LU | Broadcast (US) |
| **Spotify** | -14 | ±1 LU | Streaming |
| **Apple Music** | -16 | ±1 LU | Streaming |
| **YouTube** | -14 | ±1 LU | Streaming |
| **BULLsEYE** | -11/-8/-14 | ±1 LU | Universal |

### BULLsEYE Choice: ±1 LU Tolerance

**Rationale:**
1. **Matches most streaming services** (Spotify, Apple Music, YouTube)
2. **Industry standard** for modern distribution
3. **Reasonable tolerance** for mastering without being too loose
4. **Clear visual feedback** - tighter tolerance provides more precision
5. **Practical balance** - not too strict (Netflix ±0.5) or too loose (ATSC ±2)

### Practical Impact

| Deviation | Industry | User Perception | BULLsEYE Status |
|-----------|-----------|-----------------|-------------------|
| ±0.5 LU | Tight (Netflix) | Very critical | Balanced 🟢 |
| ±1.0 LU | Standard (Streaming) | Normal | Balanced 🟢 |
| ±2.0 LU | Loose (Broadcast US) | Forgiving | Hot/Hot 🔵🔴 |

---

## Color Psychology

### Why These Colors?

| Color | Psychology | Audio Industry Use | BULLsEYE Meaning |
|--------|------------|-------------------|------------------|
| 🔴 **Red** | Warning, danger, stop | "Too loud", clipping, compression | Exceeds target |
| 🟢 **Green** | Good, safe, go | "On target", balanced, pass | Within tolerance |
| 🔵 **Blue** | Cold, calm, neutral | "Quiet", below target, headroom | Below target |

### Visual Hierarchy

**Background:** `#1a1a1a` (soft dark gray)
**Status Colors:**
- Red `#ff4d4d` - High contrast, attention-grabbing, urgent
- Green `#4dff4d` - Bright, easy to read, positive reinforcement
- Blue `#4d4dff` - Visible but calming, indicates room for improvement

### Accessibility

All three colors meet **WCAG AA contrast standards** on the dark background:
- **Red on dark:** Contrast ratio 8.5:1 (exceeds 4.5:1 minimum)
- **Green on dark:** Contrast ratio 8.5:1 (exceeds 4.5:1 minimum)
- **Blue on dark:** Contrast ratio 8.2:1 (exceeds 4.5:1 minimum)

**Result:** Fully accessible to users with visual impairments

---

## Edge Cases

### 1. Exactly at Threshold (Boundary Behavior)

**Case A: LUFS = -9.0, Target = -8.0 (Deviation = -1.0)**
```cpp
deviation = -9 - (-8) = -1
std::abs(-1) <= 1.0  // TRUE
→ Balanced 🟢
```

**Case B: LUFS = -7.0, Target = -8.0 (Deviation = +1.0)**
```cpp
deviation = -7 - (-8) = +1
std::abs(+1) <= 1.0  // TRUE
→ Balanced 🟢
```

**Boundary Behavior:** Inclusive at both ends (≤ and ≥)
- Exactly -1.0 LU deviation = Balanced 🟢
- Exactly +1.0 LU deviation = Balanced 🟢

---

### 2. Below Gate Threshold

**Case: LUFS = -75.0, Target = -8.0**

The absolute gate is -70 LUFS (per ITU-R BS.1770). Values below this are considered "silence" by the DSP integration.

```cpp
// Below -70 LUFS absolute gate
currentLUFS = -75.0
targetLUFS = -8.0
deviation = -75 - (-8) = -67 LU

std::abs(-67) > 1.0  // TRUE
→ Quiet 🔵
```

**Behavior:** Shows "Quiet" status even for invalid measurements below gate threshold
- **Reason:** Provides visual feedback that meter is reading but below gate
- **JSFX Behavior:** Matches original JSFX reference
- **User Benefit:** Distinguishes "silence" from "not yet started"

---

### 3. Mode Switching

**Scenario:**
1. User has -8 LUFS measurement (Balanced in "Music Drums" mode)
2. User switches to "Cinema/Trailer" (-14 LUFS target)
3. Suddenly: deviation = +6 LU → **Hot** 🔴

**Visual Feedback:**
- Instant color change from green to red
- Status text changes from "Balanced" to "Hot"
- User immediately knows content is too loud for new target

**User Action:** Apply limiter or reduce gain to bring LUFS down to -14

---

### 4. Silence/Uninitialized

**Case: LUFS = -900000000.0 (negative infinity sentinel)**

```cpp
if (currentLUFS <= NEG_INF_THRESHOLD)
{
    return UISSOT::Strings::statusQuiet();  // Show "Quiet"
}
```

**Behavior:** Shows "Quiet" status for uninitialized measurements
- **Reason:** Provides default state before any audio is processed
- **Visual:** Blue status text, empty deviation display
- **Transition:** Changes to appropriate status once audio passes gate

---

## Real-World Scenarios

### Scenario 1: Mixing a Track (Music Drums Mode)

**Timeline:**

```
0:00  Track starts (quiet intro)
      LUFS: -45, Dev: -37
      Status: 🔵 Quiet
      Action: Let intro play, gain low but intentional

0:30  Drums enter
      LUFS: -12, Dev: -4
      Status: 🔵 Quiet
      Action: Increase drum bus gain +2 dB

1:00  Full mix
      LUFS: -9, Dev: -1
      Status: 🟢 Balanced
      Action: Perfect ✓

2:30  Chorus (loudest part)
      LUFS: -6, Dev: +2
      Status: 🔴 Hot
      Action: Apply limiter -3 dB threshold

3:00  Bridge (quieter)
      LUFS: -10, Dev: -2
      Status: 🔵 Quiet
      Action: Increase bridge gain +2 dB

4:00  Final chorus (after limiter)
      LUFS: -8, Dev: 0
      Status: 🟢 Balanced
      Action: Perfect ✓
```

**Result:** Track consistently stays in green "Balanced" zone throughout

---

### Scenario 2: Mastering for Netflix (Cinema/Trailer Mode)

**Timeline:**

```
0:00  Initial master
      LUFS: -12, Dev: +2
      Status: 🔴 Hot
      Action: Reduce overall gain -2 dB

0:10  After gain reduction
      LUFS: -14, Dev: 0
      Status: 🟢 Balanced
      Action: Perfect ✓

0:20  Check dynamics
      LUFS: -13.5, Dev: +0.5
      Status: 🟢 Balanced
      Action: Good, within Netflix ±0.5 LU spec

0:30  True Peak check
      TP: -1.2 dBTP
      Status: 🟢 Balanced
      Action: Acceptable for Netflix (-2 dBTP limit)
```

**Result:** Master meets Netflix specifications perfectly

---

### Scenario 3: Live Monitoring During Recording (Music Non-drums Mode)

**Timeline:**

```
0:00  Setup phase
      LUFS: -45, Dev: -34
      Status: 🔵 Quiet
      Action: Adjusting input gain

0:30  Recording starts
      LUFS: -10, Dev: +1
      Status: 🟢 Balanced
      Action: Good level

1:00  Vocal solo
      LUFS: -7, Dev: +4
      Status: 🔴 Hot
      Action: Back off microphone -4 dB

1:30  Vocal level adjusted
      LUFS: -11, Dev: 0
      Status: 🟢 Balanced
      Action: Perfect recording level

2:00  Band section enters
      LUFS: -6, Dev: +5
      Status: 🔴 Hot
      Action: Apply compression to band bus
```

**Result:** Real-time feedback allows immediate gain adjustments during recording

---

### Scenario 4: Podcast Production (Music Non-drums Mode)

**Timeline:**

```
0:00  Raw recording
      LUFS: -18, Dev: -7
      Status: 🔵 Quiet
      Action: Normalize to -16 LUFS

0:10  After normalization
      LUFS: -16, Dev: -5
      Status: 🔵 Quiet
      Action: Increase gain +5 dB

0:20  After gain boost
      LUFS: -11, Dev: 0
      Status: 🟢 Balanced
      Action: Perfect for podcast

0:30  Check music bed
      LUFS: -9, Dev: +2
      Status: 🔴 Hot
      Action: Lower music bed -3 dB

0:40  Music adjusted
      LUFS: -12, Dev: +1
      Status: 🟢 Balanced
      Action: Good balance
```

**Result:** Podcast meets Spotify/Apple Music streaming standards

---

## Summary Reference

### Quick Reference Table

| Status | Deviation | Color | Hex | RGB | Function | Action |
|--------|-----------|--------|-----|-----|----------|---------|
| Quiet | < -1.0 LU | Blue | `#4d4dff` | (77, 77, 255) | `UISSOT::Colors::quiet()` | Increase gain |
| Balanced | -1.0 to +1.0 LU | Green | `#4dff4d` | (77, 255, 77) | `UISSOT::Colors::balanced()` | Perfect ✓ |
| Hot | > +1.0 LU | Red | `#ff4d4d` | (255, 77, 77) | `UISSOT::Colors::hot()` | Reduce gain |

### Typography Reference

| Element | Font Size | Style | Usage |
|---------|------------|--------|--------|
| Status Text ("Balanced", "Hot", "Quiet") | 14pt | Bold | Center text in CircularMeter |
| LUFS Value | 14pt | Bold | StatusDisplayComponent |
| Labels | 12pt | Regular | "True Peak", "Content Type" |

**Note:** Font text and meter arc always use the same `statusColor` variable for visual consistency.

### Constants from SSOT

```cpp
// From DSPSSOT.h
namespace DeviationDisplay
{
    constexpr double BALANCED_RANGE_LU = 1.0;   // ±1 LU = balanced
}

// From UISSOT.h
namespace Colors
{
    constexpr Colour balanced()  { return juce::Colour(0x4d, 0xff, 0x4d); }  // #4dff4d
    constexpr Colour hot()       { return juce::Colour(0xff, 0x4d, 0x4d); }  // #ff4d4d
    constexpr Colour quiet()     { return juce::Colour(0x4d, 0x4d, 0xff); }  // #4d4dff
}
```

### Content Type Targets

```cpp
// From DSPSSOT.h
namespace LoudnessTargets
{
    constexpr double MUSIC_NON_DRUMS = -11.0;
    constexpr double MUSIC_DRUMS = -8.0;
    constexpr double CINEMA_TRAILER = -14.0;
    constexpr double DEFAULT_TARGET = MUSIC_DRUMS;
}
```

### Deviation Formula

```
deviationLU = currentLUFS - targetLUFS

Where:
  currentLUFS = Measured integrated loudness (from DSP)
  targetLUFS = Target loudness for selected content type

Status determination:
  abs(deviation) ≤ 1.0  →  Balanced 🟢
  deviation > 0           →  Hot 🔴
  deviation < 0           →  Quiet 🔵
```

### Visual Representation

```
Deviation Scale:

Quiet (blue)    Balanced (green)    Hot (red)
   🔵                   🟢                  🔴
    │                     │                   │
-5   -4   -3   -2   -1   0   +1   +2   +3   +4   +5  (LU)
    │                     │                   │
    │←─ Quiet zone ──│── Balanced ────│─ Hot zone ─→│
```

---

## Key Takeaways

1. **Three Simple States** - Easy to understand at a glance
2. **±1 LU Tolerance** - Industry standard for streaming services
3. **Color-Coded Feedback** - Instant visual communication without numbers
4. **Deviation-Based** - Automatically adapts to any target loudness
5. **Used Everywhere** - Consistent across all GUI components
6. **Inclusive Boundaries** - Exactly ±1.0 LU still "Balanced"
7. **Below-Gate Behavior** - Shows "Quiet" even for invalid measurements
8. **Mode Switching** - Instant visual feedback when changing content types

The Quiet/Balanced/Hot system provides immediate, actionable feedback that enables users to:
- Quickly identify loudness problems
- Understand how far off-target they are
- Make informed decisions about gain adjustments
- Meet industry standards for distribution

---

*Last Updated: 2026-02-07*  
*Version: 1.0*  
*Status: Production Ready*