# JSFX → JUCE: Framework Migration Engineering Guide

**Version:** 2.0  
**Purpose:** Portable reference for porting JSFX plugins to JUCE/C++  
**Audience:** Audio engineers, plugin developers, anyone migrating JSFX code  
**Based on:** BULLsEYE case study (Feb 2026) + IntelliMute experience  

---

## Quick Start: The 15 Laws

If you're in a hurry, these 15 laws prevent 80% of porting bugs:

| # | Law | Quick Summary |
|---|-----|---|
| 1 | Algorithm ≠ Output | Identical math ≠ identical results |
| 2 | Implicit → Explicit | JSFX framework ≠ JUCE framework |
| 3 | Sentinel ≠ Value | Special values need domain separation |
| 4 | Heuristics fail | No silence detection, no guessing |
| 5 | Transport ≠ Audio | Stopped ≠ no blocks |
| 6 | Warmup is algorithm | Early blocks matter to integrated result |
| 7 | Lifecycle = drift | Reset timing affects final value |
| 8 | Topology matters | Block size, order, timing all affect result |
| 9 | Unknowable ≠ unsolvable | Some diffs are economically irrational to trace |
| 10 | Calibration valid | Empirical offsets are legitimate |
| 11 | Synthetic tests lie | Real audio reveals hidden bugs |
| 12 | Read source, not cache | UI must read APVTS, not DSP object |
| 13 | Glitches = timing | Stale values are threading bugs |
| 14 | Host is active | DAW behavior is part of your system |
| 15 | Code ≠ intent | Translate intent, not just syntax |

**Read time: 2 minutes. Read the full laws: 30 minutes. See MIGRATION_LAWS.md.**

---

## Why This Guide Exists

BULLsEYE JSFX→C++ migration took 48+ hours to debug. The bugs weren't in code—they were in **framework contracts no one wrote down**.

This guide codifies what was learned. Use it to save 10-20 hours on your next port.

---

## Part 1: Before You Start Porting

### Step 1: Contract Mapping (2 hours)

For every JSFX layer, write down its JUCE equivalent **before touching code**.

**Template:**

```markdown
## JSFX Layer: @init

**Behavior:** Called once at startup, and on transport restart (after stop)

**Purpose:** 
- Initialize filter states
- Reset measurement accumulators
- Set up gate thresholds

**JUCE Equivalent:**
- Constructor for initialization
- ??? for reset on transport restart (must design)

**Authority:** Who guarantees this?
- JSFX: REAPER framework (automatic)
- JUCE: Your code (must implement)

**Thread-safe:** Yes/No?

**Testable:** How?

---

## JSFX Layer: @slider

**Behavior:** Called when user changes a parameter

**Purpose:**
- Update DSP cache with new parameter

**JUCE Equivalent:**
- APVTS parameter listener callback

**Authority:**
- JUCE: AudioProcessorValueTreeState listener

**Thread-safe:** Depends on implementation. Use lock-free where possible.

**Testable:** Can mock parameter changes in unit test.
```

**Goal:** Write this doc **before** you write C++ code. It forces you to ask hard questions about framework differences.

**Deliverable:** `PROJECT_NAME_CONTRACT.md` in your repo root.

---

### Step 2: Sentinel Audit (1 hour)

Every magic number in JSFX has a meaning. Make it explicit.

**Template:**

```cpp
// ❌ From JSFX (implicit meaning)
if (L_int <= -900000000) {
    // ...
}

// ✅ What does -900000000 mean?
// Answer: "Measurement hasn't started yet" (sentinel, not a dB value)

// ✅ In C++, make it explicit:
namespace DSPSSOT {
    // Sentinel: marks "no valid measurement yet"
    constexpr double NEG_INF_THRESHOLD = -900000000.0;
    
    // Real value: lowest displayable LUFS
    constexpr double MIN_DISPLAY_DB = -120.0;
}

// ✅ Use it with clear semantic intent:
if (currentLUFS <= DSPSSOT::NEG_INF_THRESHOLD) {
    // "No valid measurement yet, use absolute gate"
    gateThr = GATE_ABS_DB;
} else {
    // "Valid measurement exists, use relative gate"
    gateThr = currentLUFS - 10.0;
}
```

**Goal:** Every sentinel gets a named constant + comment explaining its domain (sentinel vs real).

**Deliverable:** Add sentinel audit to DSPSSOT.h. Review before code review.

---

### Step 3: Authority Inventory (1 hour)

For every state/timing question, identify the **authoritative source**.

**Questions to answer:**

| Question | Source (Don't use heuristics) | Note |
|----------|------|------|
| Is transport playing? | DAW playhead API | `AudioPlayHead::getPosition()->getIsPlaying()` |
| What sample rate? | JUCE processor callback | `getSampleRate()` |
| What block size? | JUCE processor callback | `getBlockSize()` |
| Parameter changed? | APVTS listener | Use listener callback, not polling |
| Did audio stop? | ❌ Silence detection | ✅ Playhead API |
| Did filter settle? | ❌ Sample counter | ✅ Measure via test, apply offset if needed |

**Goal:** No heuristics, no guessing. Every piece of state comes from an authoritative API.

**Deliverable:** `AUTHORITIES.md` — document which API owns each piece of state.

---

## Part 2: During Porting

### Phase 1: Replicate Contract (Week 1)

**Do:**
- ✅ Implement the contract you documented in Step 1
- ✅ Create explicit reset mechanism (for transport, filter settling, etc.)
- ✅ Add playhead polling in `processBlock()` if needed
- ✅ Connect APVTS listeners to DSP updates

**Don't:**
- ❌ Try to replicate JSFX's layer structure exactly
- ❌ Use heuristics (silence, sample counting, pattern matching)
- ❌ Cache DSP state for UI to read (more on this in Part 3)

**Test:**
- Create unit tests for every contract element
- Verify reset is called at right time
- Verify parameter changes propagate

**Check:**
- [ ] Contract doc is up to date
- [ ] No heuristics in code
- [ ] All authorities are used, not guessed
- [ ] Thread-safe (no shared mutable state across threads)

---

### Phase 2: Algorithm Replication (Week 1-2)

**Do:**
- ✅ Port line-by-line (K-weighting filters, gates, accumulators)
- ✅ Use synthetic test cases first (sine, pink noise)
- ✅ Verify on real audio immediately (not day 3)
- ✅ Instrument code with detailed logging
- ✅ Create A/B test harness comparing against reference

**Don't:**
- ❌ Trust synthetic tests alone
- ❌ Assume "looks close" is good enough
- ❌ Skip real audio testing to "save time"

**Test:**
- Run 48+ test cases (synthetic + real audio)
- Compare DSP output sample-by-sample against reference (first 1000 samples)
- Log warmup behavior (gate switching, filter settling)
- Budget 20% of time for "mysterious" numerical drift

**Check:**
- [ ] All synthetic tests pass
- [ ] Real audio A/B test against reference
- [ ] Warmup behavior matches
- [ ] Offset quantified (if any)

---

### Phase 3: Handle Platform Differences (Week 2)

**Law 6: Warmup is algorithm, not noise**

Verify:
- Gate switching behavior in early blocks
- Filter state initialization
- Block boundary effects on integrated measurements
- Reset timing across DAWs

**Law 8: Execution topology matters**

Accept that:
- Block size differs per DAW
- Floating-point accumulation order differs
- Compiler optimizations reorder operations
- Some offsets are platform-intrinsic

**If offset is consistent:**
- Measure it (all 100+ test files within ±0.05 dB?)
- Document it (root cause investigated?)
- Calibrate it (add constant, document in code + LAW_BOOK.md)

**If offset is intermittent:**
- Find the bug (it's there)
- Never "tune" it away

**Test:**
- [ ] Warmup behavior A/B tested
- [ ] Real audio on 3+ DAWs
- [ ] Block size variation tested (if possible)
- [ ] Offset quantified and documented

---

### Phase 4: UI Synchronization (Week 2-3)

**Law 12: UI reads source of truth, not cache**

Architecture:

```
APVTS (Source of Truth, lock-free)
  ↑
  |
  ├→ Audio Thread (reads APVTS via parameter listeners)
  |
  └→ UI Thread (reads APVTS directly)

❌ Wrong: UI reads DSP cache (updated in audio thread)
✅ Right: UI reads APVTS (always current, lock-free)
```

**Implementation:**

```cpp
// ✅ In paint():
auto contentType = *audioProcessor.getAPVTS().getRawParameterValue("contentType");

// ❌ Not this:
auto contentType = audioProcessor.getContentType();  // DSP cache
```

**Test:**
- [ ] Parameter change immediately reflects in UI
- [ ] No stale values (test on slow system, high CPU load)
- [ ] No thread-safety warnings

---

### Phase 5: Cross-Platform Testing (Week 3-4)

**Law 14: Host is active agent**

Test on multiple DAWs:

| DAW | OS | Format | Must Test |
|-----|----|----|---|
| REAPER | macOS | AU, VST3 | ✅ Yes (best platform for testing) |
| Ableton | macOS | AU | ✅ Yes (block size varies) |
| Logic | macOS | AU | ✅ Yes (sample rate varies) |
| Cubase | macOS | VST3 | ✅ Optional (similar to REAPER) |
| REAPER | Windows | VST3 | ✅ Yes (if targeting Windows) |
| FL Studio | Windows | VST3 | ✅ Yes (host behavior differs) |

**Checklist:**
- [ ] Plugin loads without errors
- [ ] Audio processes without clicks/pops
- [ ] Parameter changes work (UI + audio)
- [ ] Transport restart works (if applicable)
- [ ] Measurements match across DAWs (within tolerance)
- [ ] No CPU spikes or dropouts
- [ ] Project save/load works

---

## Part 3: Critical Patterns

### Pattern 1: Transport Detection (LAW 2, LAW 5)

**Problem:** How do you reset DSP on stop→play like JSFX does?

**Solution:** Use playhead API (LAW 4.A - authority, not heuristic)

```cpp
// In processBlock():
auto playheadState = getPlayHead()->getPosition();

if (playheadState) {
    bool isPlaying = playheadState->getIsPlaying();
    
    // Detect stop→play transition
    if (isPlaying && !wasPlaying) {
        dspCore.reset();  // Reset just like JSFX @init
    }
    
    wasPlaying = isPlaying;
}
```

**Not this:**

```cpp
// ❌ Heuristic 1: Silence detection (false positives)
if (sampleSum == prevSampleSum && elapsedSamples > 48000) {
    dspCore.reset();  // Resets on quiet audio!
}

// ❌ Heuristic 2: Block counter (false negatives)
if (totalSamples == prevTotalSamples) {
    dspCore.reset();  // Never triggers on most hosts
}
```

---

### Pattern 2: Warmup Behavior (LAW 6)

**Problem:** Gate switching in early blocks affects final LUFS-I.

**Solution:** Verify gate behavior matches reference exactly.

```cpp
// Document in header:
// Gate Logic:
// - Absolute gate: -70 dB (until valid measurement)
// - Relative gate: L_int - 10 LU (once valid measurement exists)
// - Warmup: First ~8 blocks at absolute gate
//
// These early blocks ARE counted in LUFS-I integration.
// Different warmup behavior = different final offset.

// In DSP core:
if (currentLUFS <= NEG_INF_THRESHOLD) {
    // "No valid measurement yet, warmup phase"
    gateThr = GATE_ABS_DB;
} else {
    // "Valid measurement, switch to relative gate"
    gateThr = currentLUFS - GATE_REL_OFFSET_DB;
}
```

**Test:**
- Compare gate switching timing against reference
- Verify same blocks are accepted/rejected
- Log gate behavior in first 100ms
- A/B test warmup phase

---

### Pattern 3: Sentinel Overload (LAW 3)

**Problem:** Special "invalid" marker uses same domain as real values.

**Solution:** Explicit constants + clear naming.

```cpp
// ✅ Clear separation of domains:
namespace DSPSSOT {
    namespace Sentinels {
        constexpr double INVALID_MEASUREMENT = -900000000.0;  // Domain: invalid
    }
    
    namespace Thresholds {
        constexpr double MIN_DISPLAYABLE = -120.0;    // Domain: dB
        constexpr double GATE_ABSOLUTE = -70.0;       // Domain: dB
        constexpr double GATE_RELATIVE_OFFSET = 10.0; // Domain: LU
    }
}

// ❌ Wrong: shares domain
constexpr double THRESHOLD = -119.0;  // Is this a sentinel or a real value?
```

---

### Pattern 4: Numerical Offset (LAW 9, LAW 10)

**Problem:** Consistent -1.7 dB offset despite identical algorithms.

**Solution:** Measure, document, calibrate.

```cpp
// Step 1: Measure offset across 100+ test files
// Result: Consistent -1.7 dB ±0.05 dB variation

// Step 2: Investigate root cause
// - K-weighting: identical ✅
// - Biquad: identical ✅
// - Gate logic: identical ✅
// - Synthetic tests: identical ✅
// - Real audio: -1.7 dB offset ❌
// Root cause: Execution topology (economically unknowable)

// Step 3: Document decision
constexpr double JSFX_CALIBRATION_OFFSET_DB = 1.7;

// Step 4: Apply calibration
newLUFS = K_OFFSET_DB + 
          10.0 * std::log10(clampedMean) +
          JSFX_CALIBRATION_OFFSET_DB;

// Step 5: Add to LAW_BOOK.md or README
// "C++ implementation has consistent 1.7 dB offset vs JSFX reference.
//  Root cause: Platform-intrinsic differences in block delivery timing
//  and floating-point accumulation order. Offset is deterministic and
//  documented. See DSPSSOT.h for constant definition."
```

---

### Pattern 5: UI Thread Safety (LAW 12, LAW 13)

**Problem:** UI sometimes shows stale parameter values.

**Solution:** Read APVTS directly, not DSP cache.

```cpp
// Architecture:
// 
// APVTS (single source of truth, lock-free)
//   ↑
//   └→ Audio thread reads (for DSP updates)
//   └→ UI thread reads (for rendering)
//
// DSP cache (optimization only, not authoritative)
//   ↑
//   └→ Audio thread updates (in processBlock)
//   └→ UI MUST NOT read (is stale, race condition)

// ✅ In paint():
auto value = *audioProcessor.getAPVTS().getRawParameterValue("myParam");

// ❌ Not this:
auto value = audioProcessor.dspCore.getCachedValue();
```

---

## Part 4: Testing Strategy

### Synthetic Tests (Necessary, Not Sufficient)

```cpp
TEST(BiquadFilter, SineWave_1kHz) {
    // Test at specific frequency
    // Verify magnitude response
    // Verify phase response
}

TEST(GateLogic, AbsoluteGate_BelowThreshold) {
    // Test gate switching at -70 dB
    // Verify block rejection
}

TEST(LUFS, PinkNoise_RMS) {
    // Test on standard test signal
    // Verify output within tolerance
}
```

**Budget:** 20% of testing time

**Limitation:** Doesn't catch platform differences

---

### Real Audio A/B Testing (Necessary and Sufficient)

```
Objective: Compare DSP output against reference implementation

Test Set:
- 10 diverse music files (pop, electronic, classical, drums, acapella, etc.)
- 100+ LUFS measurements per file (gate behavior, warmup, long-term)
- Peak measurements
- Parameter change scenarios

Procedure:
1. Feed audio to reference (JSFX)
2. Feed audio to implementation (C++)
3. Compare measurements:
   - Final LUFS-I (target tolerance: ±0.1 dB)
   - Gate switching behavior (identical block rejection)
   - Warmup phase (identical first 100ms)
   - Peak detection (identical max values)

Pass Criteria:
- LUFS-I within ±0.1 dB on 95%+ of test files
- Any offset >0.1 dB must be investigated + documented
- Gate logic identical (sample-by-sample)
- Warmup behavior identical
```

**Budget:** 40% of testing time

**ROI:** Catches 95% of bugs that synthetic tests miss

---

### Cross-DAW Testing (Necessary for Production)

```
Test on:
- REAPER (closest to JSFX)
- Ableton Live (different block size)
- Logic Pro (different sample rate handling)
- Cubase (different VST3 implementation)
- Pro Tools (if applicable)

For each DAW:
- [ ] Plugin loads
- [ ] Audio processes
- [ ] Parameters change
- [ ] Measurements match (within tolerance)
- [ ] Transport works (if implemented)
- [ ] No crashes or artifacts
```

**Budget:** 30% of testing time

**ROI:** Catches host-specific behaviors before users report them

---

## Part 5: Checklist for Your Project

### Before Coding (Week 0)
- [ ] Read MIGRATION_LAWS.md (this file + full laws)
- [ ] Create CONTRACT.md (framework differences documented)
- [ ] Create SENTINEL_AUDIT.md (special values documented)
- [ ] Create AUTHORITIES.md (state ownership documented)
- [ ] Identify which DAWs you'll test on
- [ ] Set up A/B testing framework

### During Porting (Week 1-3)
- [ ] Phase 1: Contract replicated (reset, lifecycle, events)
- [ ] Phase 2: Algorithm ported (synthetic tests pass)
- [ ] Phase 3: Real audio tested (A/B against reference)
- [ ] Phase 4: UI synchronized (no stale values)
- [ ] Phase 5: Cross-DAW tested (3+ DAWs work)

### Before Shipping (Week 4)
- [ ] All 15 laws reviewed (MIGRATION_LAWS.md)
- [ ] No heuristics in codebase (search for comments that sound hacky)
- [ ] Sentinel audit complete (DSPSSOT.h reviewed)
- [ ] Offsets documented (LAW_BOOK.md or README)
- [ ] Thread safety verified (no shared mutable state)
- [ ] Code review by someone who read MIGRATION_LAWS.md

### Post-Launch Monitoring
- [ ] Monitor user reports for host-specific issues
- [ ] Document any new platform differences discovered
- [ ] Update AUTHORITIES.md, CONTRACT.md if needed
- [ ] Contribute findings back to this guide

---

## Troubleshooting

### Symptom: "Measurements off by 1-2 dB"

**Check:**
1. Gate threshold semantics (LAW 3) — sentinel vs real value
2. Warmup behavior (LAW 6) — are early blocks included?
3. Block rejection logic (LAW 6) — same blocks accepted/rejected as reference?

**Verify:**
- A/B test on real audio (not synthetic)
- Compare gate switching frame-by-frame
- Log gate threshold values during warmup

---

### Symptom: "UI shows stale content type"

**Check:**
1. Is UI reading from cache or APVTS? (LAW 12)
2. Are there race conditions? (LAW 13)

**Verify:**
```cpp
// ❌ Wrong: reads DSP cache
auto type = audioProcessor.dspCore.getContentType();

// ✅ Right: reads APVTS (lock-free, current)
auto type = *audioProcessor.getAPVTS().getRawParameterValue("contentType");
```

---

### Symptom: "Transport reset doesn't work"

**Check:**
1. Are you using a heuristic? (LAW 4)
2. Is playhead API available? (LAW 4.A)

**Verify:**
```cpp
// ❌ Wrong: heuristic
if (sampleSum == prev && elapsedSamples > 48000) {
    dspCore.reset();
}

// ✅ Right: authoritative API
if (playheadState && !playheadState->getIsPlaying() && wasPlaying) {
    dspCore.reset();
}
```

---

## Key Files in This Template

| File | Purpose |
|------|---------|
| **REFERENCE.md** | Algorithm documentation (constants, formulas) |
| **LEARNING.md** | Development notes and discoveries |
| **MIGRATION_PLAN.md** | Your migration roadmap |
| **PORTING_GUIDE.md** | This file — framework migration guide |
| **Source/SSOT/** | Constants (no hardcoding) |
| **Source/DSP/** | Algorithm implementation |
| **tests/** | Unit + A/B tests |

---

## Closing: The Core Principle

> **If your DSP sounds "almost right" after a port, you are already wrong.**

"Almost right" means:
- Intent wasn't reconstructed (LAW 15)
- Framework contracts weren't explicit (LAW 2)
- Assumptions weren't validated (LAW 6, 7, 8)
- Real audio wasn't tested (LAW 1, 11)

Acceptable results:
- ✅ **Exactly right** — within tolerance, tested
- ✅ **Explicitly limited** — documented trade-off
- ✅ **Defensibly calibrated** — consistent offset, reason documented
- ❌ **Almost right** — accepted without understanding

Choose the first three.

---

## References

- **MIGRATION_LAWS.md** — Full 15 laws with templates
- **REFERENCE.md** — Algorithm documentation
- **LEARNING.md** — Development discoveries
- **Source/SSOT/DSPSSOT.h** — Constants (sentinels, thresholds, offsets)
- **tests/** — Test examples

---

## Attribution

This guide is based on the BULLsEYE JSFX→C++ migration (Feb 2026).

Use for any JSFX→JUCE project. Attribution appreciated but not required.

---

*"Porting code is easy. Porting intent is hard. Accept it. Use the laws."*
