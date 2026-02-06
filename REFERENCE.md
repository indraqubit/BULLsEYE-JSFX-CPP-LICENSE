# Plugin Reference

**Editable SSOT constants for plugin identity**

---

## Plugin Identity

| Field | Value |
|-------|-------|
| **Plugin Name** | YourPluginName |
| **Plugin Code** | YPCD |
| **Company** | YourCompany |
| **Version** | 1.0.0 |
| **JUCE Version** | 8 |
| **C++ Standard** | C++17 |

---

## SSOT File Locations

| Constant Type | File | Namespace |
|---------------|------|-----------|
| Model constants | `Source/SSOT/ModelSSOT.h` | `ModelSSOT` |
| UI constants | `Source/SSOT/UISSOT.h` | `UISSOT` |
| DSP constants | `Source/SSOT/DSPSSOT.h` | `DSPSSOT` |
| Processor constants | `Source/SSOT/ProcessorSSOT.h` | `ProcessorSSOT` |

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
- [ ] Update SSOT namespace comments
- [ ] Rebuild project

---

**Last Updated:** 2026-02-06
