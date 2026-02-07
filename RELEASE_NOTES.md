# BULLsEYE v1.2.1 Release Notes

## Overview

BULLsEYE is a professional-grade LUFS-I gated loudness meter with True Peak detection, designed for mixing, mastering, and broadcast preparation. This is the initial release following migration from JSFX to JUCE/C++.

## Plugin Description

BULLsEYE measures loudness according to ITU-R BS.1770-4 standard with:
- **LUFS-I Integration**: Perceptual loudness measurement with gated integration
- **True Peak Detection**: 4x oversampling with Hermite interpolation
- **Content Type Modes**: Optimized targets for Music and Cinema
- **Visual Feedback**: LED strip meter with color-coded zones

## System Requirements

| Component | Requirement |
|-----------|-------------|
| Operating System | macOS 10.15 (Catalina) or higher |
| RAM | 4 GB minimum |
| Disk Space | 50 MB for plugin installation |
| DAW | REAPER, Ableton Live, Logic Pro, Pro Tools, Cubase |

### Format Support

| Format | macOS | Windows | Linux |
|--------|-------|---------|-------|
| Audio Unit (AU) | Yes | No | No |
| VST3 | Yes | Yes | Yes |

## Installation Instructions

### macOS Audio Unit (AU)

1. Build the plugin:
   ```bash
   ./build.sh
   ```

2. The AU component will be installed to:
   ```
   ~/Library/Audio/Plug-Ins/Components/BULLsEYE.component/
   ```

3. **First Launch**: macOS may block the plugin due to Gatekeeper. To approve:
   - Open **System Settings > Privacy & Security**
   - Scroll to **Security** section
   - Click **Allow Anyway** next to the blocked developer message
   - Restart your DAW and re-scan for plugins

4. Rescan plugins in your DAW:
   - REAPER: Options > Add or Remove ReaPlugs > Re-scan
   - Ableton Live: Preferences > Plug-ins > Rescan
   - Logic Pro: Preferences > Plug-ins > Manage Plug-ins > Reset and Rescan

### VST3 Installation

1. Build the plugin:
   ```bash
   ./build.sh
   ```

2. The VST3 component will be installed to:
   ```
   ~/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3/
   ```

3. Some DAWs may require manual scanning of the VST3 folder.

## Code Signing Status

### Current Signature Status

BULLsEYE uses ad-hoc code signing for development builds. This provides:

- **Local Testing**: Full functionality for development and testing
- **DAW Compatibility**: Plugin loads and operates correctly
- **Gatekeeper**: May require manual approval on first launch (see below)

### Signature Types

| Signature Type | Status | Use Case |
|----------------|--------|----------|
| Ad-hoc (Development) | Active | Local development, testing, debugging |
| Developer ID (Distribution) | Required | Sharing with users, plugin stores, downloads |

### Developer ID Signing Requirements

For distribution to users, the plugin requires proper Developer ID signing:

**Required for:**
- Distributing outside your development machine
- Hosting download links on websites
- Submitting to plugin marketplaces
- Professional distribution

**How to Obtain:**
1. Enroll in Apple Developer Program ($99/year)
2. Create Developer ID certificate in Developer Portal
3. Sign plugin using provided build scripts
4. Submit for Apple notarization (macOS 10.15+)
5. Staple notarization ticket to plugin

**See Also:**
- `docs/CODE_SIGNING.md`: Complete signing guide
- `NOTARIZATION.md`: Notarization requirements
- `build/sign_plugin.sh`: Automated signing script

### Gatekeeper Considerations

With ad-hoc signing, users may encounter:

1. **First Launch Warning**: "Apple could not check for malicious software"
2. **Resolution**: Users approve in System Settings > Privacy & Security
3. **Subsequent Launches**: Plugin runs normally after approval

### For Plugin Distribution

To distribute BULLsEYE to users:

1. **Code Sign**: Use Developer ID certificate
   ```bash
   ./build/sign_plugin.sh "Developer ID Application: Your Name"
   ```

2. **Notarize**: Submit to Apple for notarization
   ```bash
   # See NOTARIZATION.md for complete process
   xcrun altool --notarize-app --file BULLsEYE.component ...
   ```

3. **Staple**: Embed notarization ticket
   ```bash
   xcrun stapler staple BULLsEYE.component
   ```

4. **Distribute**: Provide signed and notarized plugin

## Key Features

### LUFS-I Gated Loudness Measurement

- **ITU-R BS.1770-4 Compliant**: Industry-standard loudness measurement
- **Absolute Gate**: -70 LUFS threshold
- **Relative Gate**: L-10 (current loudness minus 10 LU)
- **K-Weighting**: 60Hz high-pass and 4kHz high-shelf filters

### True Peak Detection

- **4x Oversampling**: Detects inter-sample peaks
- **Hermite Interpolation**: Accurate peak detection between samples
- **dBTP Display**: True Peak level in dB Full Scale

### Content Type Modes

| Mode | Target LUFS | Use Case |
|------|-------------|----------|
| Music Non-drums | -11 | Spotify alignment for music with minimal percussion |
| Music Drums | -8 | Default for modern music production |
| Cinema/Trailer | -14 | Broadcast standards (ATSC A/85, EBU R128) |

### Visual Interface

- **LED Strip Meter**: 20-segment color-coded display
- **Color Zones**: Green (balanced), Light Green (near target), Yellow, Orange, Red
- **Target Marker**: Cyan line indicating target loudness
- **Deviation Bar**: Shows deviation from target in LU

## Known Limitations

1. **macOS AU Gatekeeper**: First launch requires manual approval in System Settings
2. **Ableton Live VST3**: Use AU format for best compatibility
3. **REAPER Plugin Rescan**: May require manual rescan after installation
4. **Windows/Linux**: VST3 support compiles but testing limited to macOS

## Performance

| Metric | Value |
|--------|-------|
| CPU per channel (48kHz) | ~0.08% |
| CPU stereo (192kHz) | ~0.35% (worst case) |
| Memory footprint | ~192 bytes |
| Latency | 0 samples (no lookahead) |
| Atomic update rate | 100 Hz |

## Credits

### Development

- **Plugin Architecture**: Single-Pass TETRIS design
- **DSP Implementation**: ITU-R BS.1770-4 compliant algorithms
- **UI Design**: Color-coded LED meter with cached rendering
- **SSOT System**: Centralized constants for maintainability

### Based On

- Original JSFX implementation by IQ (2025)
- ITU-R BS.1770-4 loudness standard
- JUCE 8 framework

### Third-Party

- **JUCE 8**: Audio plugin framework
- **CMake**: Build system
- **C++17**: Programming language

## Version History

| Version | Date | Changes |
|---------|------|---------|
| 1.2.1 | 2026-02-06 | Initial JUCE/C++ release |

## Support

### Documentation

- **USER_MANUAL.md**: Comprehensive user guide
- **REFERENCE.md**: Technical specifications and constants
- **LEARNINGS.md**: Development notes and decisions
- **README.md**: Project overview
- **docs/CODE_SIGNING.md**: Code signing guide
- **NOTARIZATION.md**: Apple notarization guide

### Troubleshooting

**Plugin not appearing in DAW:**
- Verify correct format (AU for macOS)
- Check plugin path is scanned by DAW
- Try manual rescan

**Measurements showing "--.-":**
- Signal may be below absolute gate (-70 LUFS)
- Check input signal levels
- Verify gate behavior

**True Peak showing incorrect values:**
- Test with known peak signals
- 4x oversampling may miss extreme transients
- Consider external metering for verification

**Gatekeeper warning on launch:**
- See Code Signing Status section above
- Check if Developer ID signing is required for distribution
- Review docs/CODE_SIGNING.md for signing instructions

## License

See LICENSE file for details.

---

**BULLsEYE** - Precision Loudness Metering
