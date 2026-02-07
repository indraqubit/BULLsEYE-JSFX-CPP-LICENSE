# Changelog

All notable changes to BULLsEYE plugin will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [v1.2.1] - 2026-02-06

### Added
- Initial JUCE/Javascript/C++ migration release
- LUFS-I gated loudness measurement with ITU-R BS.1770 compliance
- True Peak 4x oversampling detection with Hermite interpolation
- LED strip meter with 20-segment color-coded display
- Content type modes: Music Non-drums, Music Drums, Cinema/Trailer
- Audio Unit (AU) format for macOS
- VST3 format for cross-platform compatibility
- Single-Pass TETRIS architecture for thread safety
- Single Source of Truth (SSOT) constants system
- Performance optimized DSP core with cached filter coefficients

### Changed
- Migrated from JSFX to JUCE 8 framework
- Implemented APVTS parameter management
- Added batched atomic updates for True Peak UI updates
- Optimized LED strip meter with position/color caching

### Fixed
- Filter coefficient recalculation on sample rate change
- NaN/infinity input validation in DSP chain
- Denormal handling for CPU efficiency
- Energy overflow protection

### Technical
- C++17 with JUCE 8
- SSOT constants in Source/SSOT/ (ModelSSOT, UISSOT, DSPSSOT, ProcessorSSOT)
- 42 integration tests passing
- macOS compatible (AU, VST3)

