# JSFX→JUCE Quick Reference Card

**Print this. Tape it to your monitor.**

---

## The 15 Laws (30-Second Version)

```
1. Algorithm ≠ Output              Real hosts differ from math
2. Implicit → Explicit             Build what JSFX provides free
3. Sentinel ≠ Value                Separate special values from real ones
4. Heuristics fail                 Query APIs, don't guess
5. Transport ≠ Audio               Stopped ≠ no processBlock()
6. Warmup is algorithm             Early blocks count in integrated result
7. Lifecycle = drift               Reset timing affects final value
8. Topology matters                Block size, order, timing all differ
9. Unknowable ≠ unsolvable         Some diffs cost more than they're worth
10. Calibration valid              Empirical offsets are legitimate
11. Synthetic tests lie            Real audio reveals everything
12. Read source, not cache         UI reads APVTS, not DSP object
13. Glitches = timing              Stale values = race conditions
14. Host is active                 DAW is part of your system
15. Code ≠ intent                  Translate intent, not syntax
```

---

## Before You Code

- [ ] Read PORTING_GUIDE.md (30 min)
- [ ] Read MIGRATION_LAWS.md (30 min)
- [ ] Create CONTRACT.md (framework differences)
- [ ] Create SENTINEL_AUDIT.md (special values)
- [ ] Create AUTHORITIES.md (state ownership)

---

## Critical Patterns

### Pattern 1: Transport Detection (LAW 2, 5, 4)
```cpp
// ✅ Right: Use API
auto playhead = getPlayHead()->getPosition();
if (playhead && !playhead->getIsPlaying() && wasPlaying) {
    dspCore.reset();
}

// ❌ Wrong: Heuristic
if (sampleSum == prev) dspCore.reset();  // WILL FAIL
```

### Pattern 2: Sentinel Audit (LAW 3)
```cpp
// ✅ Clear separation
constexpr double INVALID = -900000000.0;  // Sentinel
constexpr double MIN_DB = -120.0;         // Real value

// ❌ Ambiguous
constexpr double THRESHOLD = -119.0;  // Is this special or real?
```

### Pattern 3: UI Thread Safety (LAW 12)
```cpp
// ✅ Read APVTS (source of truth)
auto val = *processor.getAPVTS().getRawParameterValue("param");

// ❌ Read DSP cache (stale, race condition)
auto val = processor.dspCore.getParam();
```

### Pattern 4: Real Audio Testing (LAW 11)
```
Synthetic tests (necessary, insufficient)
+ Real audio A/B (catch 95% of bugs)
+ Cross-DAW testing (catch host-specific issues)
= Production-ready
```

### Pattern 5: Calibration Offset (LAW 9, 10)
```cpp
// If offset is consistent (all files ±0.05 dB):
// 1. Document root cause investigated
// 2. Create named constant
// 3. Add comment explaining it's calibration, not bug

constexpr double CALIBRATION_OFFSET_DB = 1.7;  // Platform-intrinsic
```

---

## Testing Checklist

### Synthetic Tests (20% of time)
- [ ] Sine waves (frequency response)
- [ ] Pink noise (RMS levels)
- [ ] Impulses (impulse response)

### Real Audio A/B (40% of time)
- [ ] 10+ diverse files (pop, electronic, classical, etc.)
- [ ] Compare against reference sample-by-sample
- [ ] Pass: ±0.1 dB on 95%+ of files
- [ ] Investigate any >0.1 dB offset

### Cross-DAW Testing (30% of time)
- [ ] REAPER (most predictable)
- [ ] Ableton Live (different block size)
- [ ] Logic Pro (different sample rate)
- [ ] Cubase (VST3 implementation)

---

## Troubleshooting Flow

```
Symptom                          Check First              See Law
═══════════════════════════════════════════════════════════════════
Measurements 1-2 dB off      → Gate logic (LAW 6)       → MIGRATION_LAWS.md #6
UI shows stale value         → Reading cache (LAW 12)   → PORTING_GUIDE "Pattern 5"
Transport reset doesn't work → Using heuristic (LAW 4)  → MIGRATION_LAWS.md #4
Synthetic tests pass but     → Real audio untested      → PORTING_GUIDE Phase 5
  real audio fails             (LAW 11)
Offset varies per DAW        → Host-specific behavior   → MIGRATION_LAWS.md #14
                               (LAW 14)
```

---

## Code Review Checklist

- [ ] LAW 2: Framework contracts documented?
- [ ] LAW 3: Sentinel audit complete?
- [ ] LAW 4: No heuristics (grep for `if.*count`, `if.*silence`, `if.*sum`)?
- [ ] LAW 6: Warmup behavior tested?
- [ ] LAW 11: Real audio testing done (not just synthetic)?
- [ ] LAW 12: UI reads APVTS, not cache?
- [ ] LAW 14: Cross-DAW testing done?

---

## File Map

```
Your Project Root/
├── PORTING_GUIDE.md        ← Read first (30 min)
├── MIGRATION_LAWS.md       ← Reference laws (30 min)
├── QUICK_REFERENCE.md      ← This file (5 min)
├── CONTRACT.md             ← Your framework differences (create)
├── SENTINEL_AUDIT.md       ← Your special values (create)
├── AUTHORITIES.md          ← Your state ownership (create)
├── Source/SSOT/            ← Constants (no hardcoding)
├── Source/DSP/             ← Algorithm (TETRIS-compliant)
├── Source/Components/      ← UI (thread-safe)
├── tests/                  ← Synthetic + real audio A/B
└── jsfx/                   ← Your original JSFX files
```

---

## Closing: The One Rule

> **If your DSP sounds "almost right" after a port, you are already wrong.**

Acceptable outcomes:
- ✅ Exactly right (tested)
- ✅ Explicitly limited (documented)
- ✅ Defensibly calibrated (reasoned)

Unacceptable outcomes:
- ❌ Almost right (unknown issues lurking)

---

## Key References

**Full Documentation:**
- PORTING_GUIDE.md — Main reference (framework strategy)
- MIGRATION_LAWS.md — Detailed laws (incidents, templates)
- FRAMEWORK_MIGRATION.md — What's new in v2.0

**For Specific Issues:**
- LAW #4 → Transport detection, heuristics
- LAW #6 → Warmup behavior, numerical offsets
- LAW #11 → Testing strategy
- LAW #12 → UI thread safety

**For Code Examples:**
- PORTING_GUIDE.md Part 3 (5 critical patterns)
- MIGRATION_LAWS.md (each law has incident + fix)

---

*Print this. Tape it to your monitor. Reference it daily.*

*The laws exist because developers paid in debugging hours so you don't have to.*
