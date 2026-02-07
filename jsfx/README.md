# BULLsEYE - Mini Combo

**LUFS-I Gated Loudness Meter with True Peak Detection**

A compact JSFX plugin for precise audio loudness measurement, featuring ITU-R BS.1770 compliant K-weighting, gated integration, and 4x oversampling True Peak detection.

## Overview

BULLsEYE provides accurate loudness metering in a minimal interface, designed for mixing and mastering workflows where visual clarity is essential. The plugin implements industry-standard loudness measurement techniques while remaining lightweight and responsive.

The plugin operates in three distinct modes optimized for different content types, automatically adjusting target loudness values to match common industry standards.

## Features

**Dual-Mode Loudness Measurement**
- Real-time LUFS-I (Integrated Loudness) display with gated measurement
- Live meter showing current signal loudness relative to target
- ITU-R BS.1770 compliant K-weighting (60 Hz high-pass, 4 kHz high-shelf +4 dB)

**Advanced True Peak Detection**
- 4x oversampling using Hermite interpolation for sub-sample accuracy
- True Peak metering up to -120 dB resolution
- Peak hold functionality for reliable True Peak identification

**Visual Feedback System**
- Deviation bar showing offset from target loudness in LU (Loudness Units)
- Color-coded indicators: green for balanced, red for hot, blue for quiet
- Smooth visual updates for rapid level assessment

## Parameters

### slider1: Content Type

Selects the reference mode for loudness targeting:

| Mode | Target LUFS-I | Application |
|------|---------------|-------------|
| 0 | -11 LUFS | Music Non-drums |
| 1 | -8 LUFS | Music Drums |
| 2 | -14 LUFS | Cinema/Trailer |

The selected mode affects only the target reference used for deviation display. The underlying loudness measurement algorithm remains consistent across all modes.

## Display Elements

### Live LUFS-I

The primary meter displays the current integrated loudness value in LUFS. During signal absence or levels below the gate threshold, the display shows "--.-" to indicate no valid measurement.

### Mix Energy Bar

A horizontal deviation bar provides at-a-glance assessment of loudness deviation:

- **Center Position**: Signal matches target loudness
- **Left of Center**: Signal below target (Quiet)
- **Right of Center**: Signal above target (Hot)
- **Green Zone**: Within 1 LU of target (Balanced)
- **Red/Blue Indicators**: Signal exceeds 1 LU deviation

The bar represents a ±5 LU range, clamping extreme values at the edges for readability.

### True Peak

Displays the highest detected True Peak value in dBTP (True Peak decibels). The measurement uses 4x oversampling with Hermite interpolation for accurate peak detection between samples.

## Technical Implementation

### K-Weighting Filters

The plugin applies a two-stage K-weighting filter to each channel:

**High-Pass Stage (60 Hz, Q=0.5)**
Removes low-frequency content below the hearing threshold used in loudness measurement standards, following ITU-R BS.1770 specifications.

**High-Shelf Stage (4 kHz, Q=0.707, +4 dB)**
Applies the standardized high-frequency weighting that approximates equal-loudness contours, ensuring frequency-weighted loudness correlation with human perception.

### Gated Integration

The measurement system operates on 400 ms integration blocks with dual-threshold gating:

**Absolute Gate (-70 LUFS)**
Ignores any block below this threshold, eliminating noise floor contributions from system or environmental sources.

**Relative Gate (L_int - 10 LU)**
Applies a dynamic threshold 10 LU below the current integrated loudness, following the EBU R128 recommendation for gated loudness measurement.

Blocks exceeding either threshold contribute to the integrated measurement, accumulating energy and sample counts until a valid measurement stabilizes.

### True Peak Detection

For each audio sample, the True Peak detector:

1. Buffers four consecutive samples (l0 through l3)
2. Generates three interpolated points between samples using Hermite interpolation (t=0.25, 0.50, 0.75)
3. Evaluates absolute peak across original and interpolated samples
4. Tracks maximum peak across all processed samples
5. Converts linear peak value to decibel representation

The Hermite interpolation formula produces smooth curves through sample points, revealing inter-sample peaks that would otherwise escape detection.

## Usage Recommendations

### Mixing

Set the content type to match your target delivery format. During mixing, use the deviation bar to maintain consistent loudness levels across the stereo field, adjusting faders to center the indicator.

### Mastering

For mastering applications, monitor the integrated value closely. Small deviations (±0.5 LU) typically fall within acceptable mastering tolerances, while larger deviations suggest targeted level adjustment.

### Broadcast Preparation

Select the Cinema/Trailer mode when preparing content for broadcast delivery, as -14 LUFS aligns with common broadcast loudness standards.

## Performance Considerations

The plugin performs all calculations sample-by-sample, ensuring accurate measurement regardless of buffer size or latency settings. The 400 ms integration block provides sufficient smoothing for readable meters without excessive delay.

Memory usage remains minimal with fixed-size coefficient arrays and a small sample buffer for True Peak interpolation. The plugin operates efficiently across all supported sample rates without recomputation.

## Version History

**1.2.1** - Initial release with LUFS-I gated measurement, True Peak 4x detection, and three content type modes.
