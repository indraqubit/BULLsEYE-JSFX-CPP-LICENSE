# Plugin Reference

|Editable SSOT constants for plugin identity|

---

## Plugin Identity

| Field | Value |
|-------|-------|
| **Plugin Name** | BULLsEYE |
| **Plugin Code** | BULL |
| **Company** | IQ |
| **Version** | 1.2.1 |
| **JUCE Version** | 8 |
| **C++ Standard** | C++17 |

---

## JSFX Source

| Field | Value |
|-------|-------|
| **Source File** | jsfx/BULLsEYE-Mini-Combo.jsfx |
| **Author** | IQ 2025 |
| **Description** | LUFS-I Gated Loudness Meter with True Peak 4x Detection |

---

## SSOT File Locations

| Constant Type | File | Namespace |
|---------------|------|-----------|
| Model constants | `Source/SSOT/ModelSSOT.h` | `ModelSSOT` |
| UI constants | `Source/SSOT/UISSOT.h` | `UISSOT` |
| DSP constants | `Source/SSOT/DSPSSOT.h` | `DSPSSOT` |
| Processor constants | `Source/SSOT/ProcessorSSOT.h` | `ProcessorSSOT` |

---

## Loudness Targets

| Content Type | Mode Index | Target LUFS |
|--------------|------------|-------------|
| Music Non-drums | 0 | -11 LUFS |
| Music Drums | 1 | -8 LUFS |
| Cinema/Trailer | 2 | -14 LUFS |

---

## DSP Parameters

### K-Weighting Filters

| Filter | Frequency | Q Factor | Gain |
|--------|-----------|----------|------|
| High-pass | 60 Hz | 0.5 | - |
| High-shelf | 4 kHz | 0.707 | +4 dB |

### Gated Integration

| Gate Type | Threshold |
|-----------|-----------|
| Absolute | -70 LUFS |
| Relative | L-10 |

### True Peak Detection

| Parameter | Value |
|-----------|-------|
| Oversampling | 4x |
| Interpolation | Hermite |
| Block Duration | 400 ms |

---

## UI Constants

### Colors

| Color | RGB | Purpose |
|-------|-----|---------|
| Background | 25, 25, 30 | Plugin background |
| Header | 35, 35, 45 | Header panel |
| Text | 220, 220, 220 | Primary text |
| Green Zone | 60, 180, 60 | Balanced indication |
| Light Green | 100, 200, 100 | Near target |
| Yellow | 220, 180, 0 | Slight deviation |
| Orange | 220, 100, 0 | Warning zone |
| Red | 200, 50, 50 | Out of range |

### Typography

| Element | Size | Style |
|---------|------|-------|
| Header | 16 px | Bold |
| Value | 32 px | Regular |
| Label | 12 px | Regular |

### Layout

| Element | Size |
|---------|------|
| Header Height | 40 px |
| LED Strip Width | 40 px |
| LED Strip Height | 250 px |
| LED Segment | 10 px |

---

## Build Paths

| Platform | Output Path |
|----------|-------------|
| macOS | `build/*.component/` |
| Windows | `build/*.vst3/` |
| Linux | `build/*.so/` |

---

## Update Checklist

When changing plugin identity:

- [ ] Update `CMakeLists.txt`
- [ ] Update `REFERENCE.md`
- [ ] Update `Source/SSOT/ModelSSOT.h` (parameter IDs)
- [ ] Rebuild project

---

## API Endpoints

| Component | Access Method |
|-----------|---------------|
| Processor | `getProcessor()` in PluginEditor |
| Parameters | APVTS (AudioProcessorValueTreeState) |
| DSP State | Member variables in PluginProcessor |

---

## Integration Test Status

| Category | Tests | Status |
|----------|-------|--------|
| APVTS <-> DSP Binding | 3 | PASS |
| State Management | 4 | PASS |
| Sample Rate | 4 | PASS |
| Buffer Size | 3 | PASS |
| Channel Configuration | 4 | PASS |
| Parameter Propagation | 3 | PASS |
| True Peak Detection | 4 | PASS |
| LUFS Integration | 4 | PASS |
| Normalization | 3 | PASS |
| Deviation | 2 | PASS |
| Edge Cases | 5 | PASS |
| Performance/Stress | 3 | PASS |

**Total: 42 integration tests**

---

## DAW Compatibility

| DAW | Format | Status |
|-----|--------|--------|
| REAPER | AU | Tested |
| REAPER | VST3 | Tested |
| Ableton Live | AU | Compatible |
| Logic Pro | AU | Compatible |
| Pro Tools | AU | Compatible |
| Cubase | VST3 | Compatible |

---

## Known Limitations

1. macOS security warnings on first AU load (approve in System Settings)
2. Ableton Live 10 VST3 compatibility (use AU format)
3. REAPER may require manual plugin rescan after installation

---

**Last Updated:** 2026-02-06
**Version:** 1.2.1