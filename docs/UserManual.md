# BULLsEYE User Manual

**LUFS-I Gated Loudness Meter with True Peak Detection**

**Version:** 1.2.1  
**Plugin Format:** Audio Unit (AU), VST3  
**Documentation Version:** 1.0.0

---

## Table of Contents

1. [Introduction](#1-introduction)
2. [Installation](#2-installation)
3. [Interface Overview](#3-interface-overview)
4. [Content Type Selection](#4-content-type-selection)
5. [Interpreting Measurements](#5-interpreting-measurements)
6. [Tips and Best Practices](#6-tips-and-best-practices)
7. [Troubleshooting Guide](#7-troubleshooting-guide)
8. [Technical Specifications](#8-technical-specifications)

---

## 1. Introduction

### What is BULLsEYE?

BULLsEYE is a professional-grade loudness metering plugin designed for mixing engineers, mastering engineers, and content creators who need accurate, industry-standard loudness measurements. The plugin implements ITU-R BS.1770 compliant loudness processing with an emphasis on visual clarity and real-time feedback.

The name BULLsEYE reflects the plugin's primary purpose: helping you hit your target loudness with precision, much like aiming for the center of a target. Whether you are preparing music for streaming platforms, creating content for broadcast, or mastering for CD distribution, BULLsEYE provides the measurements you need to ensure your audio meets the appropriate loudness standards.

### Key Features

BULLsEYE combines several powerful measurement technologies into a streamlined interface. The integrated LUFS-I (Loudness Units relative to Full Scale Integrated) measurement follows the international standard for loudness evaluation, providing a single number that represents the overall loudness of your program material. Unlike peak-based measurements that only capture momentary peaks, LUFS-I accounts for human hearing perception, giving you a more accurate representation of how loud your audio will actually sound to listeners.

The True Peak detection system uses 4x oversampling with Hermite interpolation to identify inter-sample peaks that standard peak meters miss. This is critical for digital audio delivery, as inter-sample peaks can cause clipping when the audio is subsequently processed or converted by different systems. By detecting these hidden peaks, BULLsEYE helps you avoid unexpected distortion in your final product.

The gated integration system implements the dual-threshold gating approach specified in EBU R128 and ITU-R BS.1770. This gating prevents silence and low-level background noise from artificially lowering your loudness measurement, ensuring that the reported LUFS value reflects the actual program content rather than the noise floor of your recording environment.

### Content Type Modes

BULLsEYE includes three preset content type modes that automatically configure appropriate target loudness values. These modes correspond to common industry standards for different types of audio content. The Music Non-drums mode targets -11 LUFS, which aligns with many streaming platform recommendations for music content. Music Drums mode targets -8 LUFS, suitable for music with significant dynamic range including percussion elements. Cinema/Trailer mode targets -14 LUFS, matching common broadcast and cinema loudness standards.

Each mode affects only the target reference used for deviation display and the status indicator. The underlying loudness measurement algorithm remains consistent across all modes, ensuring accurate and comparable measurements regardless of the selected content type.

### Applications

BULLsEYE serves multiple purposes in audio production workflows. During mixing, use the real-time LED strip meter and deviation display to maintain consistent loudness levels as you adjust tracks and balance the mix. The immediate visual feedback helps you make informed fader decisions without constantly referencing a separate meter.

During mastering, the integrated LUFS-I measurement provides the key metric for achieving target loudness. Whether you are targeting -14 LUFS for streaming platforms, -23 LUFS for broadcast, or -24 LUFS for music streaming services, BULLsEYE gives you the precise measurement needed to deliver compliant audio.

For broadcast and post-production, the True Peak measurement ensures your audio will not clip when transmitted through various broadcast chains, which often apply additional processing that could cause inter-sample peaks to become audible distortion.

---

## 2. Installation

### System Requirements

BULLsEYE requires a compatible 64-bit host application (DAW) running on macOS. The plugin has been tested with macOS versions from 10.15 (Catalina) through the latest releases. Sufficient disk space is required for the plugin components, with the Audio Unit version requiring approximately 8.7 MB and the VST3 version requiring approximately 9 MB.

A minimum of 4 GB RAM is recommended for optimal performance, though the plugin itself has minimal memory requirements. The plugin supports all standard audio sample rates from 44.1 kHz through 192 kHz and beyond, with automatic sample rate handling built into the processing core.

### Installing Audio Unit Format (macOS)

The Audio Unit format is the recommended choice for users of Logic Pro, GarageBand, and other macOS-native audio applications. To install the AU version, first ensure your DAW is closed. Locate the built plugin component file, which by default installs to your user's Audio Plug-Ins folder.

Navigate to ~/Library/Audio/Plug-Ins/Components/ in Finder. If this folder does not exist, create it. Copy the BULLsEYE.component file into this folder. On macOS 11 (Big Sur) and later, you may need to right-click the component and select "Open" the first time to grant permission through Gatekeeper. Alternatively, you can approve the plugin through System Settings > Privacy & Security.

After installation, launch your DAW and perform a plugin rescan. The method for rescanning varies by application: Logic Pro and GarageBand typically scan automatically on launch, while other applications may have a specific rescan function in their preferences or options menus.

### Installing VST3 Format

The VST3 format provides compatibility with a wider range of DAWs including REAPER, Cubase, Nuendo, and Bitwig Studio. The VST3 component installs to ~/Library/Audio/Plug-Ins/VST3/ by default. Copy the BULLsEYE.vst3 folder into this location.

Some DAWs have specific VST3 search paths configured in their preferences. If your DAW does not automatically detect the plugin, check the VST3 paths setting in your DAW's preferences and add the ~/Library/Audio/Plug-Ins/VST3/ path if necessary.

### Verifying Installation

After installing and rescanning, verify that BULLsEYE loads correctly by inserting it on an audio track. The plugin interface should appear when you open the plugin window. You should see the mode selector dropdown, the LED strip meter, and the status display showing current measurements.

If the plugin does not appear in your DAW's plugin browser, first confirm that the plugin file was installed to the correct location. Then check that you installed the correct format for your DAW (AU for Logic Pro/GarageBand, VST3 for most other DAWs). Finally, try restarting both your DAW and your computer to ensure all cached plugin information is refreshed.

---

## 3. Interface Overview

### Main Window Layout

The BULLsEYE interface presents all essential information in a clean, organized layout. The top section contains the mode selector for choosing your content type. The center section features the LED strip meter, which provides real-time visualization of your audio levels relative to the target loudness. The bottom section displays the status panel with numerical readouts for integrated loudness, True Peak, and deviation from target.

The interface uses a modern dark theme optimized for extended use in dimly lit studio environments. Color coding throughout the interface provides quick visual feedback on your loudness status, with green indicating balanced levels, red indicating hot (over-target) levels, and blue indicating quiet (under-target) levels.

### Mode Selector Component

The mode selector dropdown, located at the top of the plugin interface, controls the content type setting. Click the dropdown to reveal three options: Music Non-drums, Music Drums, and Cinema/Trailer. The currently selected mode displays in the dropdown button.

Selecting a different content type updates the target LUFS value immediately. This changes the reference point used for the deviation display and status indicator, allowing you to quickly check compliance with different loudness standards without manually calculating target offsets.

The mode selector maintains its state when you save and load DAW projects, ensuring your preferred content type remains selected between sessions. Some DAWs may display the content type parameter in their automation lanes, allowing you to automate mode changes during your workflow.

### LED Strip Meter

The LED strip meter provides a horizontal visualization of your audio loudness in real-time. Twenty individual LED segments span the meter, each representing a portion of the total display range. The meter's display range extends from -60 LUFS (floor) to 0 LUFS (ceiling), with a calibrated target marker indicating your current target loudness level.

Color coding on the LED strip follows standard metering conventions. The bottom segments display dark green, transitioning through light green, yellow, and orange, with the top segments in red. This color progression mirrors traditional analog meter ballistics and provides intuitive visual feedback on your levels.

A cyan target marker indicates where your current target LUFS value falls on the meter scale. As you adjust the mode selector or make changes to your audio, the target marker may shift, and the LEDs will respond accordingly to show your current loudness position relative to the target.

The LED meter updates smoothly at 30 frames per second, providing readable visualization without the distracting flicker that can occur with higher refresh rates. The smoothing helps you perceive the general loudness trend of your program material rather than momentary fluctuations.

### Status Display Component

The status display panel presents three key measurements in numerical format. The primary display shows the integrated LUFS-I value, which represents the overall loudness of your program material. Below this, the True Peak display shows the highest detected peak value in dBTP (decibels True Peak).

The deviation bar provides a horizontal visualization of how far your current loudness deviates from the target. When the indicator sits at center position, your audio matches the target loudness. Movement to the left indicates quieter-than-target audio, while movement to the right indicates louder-than-target audio.

Color-coded text and indicators in the status display provide quick feedback on your loudness compliance. The status text may display "Balanced" when within 1 LU of target, "Hot" when significantly above target, or "Quiet" when significantly below target. These indicators help you quickly assess whether your audio meets the selected content type's requirements.

---

## 4. Content Type Selection

### Understanding Content Types

Content type selection in BULLsEYE corresponds to different target loudness standards used across various audio distribution channels. Selecting the appropriate content type ensures that BULLsEYE's deviation display and status indicators accurately reflect compliance with your target delivery specification.

The content type affects only the target reference, not the underlying measurement algorithm. LUFS-I measurement, True Peak detection, and gated integration operate identically regardless of the selected content type. This ensures that your measurements remain accurate and comparable across different modes.

### Music Non-drums (-11 LUFS)

The Music Non-drums mode targets -11 LUFS, which aligns with Spotify's recommendations for music content. Many streaming platforms apply normalization that brings music to approximately -14 LUFS, but targeting -11 LUFS provides headroom while maintaining competitive loudness. This mode works well for acoustic music, vocal-focused recordings, jazz, classical, and other genres where maintaining dynamic range is important.

When using this mode, the deviation display indicates how your audio relates to -11 LUFS. If your integrated loudness measures -12 LUFS, the deviation shows -1 LU, indicating your audio is 1 LU below the target and will be normalized upward by compatible streaming platforms. If your integrated loudness measures -9 LUFS, the deviation shows +2 LU, indicating your audio is 2 LU above the target and may be normalized downward.

### Music Drums (-8 LUFS)

Music Drums mode targets -8 LUFS, providing a reference point for music with significant percussion and rhythmic elements. This target represents a louder presentation level common in modern pop, rock, electronic, and hip-hop productions. The higher target reflects the competitive nature of these genres where louder masters often perceived as more impactful.

This mode serves as BULLsEYE's default selection, indicated by the mode selector's initial position. The -8 LUFS target also serves as a useful intermediate reference during mixing, as many engineers find this level provides good visibility into their mix decisions while maintaining headroom for mastering adjustments.

When targeting -8 LUFS, remember that streaming normalization will likely reduce the loudness of your master to -14 LUFS or lower. The deviation display shows the relationship to your target, not the relationship to normalized playback levels. Keep your delivery specifications in mind when interpreting deviation readings in this mode.

### Cinema/Trailer (-14 LUFS)

Cinema/Trailer mode targets -14 LUFS, matching the broadcast loudness standard used by many television networks and streaming services for dramatic content. This mode is appropriate for film scores, dialogue-heavy productions, trailers, and any content intended for television or streaming distribution where broadcast compliance is required.

Netflix, Amazon Prime Video, and many regional broadcast networks specify -24 LUFS integrated loudness with -1 dBTP maximum True Peak. However, -14 LUFS serves as a useful reference point during mixing, as it allows you to work at a comfortable listening level while maintaining awareness of your relationship to the broadcast standard.

For content specifically targeting Netflix delivery, use -24 LUFS as your reference by selecting Music Non-drums mode (which targets -11 LUFS) and allowing 13 LU of deviation in your calculations. Similarly, for EBU R128 broadcast compliance at -23 LUFS, Music Non-drums mode provides a -11 LUFS target that you would adjust accordingly.

### Selecting the Right Mode

Choosing the correct content type depends primarily on your intended delivery format. For music destined for streaming platforms, Music Drums mode provides a familiar reference point during mixing, while Music Non-drums mode offers a more conservative target if you prefer to maintain maximum dynamic range.

For broadcast and television content, Cinema/Trailer mode provides the appropriate -14 LUFS reference. During post-production mixing, this mode helps you maintain awareness of your loudness relative to broadcast standards without requiring mental calculation of target offsets.

If your specific delivery requirement does not match any preset mode exactly, select the closest preset and use the deviation display as a reference point. The deviation value tells you exactly how many LU your audio differs from the target, allowing you to calculate the necessary adjustment for any target value.

---

## 5. Interpreting Measurements

### Understanding LUFS

LUFS (Loudness Units relative to Full Scale) represents a perceptual loudness measurement based on how human hearing responds to different frequencies. Unlike simple amplitude averaging, LUFS measurement applies frequency weighting (K-weighting) that emphasizes frequencies to which human ears are most sensitive while reducing emphasis on very low frequencies that contribute less to perceived loudness.

The K-weighting filter implemented in BULLsEYE follows ITU-R BS.1770 specifications exactly. A high-pass filter at 60 Hz with Q=0.5 removes low-frequency content below the nominal hearing threshold used in the standard. A high-shelf filter at 4 kHz with Q=0.707 and +4 dB gain applies the standardized high-frequency weighting. Together, these filters create a frequency response that correlates well with perceived loudness across diverse program material.

When you see an integrated loudness measurement of -12 LUFS, this indicates that your program material has a perceptual loudness equivalent to a steady -12 dBFS sine wave after K-weighting application. A measurement of -8 LUFS indicates louder perceptual loudness, while -16 LUFS indicates quieter perceptual loudness.

### Understanding True Peak

True Peak measurement differs from sample peak measurement by detecting inter-sample peaks that occur between digital audio samples. Standard peak meters only examine the actual sample values, missing peaks that exist in the continuous analog signal reconstructed from those samples. When this reconstructed signal passes through subsequent processing or digital-to-analog conversion, inter-sample peaks can manifest as actual waveform peaks that exceed 0 dBFS.

BULLsEYE's True Peak detector uses 4x oversampling with Hermite interpolation to examine the waveform between samples. By buffering four consecutive samples and interpolating three additional points between each sample pair, the detector identifies peaks that would otherwise remain hidden. This approach follows recommendations in ITU-R BS.1770 and provides reliable detection of inter-sample peaks.

A True Peak reading of -3 dBTP indicates that the highest detected peak (including inter-sample peaks) reached -3 dB relative to full scale. Readings closer to 0 dBTP indicate higher peak levels, while readings further below 0 dBTP (such as -6 dBTP or -12 dBTP) indicate lower peak levels.

Most delivery specifications require True Peak not exceeding -1 dBTP or -2 dBTP. Some specifications (particularly broadcast) require -3 dBTP or lower. Monitor your True Peak reading throughout your workflow to ensure your audio remains compliant with your target specification.

### Understanding Deviation

The deviation measurement shows how far your current integrated loudness deviates from your target loudness in Loudness Units. A deviation of 0 LU indicates perfect alignment with your target. Positive deviation values (shown when the deviation indicator moves right of center) indicate your audio is louder than target. Negative deviation values (shown when the indicator moves left of center) indicate your audio is quieter than target.

Each LU represents a doubling of perceived loudness according to the ITU-R BS.1770 standard. A +1 LU increase sounds approximately twice as loud as the reference level, while a +2 LU increase sounds approximately four times as loud. This logarithmic relationship means that small changes in LUFS values correspond to noticeable changes in perceived loudness.

When working toward a target, aim to reduce deviation to within 1 LU for most delivery specifications. Some platforms accept deviations up to 1 LU above or below the target, while stricter specifications may require tighter tolerance. The deviation bar's green "balanced zone" indicates the range within 1 LU of target.

### Gated Integration Behavior

BULLsEYE implements gated integration according to EBU R128 and ITU-R BS.1770 specifications. The gate prevents silence and low-level background noise from lowering the integrated loudness measurement. Two thresholds work together: an absolute gate at -70 LUFS ignores any content below this level, while a relative gate at L-10 ignores content 10 LU below the current integrated measurement.

When you first load audio into BULLsEYE, the gate operates using only the absolute threshold since no integrated measurement exists yet. As measurement accumulates, the relative gate activates, further filtering out content that falls significantly below the program loudness level.

During passages of silence or very quiet audio (such as a held fermata in a classical piece), the display may show "--.-" to indicate that the signal is below the gate threshold and no valid measurement is available for that portion. This is normal behavior and ensures that silence does not artificially lower your integrated measurement.

### Measurement Timing

Integrated loudness measurements accumulate over the duration of your program. BULLsEYE processes audio in real-time, updating measurements as audio plays through the plugin. The integrated value represents the measurement from the beginning of the audio (or since the last reset).

When you first insert BULLsEYE on a track, the integrated measurement displays "--.-" until sufficient audio has accumulated to calculate a valid value. Once the first 400ms block passes the gate threshold, a measurement appears. This value continues updating as more audio processes through the plugin.

To restart measurement from the beginning of your program, bypass and re-insert BULLsEYE, or use your DAW's transport controls to stop and restart playback from the beginning of the region. Note that BULLsEYE does not include a dedicated reset button; measurement accumulation is continuous during playback.

---

## 6. Tips and Best Practices

### Workflow Integration

Insert BULLsEYE as an insert effect on your master bus or group buses during mixing. The plugin's minimal CPU footprint allows it to remain active throughout your session without impacting performance. Monitoring the LED strip meter and deviation display as you make mixing decisions helps you maintain awareness of your loudness trajectory.

When mixing toward a specific target, periodically check the integrated LUFS value during playback. If the deviation indicator shows significant movement to one side, consider adjusting fader levels to bring the mix back toward target. Small, incremental adjustments during mixing are more effective than large changes made after the mix is complete.

For mastering workflows, use BULLsEYE alongside your DAW's built-in metering and any other metering plugins you prefer. Cross-reference measurements between plugins to confirm accuracy. The agreement between BULLsEYE and other ITU-R BS.1770 compliant meters should be within 0.2 LU for typical program material.

### Inter-Sample Peak Protection

Monitor True Peak throughout your mixing and mastering process, not just at the final stage. True Peak accumulation can occur gradually as you add processing and make level changes. Catching True Peak violations early prevents surprises at the final stage of your workflow.

If True Peak exceeds your target specification, several approaches can help. First, check for inter-sample peaks by comparing BULLsEYE's True Peak reading to your DAW's sample peak meter. If True Peak exceeds sample peak, inter-sample peaks are present. Reduce the overall level of the affected track or bus. Alternatively, apply a transparent limiter with appropriate look-ahead time set to True Peak mode.

Avoid the temptation to push levels excessively high in search of competitive loudness. Modern streaming platforms normalize audio playback, meaning that louder masters do not necessarily sound louder to listeners. A well-balanced master at -14 LUFS may sound better than an over-compressed master at -8 LUFS after platform normalization is applied.

### Content Type Selection Strategy

Select your content type early in the workflow and maintain consistency throughout. Changing content types mid-mix can lead to confusion about whether your decisions are appropriate for your target delivery format. Make content type selection one of your first workflow decisions before significant mixing begins.

If you deliver to multiple platforms with different specifications, consider creating separate mixes or stems rather than trying to create one master that meets all specifications. The different target loudness values and True Peak requirements typically require separate masters for optimal results.

For content destined for Spotify, the Music Drums mode (-8 LUFS) provides a useful reference during mixing. Spotify normalizes music to approximately -14 LUFS, so a mix targeting -8 LUFS will play back at roughly -14 LUFS when Spotify applies normalization. The deviation display shows the relationship between your current mix and this target.

### Working with Deviation

Use the deviation bar as a quick visual reference rather than obsessively chasing perfect zero deviation during mixing. Perfect alignment with the target is neither expected nor necessary during the mixing phase. The goal is awareness of your trajectory and prevention of significant deviations that would require substantial rework later.

Small deviations of 1-2 LU are normal and acceptable during mixing. If your deviation exceeds 3-4 LU, consider whether level adjustments or dynamic processing changes would bring the mix closer to target without compromising musical intent.

When you observe consistent deviation in one direction across multiple projects, this may indicate a calibration issue with your monitoring system or a workflow tendency that requires adjustment. Compare BULLsEYE's readings with measurements from other trusted meters to confirm accuracy.

### Dealing with Dynamic Range

Modern loudness normalization has diminished the importance of maximizing loudness for competitive purposes. Content with greater dynamic range often translates better across playback systems and sounds more engaging to listeners. Use BULLsEYE to verify compliance with delivery specifications while preserving the dynamic range that serves your artistic vision.

The gated integration measurement ensures that quiet passages do not artificially lower your integrated loudness. This means you can include intentional quiet passages without worrying that they will require you to make the loud sections excessively loud to achieve your target.

If your integrated loudness measures below target due to extensive dynamic range, consider whether subtle compression or level riding in quiet passages would allow slight increases in overall level without audibly affecting the musical content. Any such adjustments should serve the music, not merely achieve a number on a meter.

---

## 7. Troubleshooting Guide

### Plugin Does Not Appear in DAW

If BULLsEYE does not appear in your DAW's plugin browser after installation, first verify that you installed the correct format for your DAW. Audio Unit (AU) plugins work with Logic Pro, GarageBand, and other macOS-native applications. VST3 plugins work with REAPER, Cubase, Nuendo, Bitwig, and most Windows-compatible DAWs running on macOS through compatibility layers.

Confirm that the plugin file exists in the correct installation folder. For Audio Units, check ~/Library/Audio/Plug-Ins/Components/BULLsEYE.component. For VST3, check ~/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3. If the file is missing, reinstall the plugin using the build process.

Some DAWs cache plugin information and require a rescan to detect newly installed plugins. Check your DAW's preferences for a plugin rescan or refresh function. In some cases, restarting your DAW completely will trigger a new scan on launch.

On macOS, Gatekeeper may block plugins that have not been notarized. If you receive a security warning when attempting to open BULLsEYE, try right-clicking the plugin file and selecting "Open" to grant permission. Alternatively, approve the plugin through System Settings > Privacy & Security.

### Measurements Show "--.-"

The "--.-" display indicates that the current audio signal is below the gate threshold and no valid measurement is available. This typically occurs when the track is silent, contains only very low-level noise, or has not yet accumulated sufficient audio for measurement.

First, verify that audio is actually playing through the plugin. Check that the track is not muted, that playback is active, and that the plugin is inserted correctly in the signal chain. Temporarily insert a different meter or analyzer to confirm that audio is reaching the plugin.

If audio is playing but "--.-" persists, the signal may genuinely be below the -70 LUFS absolute gate threshold. This can occur with very quiet recordings or during extended silent passages. As soon as the signal level rises above the gate threshold, a valid measurement will appear.

If "--.-" appears despite audible audio at normal levels, there may be a routing issue. Check that the plugin is inserted on the correct track or bus and that the audio is being sent to that output. Verify that no other plugins before BULLsEYE are muting or routing the signal elsewhere.

### Deviation Bar Stays at Edge

If the deviation indicator remains at one extreme of the bar (fully left or fully right) even with normal audio levels, the deviation may exceed the display range. The deviation bar displays a range of approximately +/-5 LU, with extreme values clamped at the edges.

Check the numerical deviation display if available. If the value shows greater than +5 LU or less than -5 LU, your audio deviates significantly from target. Consider whether this deviation is appropriate for your workflow stage and target delivery format.

If the numerical display shows reasonable deviation values but the bar visual does not respond, there may be a UI update issue. Try closing and reopening the plugin window, or bypass and re-insert the plugin to refresh the UI state.

### True Peak Shows 0 dBTP

A True Peak reading of 0 dBTP indicates clipping either in your audio or in the processing chain before BULLsEYE. The plugin applies output limiting to prevent unstable signals from propagating, but True Peak at 0 dBTP reveals that the input signal exceeds full scale.

Check your DAW's sample peak meter or other meters to confirm whether clipping is occurring. If your DAW shows peaks at or below 0 dBFS but BULLsEYE shows 0 dBTP, inter-sample peaks are being detected. Reduce the overall level of the track or bus, or apply limiting to bring True Peak below 0 dBTP.

If you consistently see 0 dBTP with various audio sources, check your audio interface and software gain staging. Ensure that no mixing console or routing settings are pushing levels beyond 0 dBFS before the signal reaches BULLsEYE.

### Measurements Seem Inaccurate

If BULLsEYE's measurements appear to differ significantly from other LUFS meters, first verify that both meters use the same measurement standard. BULLsEYE implements ITU-R BS.1770 with K-weighting and gated integration. Meters using different standards (such as Leq, RMS, or unweighted measurements) will produce different values.

Cross-reference with another ITU-R BS.1770 compliant meter such as Youlean Loudness Meter, Dorrough Loudness Meters, or your DAW's built-in loudness metering if available. Agreement within 0.2 LU for typical program material indicates both meters are functioning correctly.

Check that you are comparing the correct measurement types. Integrated LUFS (LUFS-I) differs from momentary loudness (LUFS-M) and short-term loudness (LUFS-S). BULLsEYE displays integrated loudness. Make sure comparison meters are displaying the same measurement type.

If measurements are consistently off by a consistent amount, there may be a calibration discrepancy. Verify that your audio interface and DAW are configured for standard reference levels (typically -18 dBFS or -20 dBFS = 0 VU).

### High CPU Usage

BULLsEYE is designed for minimal CPU usage, but high instance counts or very high sample rates may impact performance. Monitor your DAW's CPU meter with BULLsEYE active compared to inactive to confirm whether the plugin is the source of high CPU usage.

If CPU usage seems excessive, try reducing the plugin instance count by using BULLsEYE only on master or group buses rather than on every individual track. One instance on the master bus provides the same integrated loudness measurement as multiple instances throughout the signal chain.

Ensure that your DAW is configured for appropriate buffer sizes. Very small buffer sizes (64 samples or 128 samples) increase CPU overhead for all plugins. Increasing buffer size to 512 samples or higher reduces CPU load while maintaining acceptable latency for monitoring.

### Audio Distortion After Insertion

If audio becomes distorted after inserting BULLsEYE, check first whether the distortion was present before insertion. The plugin applies minimal processing and passes audio through unchanged except for True Peak limiting to prevent unstable signals from propagating.

Verify that your DAW's audio engine is not experiencingxruns or buffer underruns, which can cause distortion that appears coincidentally with plugin insertion. Stop and restart playback, or restart your DAW to clear potential engine state issues.

If distortion persists, check the True Peak display. Values at or above 0 dBTP indicate inter-sample clipping. Reduce the output level of tracks feeding BULLsEYE, or insert a limiter before BULLsEYE to control peaks.

---

## 8. Technical Specifications

### Loudness Measurement

BULLsEYE implements integrated loudness measurement according to ITU-R BS.1770-4 and EBU R128 specifications. The measurement applies K-weighting filters to all audio channels before energy accumulation and integration.

| Parameter | Value |
|-----------|-------|
| Measurement Type | LUFS-I (Integrated) |
| Standard Compliance | ITU-R BS.1770-4, EBU R128 |
| K-Weighting High-Pass | 60 Hz, Q=0.5 |
| K-Weighting High-Shelf | 4 kHz, Q=0.707, +4 dB |
| Absolute Gate | -70 LUFS |
| Relative Gate | L-10 LU |
| Integration Block | 400 ms (sample rate dependent) |
| Display Range | -70 LUFS to 0 LUFS |

### True Peak Detection

True Peak detection uses 4x oversampling with Hermite interpolation to identify inter-sample peaks.

| Parameter | Value |
|-----------|-------|
| Oversampling | 4x |
| Interpolation Method | Hermite |
| Detection Points | Original + 3 interpolated per sample |
| Display Range | -60 dBTP to 0 dBTP |
| Output Limiting | Clamps to +/- 1.0 to prevent instability |

### Supported Formats

| Format | Platform | File Extension | Installation Location |
|--------|----------|----------------|----------------------|
| Audio Unit | macOS | .component | ~/Library/Audio/Plug-Ins/Components/ |
| VST3 | macOS, Windows, Linux | .vst3 | ~/Library/Audio/Plug-Ins/VST3/ (macOS) |

### Audio Compatibility

| Parameter | Range |
|-----------|-------|
| Sample Rates | 44.1 kHz - 192 kHz+ |
| Bit Depth | 32-bit float, 64-bit float |
| Channels | Stereo (2-channel), Dual Mono (2-channel) |
| Latency | 0 samples (passthrough) |
| DSP Processing | Sample-by-sample |

### Build Information

| Parameter | Value |
|-----------|-------|
| Plugin Version | 1.2.1 |
| Plugin Code | BULL |
| JUCE Version | 8 |
| C++ Standard | C++17 |
| Architecture | Universal (Intel/Apple Silicon) |

---

## Screenshot Reference

The following screenshots should be captured and included in this manual for visual reference:

| Reference | Description | Recommended Capture Method |
|-----------|-------------|---------------------------|
| `screenshot-main-interface.png` | Full BULLsEYE interface with all elements visible | Insert on active audio track in DAW, capture with audio playing |
| `screenshot-mode-selector.png` | Mode selector dropdown expanded showing all three options | Open plugin window, click dropdown arrow |
| `screenshot-balanced-display.png` | Interface showing balanced state (green indicators) | Process audio at target loudness, capture status display |
| `screenshot-hot-display.png` | Interface showing hot state (red indicators) | Process audio above target, capture status display |
| `screenshot-quiet-display.png` | Interface showing quiet state (blue indicators) | Process audio below target, capture status display |
| `screenshot-led-meter-detail.png` | LED strip meter close-up with target marker visible | Maximize or zoom plugin window |
| `screenshot-deviation-bar.png` | Deviation bar at various positions | Process audio at different loudness levels |
| `screenshot-au-in-logic.png` | BULLsEYE loaded in Logic Pro | Insert on track, capture plugin window |
| `screenshot-vst3-in-reaper.png` | BULLsEYE loaded in REAPER | Insert on track, capture plugin window |

---

**BULLsEYE User Manual**  
*Documentation Version 1.0.0*  
*Plugin Version 1.2.1*