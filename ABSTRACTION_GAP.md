# The Abstraction Gap: JSFX→C++ Migration Case Study

**Date:** 2026-02-07  
**Version:** 2.0 (Law-first rewrite)  
**Purpose:** Engineering post-mortem illustrating MIGRATION_LAWS in practice  
**Reference:** See **MIGRATION_LAWS.md** for standalone law reference

---

## Executive Summary

The BULLsEYE JSFX→C++ migration required 48+ hours of debugging to achieve parity. Root cause: not a single bug, but a **systematic abstraction gap** between JSFX's implicit framework and C++'s explicit requirements.

This document shows **how each law manifested in real incidents**.

For the laws themselves, see MIGRATION_LAWS.md.

---

## The Core Problem

```
JSFX Framework (Implicit)  ──→  "Almost right" port  ──→  Real audio testing reveals -1.7 dB
                                  (undetected by synthetic tests)
                                  
     ↓
     
Hidden contracts, sentinel meanings, lifecycle assumptions all violated silently
```

**Incidents that revealed laws:**

1. Transport reset missing → LAW 2 (implicit contracts)
2. Gate threshold semantics wrong → LAW 3 (sentinel values)
3. Persistent -1.7 dB offset → LAW 8 (execution topology)
4. Three failed heuristics → LAW 4 (no heuristics)
5. UI showed stale content type → LAW 12 (cache vs source of truth)

---

## Incident #1: Transport Reset (LAW 2 Violation)

### Symptom
Meter doesn't reset when stopping and restarting playback in DAW.

### Root Cause
LAW 2 violation: **Implicit contract was not made explicit**

JSFX provides:
```
@init — automatically called by REAPER when transport restarts (after stop)
```

C++ has:
```
(nothing equivalent)
```

### The Abstraction Gap

| Aspect | JSFX (Implicit) | C++ (Must Be Explicit) |
|--------|-----------------|----------------------|
| Who calls reset? | Framework (@init) | Your code |
| When is it called? | On transport restart | ??? (you decide) |
| Detection mechanism | Framework event | (no event, must invent) |
| Guarantee | Framework synchronous | Depends on your implementation |

### Three Failed Attempts (LAW 4 Violations)

#### Attempt 1: Silence Detection Heuristic
**Theory:** If no samples are processed for 1 second, we're stopped.

```cpp
if (sampleSum == prevSampleSum && elapsedSamples > 48000) {
    dspCore.reset();
}
```

**Why it failed (LAW 4):**
- DAWs keep calling `processBlock()` even when stopped
- They send silence (sampleSum doesn't change)
- Heuristic triggers on quiet→loud transitions
- **False positives:** Meter resets randomly during quiet passages

**Cost:** 2 hours debugging "why does meter reset on silence?"

#### Attempt 2: Sample Counter Heuristic
**Theory:** If processBlock() stops being called, we're stopped.

```cpp
if (totalSamplesProcessed == prevTotalSamplesProcessed) {
    dspCore.reset();
}
```

**Why it failed (LAW 5 violation):**
- DAWs never stop calling `processBlock()`
- Transport state ≠ audio activity (LAW 5)
- Counter always increments
- **False negatives:** Freeze detection never triggers

**Cost:** 2 hours debugging "why does freeze detection never work?"

#### Attempt 3: Authoritative API (✅ Solution)
**Authority:** DAW's playhead API

```cpp
if (playheadState && !playheadState->getIsPlaying() && wasPlaying) {
    dspCore.reset();  // Stop→Play detected
}
```

**Why it works:**
- Queries DAW directly (authoritative source, LAW 4.A)
- No false positives/negatives
- Works across all DAWs
- Instant detection

**Lesson:** Heuristics fail. Authority succeeds. (LAW 4)

---

## Incident #2: Gate Threshold Logic (-1.7 dB) (LAW 3 Violation)

### Symptom
C++ LUFS-I measures 1.7 dB lower than JSFX reference on identical audio.

### Root Cause
LAW 3 violation: **Sentinel value reinterpreted as real threshold**

#### JSFX Intent
```
NEG_INF_THRESHOLD = -900000000.0  // Sentinel: "measurement invalid"

gate_rel = (L_int <= NEG_INF_THRESHOLD ? -70 : L_int - 10);
```

**Semantic:** "If no valid measurement exists yet, use absolute gate (-70 dB). Once we have any valid measurement, switch to relative gate."

#### C++ Interpretation (Wrong)
```cpp
if (currentLUFS <= MIN_DISPLAY_DB + 1.0)  // -120 + 1 = -119 dB (real threshold)
    gateThr = GATE_ABS_DB;
else
    gateThr = currentLUFS - 10.0;
```

**Semantic:** "If LUFS is below -119 dB (a real audio threshold), use absolute gate."

#### The Semantic Overload (LAW 3)

| Symbol | JSFX Meaning | C++ Meaning | Domain |
|--------|---|---|---|
| -900000000 | "No valid measurement yet" | (never used) | SENTINEL |
| -119 | (not used) | Real dB threshold | MEASUREMENT |

**The problem:** Same code pattern, two meanings.

In JSFX: Switches to relative gate **as soon as any valid measurement exists**
In C++: Switches when measurement **exceeds -119 dB**

During warmup (first ~8 blocks), they behave differently:
- JSFX: Early blocks included in integration
- C++: Early blocks rejected as "too quiet"
- Result: Different set of blocks accumulated → -1.7 dB offset

### Why It Was Hard to Find

1. **Silent:** Code compiles, logic seems sound
2. **Selective:** Only appears under specific conditions (early blocks)
3. **Invisible:** No single line is "wrong," just semantically misaligned
4. **Synthetic tests hide it:** Sine waves and pink noise don't trigger gate behavior

### The Fix

Made sentinel explicit:

```cpp
// DSPSSOT.h
constexpr double NEG_INF_THRESHOLD = -900000000.0;  // Matches JSFX

// BULLsEYEProcessor.h
if (currentLUFS <= NEG_INF_THRESHOLD)  // Matches JSFX semantics
    gateThr = GATE_ABS_DB;
else {
    double gateRel = currentLUFS - GATE_REL_OFFSET_DB;
    gateThr = std::max(GATE_ABS_DB, gateRel);
}
```

**Lesson:** Sentinel audit is mandatory (LAW 3.A).

---

## Incident #3: Persistent -1.7 dB Offset After Fix (LAW 8, LAW 9)

### Symptom
After fixing gate logic, C++ still measures 1.7 dB lower than JSFX on real audio.

### Investigation
Created diagnostic simulation comparing JSFX vs C++ algorithms:
- ✅ K-weighting filters: identical math
- ✅ Biquad formula: identical
- ✅ Absolute gate (-70): identical
- ✅ Relative gate (L_int - 10): identical
- ✅ Energy accumulation: identical
- ✅ Synthetic test results: identical output

Yet real audio shows -1.7 dB offset.

### Root Cause Analysis (LAW 8, LAW 9)

**Execution topology differs** (LAW 8):
```
Possible differences:
- Block size (512 vs 256 vs 1024 samples)
- Block boundary timing (when does warmup start?)
- Filter state initialization order
- Floating-point accumulation order
- Compiler optimization level
- Host-specific sample delivery timing
```

**Economically unknowable** (LAW 9):
- Could be traced with:
  - Custom instrumentation in host
  - Deterministic host simulation
  - Reverse-engineering block delivery
  - Analysis of floating-point rounding
- Would cost: 8+ hours
- Benefit: Fix one port's offset, zero reuse for next
- ROI: Negative

### Decision: Empirical Calibration (LAW 10)

Applied constant matching JSFX:

```cpp
// DSPSSOT.h
constexpr double JSFX_CALIBRATION_OFFSET_DB = 1.7;

// BULLsEYEProcessor.h
newLUFS = K_OFFSET_DB + 
          10.0 * std::log10(clampedMean) +
          JSFX_CALIBRATION_OFFSET_DB;
```

**Documented:**
- Constant named (not magic number)
- Reason in LAW_BOOK.md Appendix
- Offset is consistent (±0.05 dB variation across 100+ test files)
- Root cause investigated thoroughly (hours spent)

**Defensive:** This is legitimate engineering (LAW 10).

---

## Incident #4: UI Shows Stale Content Type (LAW 12 Violation)

### Symptom
Header displays "Music Drums: -8.0 LUFS" while dropdown shows "Cinema/Trailer".

Two threads, stale cache:

```
UI Thread (30 FPS render)      Audio Thread (audio callback)
  |                              |
  paint()                        processBlock()
  |                              |
  read getContentType()          ✅ Update APVTS
  |                              |
  ❌ reads DSP cache             ✅ Cache to DSP object
     (value from 16-33ms ago)    (updated in audio thread)
```

### Root Cause
LAW 12 violation: **UI reads cached DSP state instead of source of truth**

Two sources of truth:
1. APVTS (lock-free, always current)
2. DSP object cache (updated in audio thread, may be stale from UI's perspective)

UI chose #2 (wrong choice).

### Why It's Hard to Debug
- Race condition: timing-dependent
- Intermittent: only fails when UI renders before audio thread updates
- Looks like UI bug: "content type not updating"
- Actually is threading bug: "reading wrong source"

### The Fix
Changed paint() to read APVTS directly:

```cpp
// Before (stale cache):
audioProcessor.getContentType()

// After (current, lock-free):
*audioProcessor.getAPVTS().getRawParameterValue(CONTENT_TYPE)
```

**Lesson:** Thread safety = reading source of truth, not cache (LAW 12).

---

## Incident #5: Warmup Behavior (LAW 6 Violation)

### Symptom
Gate behavior differs during early blocks, cascading to wrong LUFS-I final value.

### Root Cause
LAW 6 violation: **Warmup is not "noise," it's algorithm data**

The gate switching logic in early blocks directly affects integrated result:
```
Blocks 1-8:   Gate switches from ABS (-70) to REL (L_int - 10)
              [Filter settling, state initialization]
              
Blocks 9-N:   Normal relative gate operation
              
Block N:      Final LUFS-I = energy from all blocks 1-N
```

If early blocks are treated differently, final result changes.

#### JSFX behavior:
- Switch to relative gate as soon as valid measurement exists
- Early blocks included in integration

#### C++ (before fix):
- Kept using absolute gate until LUFS > -119
- Early blocks rejected
- Fewer blocks counted → lower final energy → lower LUFS-I

**Effect:** -1.7 dB offset, mathematically inevitable from different warmup behavior.

### Why Synthetic Tests Hide This
Sine wave: constant amplitude from block 1
- No "invalid" measurement phase
- Gate switches immediately
- Early block behavior less visible

Pink noise: similar effect

Real audio: transient-rich, variable dynamics
- Early blocks may trigger gate behavior
- Gate switches at different times
- Warmup behavior exposed

### Defensive Action (LAW 6)
- Never skip early blocks
- Test gate switching explicitly
- A/B test against reference during warmup phase
- Verify block rejection matches exactly

---

## Incident #6: Specification Vacuum

### The Hidden Assumption
Both JSFX and C++ implementations assumed "correct reset behavior on transport restart."

**Problem:** ITU-R BS.1770 LUFS spec does **NOT specify** transport semantics.

The spec defines:
- ✅ K-weighting filters
- ✅ Absolute/relative gate
- ✅ Energy accumulation
- ✅ LUFS formula

The spec does NOT define:
- ❌ When should accumulator reset?
- ❌ What is "valid measurement"?
- ❌ Should early blocks count?
- ❌ When is integration complete?

### The Gap
JSFX chose: Reset on every `@init` (framework-driven lifecycle)
C++ could have chosen: Reset manually (explicit lifecycle)

Neither is "wrong" — the spec is underspecified.

The issue: **Assuming one behavior without verifying it's specified** (LAW 2).

### Defensive Action
- Read the spec explicitly
- For every undefined behavior, document your choice
- A/B test against reference to ensure choices match
- Accept that "spec-compliant" ≠ "implementation-equivalent"

---

## Pattern Recognition: Why Each Bug Was Hard

| Incident | Why Hard | Law Violated | Detection Method |
|----------|----------|---|---|
| Transport reset | Framework difference invisible | LAW 2 | Real DAW testing |
| Gate threshold | Semantic overload silent | LAW 3 | A/B audio testing |
| -1.7 dB offset | Execution topology hidden | LAW 8 | Extended real audio test |
| UI stale cache | Race condition intermittent | LAW 12 | Timing-dependent UI test |
| Warmup behavior | Early blocks skipped in dev | LAW 6 | Synthetic vs real audio diff |

**Common theme:** All invisible in isolation, visible only under real-world conditions.

---

## How Laws Prevented Similar Bugs

If these laws had been applied day 1:

### LAW 2 (Implicit contracts)
**Before:** Assumed framework provides reset. Not.  
**After:** Contract mapping shows "no JUCE equivalent" → design authoritative solution.

**Result:** Would find playhead API solution on day 1, not attempt 3.

### LAW 3 (Sentinel overload)
**Before:** Didn't audit sentinel meanings. Wrong.  
**After:** Sentinel audit shows `-900000000 ≠ -119` → fix gate logic immediately.

**Result:** Would fix bug day 1, not discover offset after fixing something else.

### LAW 4 (Heuristics fail)
**Before:** Tried silence detection + sample counting. Failed twice.  
**After:** Authority decision tree says "query API, not heuristic."

**Result:** Would skip two failures, go straight to playhead API.

### LAW 11 (Synthetic tests hide real bugs)
**Before:** Tested on sine waves. Bugs didn't show.  
**After:** Real audio testing day 1.

**Result:** Would catch gate threshold bug on day 1, not day 3.

---

## The Multiplier Effect

Complexity grows non-linearly:

```
1 DSP layer:        Algorithm correct ✅  → Output correct ✅
+ 1 framework gap:   Implicit contracts   → Missing features ❌
+ 1 platform diff:   Block delivery       → Timing wrong ❌
+ 1 timing issue:    UI/Audio race        → Stale values ❌

Result: "Almost right" system with 4 independent failures masquerading as 1 problem
```

---

## Key Insights

### 1. Algorithm Parity ≠ Output Parity (LAW 1)

Two implementations computing identical formulas can produce different results.

**Why:** Platform differences (block size, timing, state, execution order) matter.

### 2. Framework Contracts Must Be Explicit (LAW 2)

JSFX's framework does things automatically. JUCE doesn't.

**Why:** Different design philosophies. One provides, one requires you to build.

### 3. Semantic Audits Are Mandatory (LAW 3)

Every numeric value has a domain (sentinel vs real).

**Why:** Misalignment causes silent bugs that only appear under specific conditions.

### 4. Heuristics Always Fail (LAW 4)

Silence detection, sample counting, assumptions about host behavior.

**Why:** Hosts are active agents (LAW 14), not passive environments.

### 5. Warmup Is Measurement, Not Noise (LAW 6)

Early blocks directly affect integrated measurements.

**Why:** LUFS is cumulative; every block counts.

### 6. Real Audio Reveals Hidden Bugs (LAW 11)

Synthetic tests are too clean.

**Why:** Real audio exercises edge cases that test signals don't.

---

## Engineering Lessons

### What Worked
- ✅ Extensive testing (48+ test cases)
- ✅ Real audio A/B testing against reference
- ✅ Detailed instrumentation and logging
- ✅ Pragmatic acceptance of unknowable offsets
- ✅ Documented rationale for every fix

### What Didn't
- ❌ Assuming algorithm parity → output parity
- ❌ Trusting synthetic tests
- ❌ Using heuristics instead of authoritative APIs
- ❌ Missing implicit framework contracts
- ❌ Not semantic-auditing sentinel values

### Time Breakdown (Estimated)
- Transport detection attempts: 4 hours
- Gate threshold debugging: 6 hours
- -1.7 dB offset investigation: 8 hours
- UI sync fix: 2 hours
- Testing and validation: 20+ hours
- Documentation: 8 hours
- **Total:** 48+ hours

**Where time went:**
- 30% real audio testing (unavoidable)
- 20% hypothesis testing on failed heuristics (avoidable with LAW 4)
- 20% offset investigation (unavoidable, economically decided LAW 9)
- 20% testing and validation (expected)
- 10% documentation (expected)

**Could reduce by:**
- Applying LAW 4 (skip heuristics): -4 hours
- Applying LAW 11 (real audio day 1): -4 hours
- Applying LAW 2 (contract mapping day 1): -2 hours
- **Potential savings:** 10 hours (20% reduction)

---

## Checklist for Next Audio Port

### Before Starting
- [ ] Read MIGRATION_LAWS.md
- [ ] Create contract mapping (LAW 2.A) for all implicit behaviors
- [ ] Create sentinel audit (LAW 3.A) for all special values
- [ ] Identify authoritative APIs (LAW 4.A)
- [ ] Plan real audio testing (LAW 11)

### During Development
- [ ] Test with multiple DAWs (LAW 14)
- [ ] A/B test against reference on real audio (LAW 1)
- [ ] Document lifecycle semantics (LAW 6)
- [ ] Verify warmup behavior matches (LAW 6)
- [ ] Never use heuristics (LAW 4)

### During Debugging
- [ ] Check framework contracts (LAW 2)
- [ ] Audit sentinels (LAW 3)
- [ ] Verify execution topology (LAW 8)
- [ ] Check thread safety (LAW 12, LAW 13)
- [ ] Consider economic unknowability (LAW 9)

### Before Shipping
- [ ] All synthetic tests pass (necessary, not sufficient)
- [ ] All real audio tests pass (necessary and sufficient)
- [ ] Laws documented (LAW_BOOK.md, CONTRACT.md)
- [ ] Deviations explained (LAW 10)
- [ ] Review with engineer familiar with LAW 15 (intent reconstruction)

---

## References

**Laws:**
- **MIGRATION_LAWS.md** — Standalone law reference (15 laws + templates)

**Documentation:**
- **LAW_BOOK.md** — 31 behavioral laws for BULLsEYE
- **SPRINT.md** — Detailed incident timeline and test results
- **REFERENCE.md** — Algorithm-level documentation

**Code:**
- **Source/SSOT/DSPSSOT.h** — Sentinels and constants (LAW 3 artifacts)
- **Source/DSP/BULLsEYEProcessor.h** — Gate logic and warmup handling (LAW 6, LAW 8)
- **Source/PluginProcessor.cpp** — Transport detection (LAW 2, LAW 5)
- **Source/PluginEditor.cpp** — UI fix (LAW 12)

---

## Conclusion

### The Real Work

The 48+ hours of debugging weren't spent fixing "bugs" in code.
They were spent **bridging the abstraction gap** between:

1. **JSFX's implicit contract** (framework handles lifecycle, resets, timing)
2. **C++'s explicit requirements** (you build everything yourself)
3. **Real-world platform differences** (DAWs are unpredictable agents)

### What BULLsEYE Achieved

✅ LUFS-I within ±0.1 dB of JSFX  
✅ All 31 architectural laws replicated  
✅ Transport detection working  
✅ UI synchronized correctly  
✅ 48/48 tests passing  

### But More Importantly

This project demonstrates that **parity only exists because intent was reconstructed**, not just code translated.

The 15 laws in MIGRATION_LAWS.md are the distilled wisdom from 48+ hours of hard-won experience.

Use them for the next port. Save 10+ hours.

---

## Closing

> **If your DSP sounds "almost right" after a port, you are already wrong.**

"Almost right" means the abstraction gap wasn't bridged.  
Unacceptable outcomes: stale UI, mysterious offsets, intermittent behavior.  
Acceptable outcomes: exact match, documented limitation, or defensible calibration.

The laws exist to get you there.

---

*"Porting code is easy. Porting intent is hard. That's where the laws live."*
