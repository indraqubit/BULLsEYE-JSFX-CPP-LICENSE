# BULLsEYE DAW Testing Guide

**Comprehensive testing procedures for BULLsEYE LUFS-I Gated Loudness Meter**

This document provides complete testing procedures for validating the BULLsEYE plugin across different Digital Audio Workstations (DAWs). It includes installation verification, functional testing checklists, audio reference specifications, and comparison methodologies between the JSFX and JUCE implementations.

---

## 1. Plugin Artifacts Verification

Before commencing DAW testing, verify that the plugin artifacts are correctly installed and meet expected specifications. This section outlines the installation paths, file sizes, and verification procedures.

### 1.1 Installation Paths

The BULLsEYE plugin installs to standard Audio Unit and VST3 locations on macOS. The primary installation path for Audio Units is `~/Library/Audio/Plug-Ins/Components/BULLsEYE.component/`, while VST3 plugins install to `~/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3/`. The build system generates these artifacts automatically during the compilation process, with the Audio Unit build located at `build/BULLsEYE_artefacts/Release/AU/BULLsEYE.component/` and the VST3 build at `build/BULLsEYE_artefacts/Release/VST3/BULLsEYE.vst3/`.

Both formats should be present and functional for comprehensive testing across different DAWs. REAPER supports both AU and VST3 formats natively, while Ableton Live 11 and later versions support AU plugins on macOS. Users running older versions of Ableton Live may need to use VST3 format if compatibility is confirmed.

### 1.2 File Size Verification

| Plugin Version | File Path | Size | Expected Range | Status |
|----------------|-----------|------|----------------|--------|
| Release AU | `BULLsEYE.component/Contents/MacOS/BULLsEYE` | 8.3 MB | 8-10 MB | Verified |
| Release VST3 | `BULLsEYE.vst3/Contents/MacOS/BULLsEYE` | 8.9 MB | 8-10 MB | Expected |
| Debug AU | `BULLsEYE1.component/Contents/MacOS/BULLsEYE1` | 73 MB | 70-80 MB | Optional |

The release build sizes (8-10 MB) are consistent with a typical JUCE 8 plugin containing the full DSP implementation, K-weighting filters, and True Peak detection algorithm. The significantly larger debug build (73 MB) includes additional debugging symbols and is intended only for development purposes. For production testing, always use the release build to ensure accurate performance measurements.

### 1.3 Verification Commands

Run the following commands to verify plugin installation:

```bash
# Verify Audio Unit installation
ls -lh ~/Library/Audio/Plug-Ins/Components/BULLsEYE.component/Contents/MacOS/BULLsEYE

# Verify VST3 installation
ls -lh ~/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3/Contents/MacOS/BULLsEYE

# List all installed BULLsEYE variants
ls -lh ~/Library/Audio/Plug-Ins/Components/BULLsEYE*.component
```

The output should confirm the existence of the plugin binaries with sizes within the expected ranges. If files are missing or sizes are significantly different, rebuild the project using the provided build script.

---

## 2. REAPER DAW Testing Checklist

REAPER provides excellent support for both Audio Unit and VST3 plugins, making it an ideal DAW for initial validation. This section details comprehensive testing procedures covering plugin loading, parameter interaction, preset management, and audio processing validation.

### 2.1 Plugin Loading and Initialization

Before testing audio processing, verify that the plugin loads correctly and initializes its internal state properly. This validation ensures that the plugin can handle DAW-specific loading sequences and that all UI components initialize correctly.

| Test ID | Test Description | Expected Result | Pass/Fail |
|---------|------------------|-----------------|-----------|
| REAP-AU-01 | Load BULLsEYE as Audio Unit on empty track | Plugin appears in FX chain, no errors | [ ] |
| REAP-VST-01 | Load BULLsEYE as VST3 on empty track | Plugin appears in FX chain, no errors | [ ] |
| REAP-INIT-01 | Verify plugin window opens on double-click | GUI displays within 2 seconds | [ ] |
| REAP-INIT-02 | Confirm default content type selection | "Music Drums (-8)" selected by default | [ ] |
| REAP-INIT-03 | Verify initial meter readings | Live LUFS-I shows "--.-" before audio | [ ] |

The default content type should match the ref_mode=1 setting from the JSFX source, which corresponds to "Music Drums (-8 LUFS)". If the wrong content type appears, check the ProcessorSSOT.h file for the default parameter value and verify that the APVTS correctly initializes the parameter.

### 2.2 Audio Track Insertion Testing

Testing plugin insertion on audio tracks validates that the signal flow is correct and that the passthrough behavior matches specifications. The BULLsEYE plugin should pass audio through unchanged while performing measurements on the input signal.

| Test ID | Test Description | Expected Result | Pass/Fail |
|---------|------------------|-----------------|-----------|
| REAP-INS-01 | Insert on mono audio track | Plugin accepts mono input, converts to stereo output | [ ] |
| REAP-INS-02 | Insert on stereo audio track | Stereo passthrough maintains channel integrity | [ ] |
| REAP-INS-03 | Verify no audio coloration | Output matches input within 0.01 dB | [ ] |
| REAP-INS-04 | Test at 44.1 kHz sample rate | All measurements function correctly | [ ] |
| REAP-INS-05 | Test at 48 kHz sample rate | All measurements function correctly | [ ] |
| REAP-INS-06 | Test at 96 kHz sample rate | All measurements function correctly | [ ] |
| REAP-INS-07 | Test at 192 kHz sample rate | All measurements function correctly | [ ] |

The plugin must support all standard sample rates from 44.1 kHz through 192 kHz. The DSP implementation uses sample-rate-independent calculations for the K-weighting filters and True Peak detection, so measurements should remain consistent across sample rates when identical audio is processed.

### 2.3 FX Chain Position Testing

The position of BULLsEYE in the FX chain can affect its measurements. Test the plugin in various chain positions to understand its behavior and document any differences.

| Test ID | Chain Position | Test Description | Expected Result | Pass/Fail |
|---------|----------------|------------------|-----------------|-----------|
| REAP-CHAIN-01 | First in chain | Plugin sees unprocessed signal | Raw input levels measured | [ ] |
| REAP-CHAIN-02 | After EQ | Plugin sees EQ-processed signal | EQ effect visible in measurements | [ ] |
| REAP-CHAIN-03 | After compressor | Plugin sees compressed signal | Compression effect visible in measurements | [ ] |
| REAP-CHAIN-04 | After limiter | Plugin sees limited signal | Limiting effect visible in True Peak | [ ] |
| REAP-CHAIN-05 | Last in chain | Plugin sees final mix output | Final mix levels measured | [ ] |

For standard loudness metering workflows, BULLsEYE should be positioned after all dynamics processing (compression, limiting) but before any dithering or output limiting. This configuration provides accurate measurements of the final program loudness while avoiding measurement of dither noise.

### 2.4 Parameter Change Testing

Testing parameter changes ensures that the UI correctly communicates with the DSP layer and that measurements update appropriately when content type changes.

| Test ID | Parameter | Test Description | Expected Result | Pass/Fail |
|---------|-----------|------------------|-----------------|-----------|
| REAP-PARAM-01 | Content Type | Switch to "Music Non-drums (-11)" | Target changes to -11 LUFS, measurements adjust | [ ] |
| REAP-PARAM-02 | Content Type | Switch to "Music Drums (-8)" | Target changes to -8 LUFS, measurements adjust | [ ] |
| REAP-PARAM-03 | Content Type | Switch to "Cinema/Trailer (-14)" | Target changes to -14 LUFS, measurements adjust | [ ] |
| REAP-PARAM-04 | Parameter automation | Automate content type changes | Smooth transitions, no clicks or glitches | [ ] |
| REAP-PARAM-05 | UI responsiveness | Rapid parameter changes | UI updates without lag or freezing | [ ] |

The content type parameter affects only the target loudness value and the display labels. The underlying measurement algorithm (K-weighting, gated integration, True Peak detection) remains unchanged regardless of content type selection.

### 2.5 Preset Save and Load Testing

Preset management testing validates that the plugin correctly saves and restores its state, including content type selection and any future user preferences.

| Test ID | Test Description | Expected Result | Pass/Fail |
|---------|------------------|-----------------|-----------|
| REAP-PRESET-01 | Save preset with default settings | Preset file created, contains correct state | [ ] |
| REAP-PRESET-02 | Load saved preset | Plugin restores to saved state | [ ] |
| REAP-PRESET-03 | Change settings, reload preset | Settings return to saved values | [ ] |
| REAP-PRESET-04 | Preset metadata | Preset name and bank information stored correctly | [ ] |
| REAP-PRESET-05 | Factory presets | Default presets load correctly | [ ] |

### 2.6 Project Save and Reload Testing

Project persistence testing ensures that the plugin state is correctly saved within DAW project files and restored when projects are reopened.

| Test ID | Test Description | Expected Result | Pass/Fail |
|---------|------------------|-----------------|-----------|
| REAP-PROJ-01 | Save project with BULLsEYE inserted | Project file contains plugin state | [ ] |
| REAP-PROJ-02 | Close and reopen project | Plugin restores to previous state | [ ] |
| REAP-PROJ-03 | Test with multiple instances | Each instance maintains independent state | [ ] |
| REAP-PROJ-04 | Test with different track types | State preserved across track types | [ ] |
| REAP-PROJ-05 | Test project copy/move | Plugin state preserved in transferred project | [ ] |

### 2.7 Meter Response Validation

The meter response tests validate that the visual displays accurately reflect the underlying measurements and respond appropriately to different audio signals.

| Test ID | Test Description | Expected Result | Pass/Fail |
|---------|------------------|-----------------|-----------|
| REAP-METER-01 | Silent input | Live LUFS-I shows "--.-" | [ ] |
| REAP-METER-02 | -20 dBFS sine at 1kHz | LUFS-I settles to -20 ±0.5 LUFS | [ ] |
| REAP-METER-03 | -14 dBFS pink noise | LUFS-I settles to -14 ±0.5 LUFS | [ ] |
| REAP-METER-04 | Clipping signal | True Peak shows 0 dBTP | [ ] |
| REAP-METER-05 | Gradual loudness change | Meter updates smoothly, no jumping | [ ] |
| REAP-METER-06 | Meter reset | Manual reset clears accumulated values | [ ] |

The LUFS-I measurement uses a gated integration window of 400ms, so the meter should display valid readings after approximately 400-800ms of audio input. The True Peak measurement continuously updates and displays the maximum peak detected since the measurement started or was last reset.

### 2.8 Content Type Switching Validation

Content type switching tests verify that the target loudness changes correctly and that the deviation indication updates appropriately.

| Test ID | Test Description | Expected Result | Pass/Fail |
|---------|------------------|-----------------|-----------|
| REAP-CONT-01 | -14 LUFS target with -14 LUFS input | Deviation shows "Balanced" (0 ±1 LU) | [ ] |
| REAP-CONT-02 | -11 LUFS target with -8 LUFS input | Deviation shows "Hot" (+3 LU) | [ ] |
| REAP-CONT-03 | -8 LUFS target with -14 LUFS input | Deviation shows "Quiet" (-6 LU) | [ ] |
| REAP-CONT-04 | Deviation bar visualization | Indicator moves with loudness changes | [ ] |
| REAP-CONT-05 | Deviation color coding | Green for Balanced, Red for Hot, Blue for Quiet | [ ] |

The deviation display uses a ±1 LU tolerance band for the "Balanced" indication. Signals within 1 LU of the target display in green, signals more than 1 LU below target display in blue, and signals more than 1 LU above target display in red.

---

## 3. Ableton Live DAW Testing Checklist

Ableton Live has specific plugin loading behaviors and UI conventions that differ from REAPER. This section provides Ableton-specific testing procedures to ensure full compatibility.

### 3.1 Plugin Loading and Initialization

Ableton Live loads plugins in a slightly different manner than REAPER, with specific requirements for plugin validation and initialization.

| Test ID | Test Description | Expected Result | Pass/Fail |
|---------|------------------|-----------------|-----------|
| ABL-AU-01 | Load BULLsEYE in AU format | Plugin appears in Audio Effect browser | [ ] |
| ABL-AU-02 | Drag to track, insert on audio track | Plugin loads, displays correctly | [ ] |
| ABL-AU-03 | Verify plugin window opens | Resizable window displays properly | [ ] |
| ABL-AU-04 | Test on MIDI track | Plugin processes (but no audio expected) | [ ] |
| ABL-AU-05 | Test on return track | Plugin functions on bus signal | [ ] |

Ableton Live requires Audio Units to be rescanned when new plugins are installed. If BULLsEYE does not appear in the browser, use Live's "Rescan Plugins" function from the Preferences > Plugins menu.

### 3.2 Audio Processing Validation

| Test ID | Test Description | Expected Result | Pass/Fail |
|---------|------------------|-----------------|-----------|
| ABL-PROC-01 | Audio passthrough | Output matches input, no coloration | [ ] |
| ABL-PROC-02 | Latency reporting | Plugin reports correct latency (0 samples) | [ ] |
| ABL-PROC-03 | Tail handling | Plugin processes audio tails correctly | [ ] |
| ABL-PROC-04 | Session/arrangement view | Plugin state preserved in both views | [ ] |
| ABL-PROC-05 | Warping interaction | No interference with warping algorithms | [ ] |

The BULLsEYE plugin is a measurement-only tool and does not add processing latency. The plugin reports zero latency, which is correct for a meter that performs real-time measurements without lookahead.

### 3.3 Preset and Project Management

| Test ID | Test Description | Expected Result | Pass/Fail |
|---------|------------------|-----------------|-----------|
| ABL-PRESET-01 | Save as Live preset | .adg file created with correct state | [ ] |
| ABL-PRESET-02 | Load saved preset | Plugin restores to saved settings | [ ] |
| ABL-PRESET-03 | Drag preset to different track | Settings transfer correctly | [ ] |
| ABL-PROJ-01 | Save Live set with BULLsEYE | Project saves correctly | [ ] |
| ABL-PROJ-02 | Close and reopen Live set | Plugin restores to previous state | [ ] |

Ableton Live uses a different preset format (.adg files) compared to REAPER. Test both DAW-specific preset formats to ensure compatibility across workflows.

### 3.4 Ableton-Specific Behaviors

| Test ID | Test Description | Expected Result | Pass/Fail |
|---------|------------------|-----------------|-----------|
| ABL-UI-01 | Window resizing | GUI scales correctly at different sizes | [ ] |
| ABL-UI-02 | HiDPI rendering | Interface looks sharp on Retina displays | [ ] |
| ABL-UI-03 | CPU meter impact | Plugin uses reasonable CPU resources | [ ] |
| ABL-UI-04 | Plugin bridge | Plugin works in 32-bit bridge if needed | [ ] |
| ABL-UI-05 | Automation recording | Parameter automation records correctly | [ ] |

### 3.5 Live-Specific Limitations

Be aware of the following Ableton Live-specific behaviors that may differ from other DAWs:

- **Plugin Scanning**: Live may require manual rescanning after plugin installation
- **VST3 Support**: Older Live versions may not support VST3 format; use AU for maximum compatibility
- **Automation Display**: Live's automation lanes may display parameter changes differently than REAPER
- **Plugin Window**: Live creates plugin windows in a separate process space on some systems

---

## 4. Test Audio Reference Document

This section documents recommended test signals for validating BULLsEYE measurements. Each test signal serves a specific purpose in verifying the plugin's accuracy and behavior.

### 4.1 Silence Signal

**Purpose**: Test gate behavior and zero-signal handling

**Expected Behavior**: Live LUFS-I should display "--.-" or equivalent "no signal" indication when input is below the gate threshold. The gate uses an absolute threshold of -70 LUFS and a relative threshold of L-10 (target minus 10 LUFS).

**Generation Command**:
```bash
# Generate 10 seconds of silence at 44.1 kHz
sox -n silence.wav synth 10.0 sin 0

# Generate silence with fade in/out
sox -n silence_fade.wav synth 10.0 sin 0 fade t 0.5 9.0 0.5
```

**Validation Criteria**: The meter should not display false positive readings on silence. The gate should effectively reject all content below -70 LUFS absolute or the relative threshold.

### 4.2 Pink Noise Signal

**Purpose**: Test K-weighting filters and integrated loudness measurement

**Expected Behavior**: Pink noise at -20 dBFS should measure at approximately -20 LUFS before K-weighting, and approximately -20.7 LUFS after K-weighting (accounting for the K-Offset of -0.691 dB). The deviation from flat spectrum is due to the pre-filter applied to match human loudness perception.

**Generation Command**:
```bash
# Generate pink noise at -20 dBFS for 60 seconds
sox -n -b 24 pink_noise_20dbfs.wav synth 60.0 pinknoise gain -20

# Generate pink noise at -14 dBFS for 60 seconds
sox -n -b 24 pink_noise_14dbfs.wav synth 60.0 pinknoise gain -14
```

**Validation Criteria**: The measured LUFS-I should be within ±0.5 LU of the expected value after accounting for the K-weighting filter effect. Use a reference meter (such as Youlean Loudness Meter or iZotope RX) to verify expected values.

### 4.3 Sine Wave Signals

**Purpose**: Test True Peak detection and sample-accurate peak measurement

**Expected Behavior**: A full-scale sine wave at 0 dBFS should display as 0 dBTP True Peak. A sine wave at -6 dBFS should display as -6 dBTP. The True Peak measurement uses 4x Hermite interpolation to detect inter-sample peaks that may not be visible in the samples themselves.

**Generation Commands**:
```bash
# Generate 1 kHz sine at -3 dBFS for 30 seconds
sox -n -b 24 sine_1k_neg3dbfs.wav synth 30.0 sin 1000 gain -3

# Generate 1 kHz sine at 0 dBFS (maximum level) - use with caution
sox -n -b 24 sine_1k_0dbfs.wav synth 30.0 sin 1000 gain 0

# Generate 10 kHz sine for high-frequency True Peak testing
sox -n -b 24 sine_10k_0dbfs.wav synth 30.0 sin 10000 gain 0
```

**Validation Criteria**: The True Peak display should show 0 dBTP for full-scale signals. Higher frequency sine waves are more likely to exhibit inter-sample peaks due to the interaction between the waveform and the sample grid.

### 4.4 Program Material Signals

**Purpose**: Test integrated loudness measurement with real-world content

**Expected Behavior**: Program material with varying dynamics should show stable LUFS-I readings once the gated integration has accumulated sufficient samples. The measurement should update smoothly as the audio progresses.

**Recommended Test Material**:

| Source Type | Expected LUFS | Duration | Purpose |
|-------------|---------------|----------|---------|
| Pop music mix | -8 to -6 LUFS | 3:00+ | Standard music reference |
| Acoustic ballad | -14 to -12 LUFS | 2:30+ | Quiet content testing |
| Film dialogue | -24 LUFS | 1:00+ | Dialogue-level testing |
| Trailer/movie | -10 to -6 LUFS | 0:30 | High-dynamic content |
| Podcast | -16 to -14 LUFS | 1:00+ | Voice-centric content |

**Validation Criteria**: Compare BULLsEYE readings against a known-good reference meter. Differences should be within ±0.5 LU for well-mixed program material with appropriate gating behavior.

### 4.5 Dynamic Range Test Signals

**Purpose**: Test gated integration behavior with varying dynamics

**Expected Behavior**: Signals with significant dynamic range should trigger the gate correctly, excluding quiet passages from the integrated measurement.

**Test Signal Sequence**:

| Segment | Duration | Level | Purpose |
|---------|----------|-------|---------|
| Loud section | 5 seconds | -8 LUFS | Primary integration content |
| Quiet section | 2 seconds | -40 LUFS | Should be gated out |
| Loud section | 5 seconds | -8 LUFS | Primary integration content |
| Very quiet | 2 seconds | -70 LUFS | At absolute gate threshold |
| Loud finish | 5 seconds | -8 LUFS | Primary integration content |

**Validation Criteria**: The LUFS-I reading should reflect primarily the loud sections, with the quiet passages excluded by the gate. The gate uses absolute threshold of -70 LUFS and relative threshold of L-10 (target minus 10 LUFS).

---

## 5. JSFX vs JUCE Comparison Methodology

Comparing the JSFX and JUCE implementations ensures that the migration preserved measurement accuracy. This section documents the comparison methodology and expected tolerances.

### 5.1 Measurement Comparison Approach

Both implementations use the same underlying algorithms, but the comparison must account for differences in floating-point precision and computational approach. The JSFX version uses single-precision floating point, while the JUCE version uses double-precision floating point in the DSP core.

| Measurement Type | JSFX Precision | JUCE Precision | Expected Difference |
|------------------|----------------|----------------|---------------------|
| K-weighting | 32-bit float | 64-bit double | < 0.01 LU |
| Gated integration | 32-bit float | 64-bit double | < 0.01 LU |
| True Peak (4x Hermite) | 32-bit float | 64-bit double | < 0.1 dB |
| LUFS-I calculation | 32-bit float | 64-bit double | < 0.01 LU |

The tighter tolerance for True Peak (0.1 dB) reflects the sensitivity of peak detection to computational precision, particularly for signals near the clipping threshold.

### 5.2 Test Signal Comparison Protocol

**Step 1**: Generate test signals at 44.1 kHz sample rate using the commands in Section 4.

**Step 2**: Load test signals in REAPER with both JSFX and JUCE versions.

**Step 3**: Record LUFS-I and True Peak readings from both versions after steady-state is reached.

**Step 4**: Calculate differences and verify against tolerance thresholds.

**Step 5**: Repeat at 48 kHz, 96 kHz, and 192 kHz sample rates.

**Step 6**: Document any systematic differences in a comparison table.

### 5.3 Acceptance Criteria

| Measurement | Maximum Allowed Difference | Notes |
|-------------|---------------------------|-------|
| LUFS-I (noise) | ±0.2 LU | Small differences acceptable due to gate behavior |
| LUFS-I (program) | ±0.5 LU | Larger tolerance for complex signals |
| True Peak (sine) | ±0.1 dB | Tight tolerance for pure tones |
| True Peak (program) | ±0.3 dB | Wider tolerance for complex signals |
| Gate behavior | 100% match | Gate thresholds must be identical |

### 5.4 Known Differences and Workarounds

**UI Rendering Differences**: The JSFX version uses native JSFX graphics, while the JUCE version uses JUCE's Component architecture. Visual differences in the bar meter positioning or text rendering are cosmetic and do not affect measurements.

**Update Rate Differences**: The JSFX version updates graphics every @gfx cycle, while the JUCE version updates at the native frame rate. Minor timing differences in visual updates are expected and do not indicate measurement differences.

**Parameter Initialization**: The JSFX version initializes ref_mode=1 on @init, while the JUCE version may use APVTS defaults. Ensure both versions use identical initial states before comparison.

### 5.5 Comparison Data Recording Template

| Test Signal | JSFX LUFS | JUCE LUFS | Difference | JSFX TP | JUCE TP | TP Difference | Pass/Fail |
|-------------|-----------|-----------|------------|---------|---------|---------------|-----------|
| Silence | --.- | --.- | N/A | -120 dB | -120 dB | 0 dB | [ ] |
| Pink noise -20 | -20.7 | -20.7 | 0.0 | -20.0 | -20.0 | 0.0 dB | [ ] |
| Pink noise -14 | -14.7 | -14.7 | 0.0 | -14.0 | -14.0 | 0.0 dB | [ ] |
| Sine 1k -3dB | -3.0 | -3.0 | 0.0 | -3.0 | -3.0 | 0.0 dB | [ ] |
| Sine 1k 0dB | 0.0 | 0.0 | 0.0 | 0.0 | 0.0 | 0.0 dB | [ ] |

---

## 6. Expected Results Summary

This section summarizes expected results for each major test category to aid in pass/fail determination.

### 6.1 Installation Verification

All plugin files should exist at the specified locations with file sizes within expected ranges. Any missing files or significantly different sizes indicate a build or installation problem that should be resolved before proceeding with functional testing.

### 6.2 Audio Processing Results

The BULLsEYE plugin should function as a transparent pass-through for audio signals. Any coloration or level change greater than 0.01 dB indicates a problem with the audio processing chain that requires investigation.

### 6.3 Loudness Measurement Results

| Input Signal | Expected LUFS-I | Expected Gate State |
|--------------|-----------------|---------------------|
| Silence | --.- | Gated (below threshold) |
| -70 LUFS constant | --.- | Gated (at absolute threshold) |
| -60 LUFS constant | --.- | Gated (below relative threshold for -14 target) |
| -20 dBFS pink noise | -20.7 LUFS | Not gated |
| -14 dBFS pink noise | -14.7 LUFS | Not gated |
| Program at target | Matches target ±0.5 | Not gated |

### 6.4 True Peak Results

| Input Signal | Expected True Peak | Notes |
|--------------|-------------------|-------|
| -6 dBFS sine | -6.0 dBTP | Simple peak, no interpolation needed |
| 0 dBFS sine | 0.0 dBTP | May show inter-sample overshoot |
| Clipping square wave | > 0 dBTP | 4x interpolation catches overshoots |
| High-frequency content | Variable | More likely to show inter-sample peaks |

### 6.5 Deviation Display Results

| Actual vs Target | Deviation | Display Color | Bar Position |
|------------------|-----------|---------------|--------------|
| Target ±1 LU | Balanced | Green | Center region |
| Target +2 to +5 LU | Hot | Red | Right of center |
| Target -2 to -5 LU | Quiet | Blue | Left of center |
| Target +6+ LU | Very Hot | Red | Far right |
| Target -6+ LU | Very Quiet | Blue | Far left |

---

## 7. Known Issues and Workarounds

This section documents known issues, their workarounds, and expected resolution timelines.

### 7.1 macOS-Specific Issues

**Issue**: Audio Unit validation warnings on first load

**Description**: macOS may display security warnings when loading unsigned Audio Units. The plugin is currently code-signed with ad-hoc signature.

**Workaround**: The first time loading BULLsEYE, macOS may require user approval in System Settings > Privacy & Security. After approving, the plugin loads normally.

**Resolution**: Future builds should use proper developer ID signing for Gatekeeper compatibility.

### 7.2 DAW-Specific Issues

**Issue**: Ableton Live 10 VST3 compatibility

**Description**: Older versions of Ableton Live may not fully support VST3 plugins.

**Workaround**: Use Audio Unit format on macOS for Ableton Live 10. Ableton Live 11+ supports VST3 fully.

**Resolution**: Test with target DAW version and use compatible plugin format.

**Issue**: REAPER AU plugin rescan

**Description**: REAPER may require manual plugin rescan after installation.

**Workaround**: Use Options > Preferences > Plug-ins > Re-scan to refresh the plugin list.

**Resolution**: Normal DAW behavior; not a plugin issue.

### 7.3 Performance Considerations

**Issue**: High CPU usage with multiple instances

**Description**: Running many instances of BULLsEYE on low-end systems may cause CPU spikes.

**Workaround**: Disable meters when not actively monitoring, or reduce DAW project sample buffer size.

**Resolution**: DSP core is optimized; consider UI optimization in future releases.

### 7.4 Measurement Edge Cases

**Issue**: Gate behavior with highly variable content

**Description**: Content with extremely variable loudness (such as classical music with wide dynamic range) may exhibit gate behavior that differs slightly from reference meters.

**Workaround**: Use absolute gate threshold mode for critical measurements with highly variable content.

**Resolution**: ITU-R BS.1770 specifies gate behavior; compare results against reference implementation.

### 7.5 Known Limitations

| Limitation | Description | Expected Resolution |
|------------|-------------|---------------------|
| No True Peak reset | True Peak holds maximum since session start | Manual reset in future version |
| No histogram display | Only current values shown | Historical data in future version |
| No EBU R128 mode | Only ITU-R BS.1770 mode | Additional modes in future version |
| No loudness range | LRA measurement not available | Additional metrics in future version |

---

## 8. Testing Workflow Summary

### 8.1 Pre-Testing Checklist

Before beginning DAW testing, verify the following:

- [ ] Plugin artifacts verified at correct installation paths
- [ ] File sizes within expected ranges (8-10 MB for release)
- [ ] Test audio files generated and verified
- [ ] Reference meter available for comparison (Youlean, iZotope, etc.)
- [ ] DAW projects created for each testing section
- [ ] Test result recording template prepared

### 8.2 Testing Order

Follow this recommended testing order for efficiency:

1. **Installation Verification**: Confirm plugin files exist and are sized correctly
2. **Basic Functionality**: Load plugin, verify UI displays, confirm no errors
3. **Audio Passthrough**: Verify transparent signal flow
4. **Meter Response**: Test with simple signals (silence, sine, noise)
5. **Content Type Switching**: Verify parameter changes work correctly
6. **Preset Management**: Test save/load functionality
7. **Project Persistence**: Test project save/reload
8. **Advanced Testing**: JSFX comparison, edge cases

### 8.3 Documentation Requirements

Record the following for each test section:

- DAW version and operating system version
- Sample rate and buffer size settings
- Test signal file names and generation parameters
- Actual measured values for each test
- Any deviations from expected results
- Screenshots for visual tests (meters, UI)
- Time and date of testing for reproducibility

---

## 9. Test Result Recording Template

Use this template to record comprehensive test results for documentation and troubleshooting.

### 9.1 Test Session Header

| Field | Value |
|-------|-------|
| Date | YYYY-MM-DD |
| Tester | Name |
| DAW | REAPER / Ableton Live / Other |
| DAW Version | x.x.x |
| Plugin Version | 1.2.1 |
| Plugin Format | AU / VST3 |
| OS Version | macOS xx.x.x |
| Test Audio Path | /path/to/test/signals/ |

### 9.2 Detailed Test Results

**Section**: Audio Passthrough

| Test ID | Signal | Input Level | Expected Output | Actual Output | Difference | Pass/Fail |
|---------|--------|-------------|-----------------|---------------|------------|-----------|
| XX-01 | Sine 1k | -6 dBFS | -6.00 dBFS | | | [ ] |
| XX-02 | Sine 1k | 0 dBFS | 0.00 dBFS | | | [ ] |

**Section**: LUFS-I Measurement

| Test ID | Signal | Expected LUFS | Actual LUFS | Difference | Pass/Fail |
|---------|--------|---------------|-------------|------------|-----------|
| XX-01 | Pink noise -20 | -20.7 LUFS | | | [ ] |
| XX-02 | Pink noise -14 | -14.7 LUFS | | | [ ] |

### 9.3 Issue Log

| Issue ID | Test ID | Description | Severity | Workaround | Status |
|----------|---------|-------------|----------|------------|--------|
| | | | | | |

---

## 10. References and Resources

### 10.1 Standards Documents

- **ITU-R BS.1770-4**: Algorithms to measure audio programme loudness and true-peak audio level
- **ITU-R BS.1771-1**: Requirements for loudness and true-peak indicating meters
- **EBU R128**: Loudness normalisation and maximum level of audio signals

### 10.2 Related Files

| File | Location | Purpose |
|------|----------|---------|
| BULLsEYE-Mini-Combo.jsfx | jsfx/ | Original JSFX implementation |
| BULLsEYEProcessor.h | Source/DSP/ | JUCE DSP implementation |
| DSPSSOT.h | Source/SSOT/ | DSP constants and parameters |
| REFERENCE.md | project root/ | Plugin identity and configuration |

### 10.3 Test Audio Generation Tools

| Tool | Purpose | Installation |
|------|---------|--------------|
| SoX | Audio file generation | `brew install sox` |
| ffmpeg | Audio format conversion | `brew install ffmpeg` |
| Audacity | Audio editing and analysis | https://www.audacityteam.org/ |

---

**Document Version**: 1.0.0  
**Last Updated**: 2026-02-06  
**Status**: Initial Release
