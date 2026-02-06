# MIGRATION LAWS: DSP Porting Engineering Reference

**Version:** 1.0  
**Context:** BULLsEYE JSFX → C++ migration post-mortem  
**Purpose:** Transferable engineering laws for cross-platform audio DSP porting  
**Audience:** Audio engineers, systems architects, code reviewers

---

## LAW 1 — Algorithm Parity ≠ Output Parity

**Statement:** Two implementations sharing identical mathematical formulas **do not guarantee identical audio output** on real hosts.

**Why it matters:**
- Identical code ≠ identical execution topology
- Platform semantics (block size, timing, state initialization) always matter
- Algorithm-level testing (synthetic signals) **will hide** this gap

**Incident:** BULLsEYE gate threshold fix matched JSFX math exactly, but -1.7 dB offset persisted.  
**Cost:** Assumed "math identical = output identical" for 3+ hours before real audio testing.

**Defensive action:**
- A/B test against reference on **real audio material** (day 1, not day 3)
- Budget 20%+ of porting time for "mysterious" numerical drift
- Empirical validation > theoretical equivalence

---

## LAW 2 — Implicit Framework Contracts Must Be Made Explicit

**Statement:** Source frameworks provide lifecycle behaviors (initialization, reset, lifecycle events) **implicitly**. Target frameworks require them **explicitly**.

**Why it matters:**
- JSFX: Framework resets on `@init` (automatic on transport restart)
- JUCE: You must call `reset()` yourself
- If you miss this translation, features silently break

**Implicit contracts in source:**
| Contract | JSFX Provides | C++ Must Implement |
|----------|---------------|-------------------|
| Transport events | `@init` auto-call | Poll playhead API |
| Lifecycle reset | Framework-driven | Manual call in handler |
| State machine | Layer architecture (@init, @slider, @sample, @gfx) | Explicit state object |
| UI event ordering | `@slider` → `@sample` → `@gfx` guaranteed | APVTS callback + thread safety |

**Incident:** Transport reset missing. Attempted 3 workarounds (silence heuristic, sample counter, finally playhead API).  
**Cost:** 6+ hours to realize "framework doesn't do this automatically."

**Defensive action:**
- Create explicit "contract mapping" document before coding (LAW 2.A)
- For each JSFX event, write down its C++/JUCE equivalent
- If equivalent doesn't exist, design one with authority in mind (LAW 4)

---

## LAW 2.A — Contract Mapping Template (Mandatory)

```
JSFX Layer:        @init / @slider / @sample / @gfx
Behavior:          [what JSFX does]
JUCE Equivalent:   [where this lives in C++]
Authority:         [what guarantees it? playhead? callback? polling?]
Thread-safe:       [yes/no, and implications]
Testable:          [can this be unit tested?]
```

Example (Transport):
```
JSFX Layer:        @init
Behavior:          Called when transport restarts (after stop)
JUCE Equivalent:   processBlock() with playhead check
Authority:         DAW playhead state (getIsPlaying())
Thread-safe:       Yes (read-only, no state mutation)
Testable:          Partially (can mock playhead, but DAW behavior varies)
```

---

## LAW 3 — Sentinel Values Must Never Share Domain With Real Values

**Statement:** If a sentinel (special "invalid" marker) uses the same numeric domain as real measurements, **parity is already broken silently**.

**Why it matters:**
- Sentinel meanings are implicit in source code
- When porting, you may reinterpret sentinel as a real value
- Bug emerges only under specific conditions (early blocks, edge cases)
- **Silent:** Code compiles, tests pass, only real audio reveals it

**The gate threshold incident:**

JSFX:
```
NEG_INF_THRESHOLD = -900000000.0  // sentinel: "measurement invalid"
gate_rel = (L_int <= NEG_INF_THRESHOLD ? GATE_ABS : GATE_REL)
```
Meaning: "If measurement hasn't started yet, use absolute gate."

C++ (incorrect):
```
if (currentLUFS <= MIN_DISPLAY_DB + 1.0)  // -120 + 1 = -119 dB
    gateThr = GATE_ABS;
else
    gateThr = currentLUFS - 10.0;
```
Meaning: "If LUFS is below -119, use absolute gate."

**Result:** Different gate behavior during warmup phase → different block rejection → -1.7 dB offset.

**Problem:** Same code structure, **different semantics**.

**Defensive action:**
- Create "sentinel audit" before porting (LAW 3.A)
- Tag every special numeric value with its semantic domain:
  - `-900000000` = INVALID_MEASUREMENT (sentinel, not dB)
  - `-119.0` = MIN_DISPLAY_DB (real dB value)
- Never reuse sentinel domain for real values
- Explicit enum/constant better than magic number

---

## LAW 3.A — Sentinel Audit Template (Mandatory)

```
Symbol              | Numeric Value    | Domain           | Meaning
--------------------|------------------|------------------|------------------
NEG_INF_THRESHOLD   | -900000000.0    | INVALID (sentinel)| Measurement not started
MIN_DISPLAY_DB      | -120.0          | dB (real)        | Lowest displayable LUFS
GATE_ABS_DB         | -70.0           | dB (real)        | Absolute gate threshold
```

Rule: **If columns 3 and 4 don't align, audit fails.**

---

## LAW 4 — Heuristics Are Temporary Lies Waiting for the Right Test Case

**Statement:** Heuristics (silence detection, sample counting, pattern matching) **always fail** on some real host, some real audio, some real DAW behavior.

**Why it matters:**
- Heuristic = "I assume X, so I'll infer Y"
- Assumptions are invisible until violated
- Violations happen in production, not lab
- Authoritative source (API, spec, host guarantee) > heuristic always

**Transport detection evolution:**

| Attempt | Heuristic | Theory | Reality | Why Failed |
|---------|-----------|--------|---------|-----------|
| #1 | `sampleSum == prev` = stopped | No audio activity = stopped | DAW sends silence while stopped | Heuristic triggers on quiet→loud transitions (false positive) |
| #2 | `totalSamples == prev` = stopped | Sample counter freezes = stopped | DAW never stops calling processBlock() | Heuristic never triggers (false negative) |
| #3 | `playhead.getIsPlaying()` | Query host transport state directly | Host authoritative API | ✅ Works |

**Cost:** Two failed attempts = 4+ hours. Both "looked right" in isolated testing.

**Defensive action:**
- Ask first: "Is there an authoritative API for this?"
- If yes: Use it. (LAW 4.A)
- If no: Document as "best effort," plan replacement
- Test heuristics on **multiple DAWs** immediately (not day 2)

---

## LAW 4.A — Heuristic vs Authority Decision Tree

```
Question:              Answer:        Action:
Is there a spec?       → YES          Use spec (it's authoritative)
Is there an API?       → YES          Query API (it's authoritative)
Is there a callback?   → YES          Register handler (it's authoritative)
Is there a guarantee?  → NO           Accept heuristic (document it, flag for replacement)
```

Never go left-to-right. Go top-to-bottom.

---

## LAW 5 — Transport State Is NOT Audio Activity

**Statement:** A host in "stopped" or "paused" state **does not mean** `processBlock()` stops being called.

**Why it matters:**
- JSFX runs in REAPER, which stops calling `@sample` when paused
- JUCE/VST3 hosts keep calling `processBlock()` even when stopped
- These are **fundamentally different semantics**
- Assuming REAPER semantics in JUCE = silent bug

**The confusion:**
```
Wrong assumption:  stopped → no blocks → can infer "stopped"
Reality:           stopped → silence blocks → blocks keep coming
```

**Incident:** Tried to detect stopped state by checking if `processBlock()` stops being called. It never did.  
**Cost:** 2 hours debugging "why freeze detection never triggers."

**Defensive action:**
- Transport state and audio activity are **independent variables**
- Query one via playhead API
- Check other via energy detection (if needed)
- Never use one to infer the other

---

## LAW 6 — Warmup Behavior Is Part of the Algorithm, Not Noise

**Statement:** The behavior of DSP during early blocks (filter settling, gate switching, state initialization) **directly affects final integrated measurements**.

**Why it matters:**
- LUFS-I is an integrated (cumulative) measurement
- Early blocks set filter state, accumulator initial conditions
- Warmup phase isn't "skipped" — it's counted in the mean
- Different warmup = different final result, mathematically

**Incident:** Gate threshold bug caused different early blocks to be rejected.
- JSFX: Accepted blocks starting at `measurement_invalid`
- C++: Accepted blocks starting at `-119 dB`
- Difference: ~8 blocks of K-weighted energy at startup
- Effect on LUFS-I: -1.7 dB offset (consistent, reproducible)

**Defensive action:**
- Never skip early blocks in testing
- A/B test warmup behavior explicitly:
  - How many blocks before gate switches from ABS to REL?
  - Are early blocks included in LUFS-I accumulation?
  - Does filter settling affect final result?
- Instrument to verify gate behavior matches reference

---

## LAW 7 — Lifecycle Mismatch Causes Statistical Drift

**Statement:** If reset timing differs between implementations, the number of accumulated blocks differs, causing final integrated value to drift.

**Why it matters:**
- Every reset clears accumulators, starts fresh
- Different reset = different set of blocks counted
- Result: offset that looks "random" but is mathematically inevitable

**Compounded with LAW 6:**
- Wrong warmup behavior (LAW 6) + wrong reset timing (LAW 7) = larger drift
- No single bug, multiple causes
- Appears as mysterious numerical offset

**Defensive action:**
- Document reset contract explicitly (when is `reset()` called?)
- Verify reset behavior matches reference frame-for-frame
- If A/B test shows offset: check reset timing first
- Never assume "close enough" — integrated measurements are sensitive to this

---

## LAW 8 — Identical Math Can Diverge Due to Execution Topology

**Statement:** Two implementations computing the same formula can produce different results due to:
- Block size (512 vs 1024 samples)
- Accumulation order (what gets summed first?)
- State initialization (filter history at startup)
- Compiler optimizations (reordering float ops)

These are **not bugs**. They're platform differences.

**Why it matters:**
- Line-by-line translation **does not guarantee numerical equivalence**
- Floating-point arithmetic is non-associative
  - `(a + b) + c` ≠ `a + (b + c)` in IEEE 754
- Compiler optimization can reorder operations
- Different orders → different rounding → different result

**The unknown offset (-1.7 dB):**
After fixing gate threshold (LAW 3), offset persisted.
Investigation:
- K-weighting math: identical ✅
- Biquad formula: identical ✅
- Accumulation logic: identical ✅
- Synthetic test results: identical ✅
- Real audio: -1.7 dB offset ❌

**Possible causes** (all invisible in code):
- Block boundary timing (512 vs 256 vs 1024 blocks)
- Filter state initialization order
- Floating-point accumulation order
- Compiler optimizer reordering float ops
- Host-specific sample delivery timing

**Cost of investigation:** Potentially infinite. No ROI for tracing.

**Defensive action:**
- Accept that topology matters
- Use empirical calibration if offset is **consistent and deterministic** (LAW 10)
- Do NOT try to "fix" mysterious offsets through more code changes
- Document and move on

---

## LAW 9 — "Unknowable" Means Economically Irrational to Trace, Not Theoretically Impossible

**Statement:** Some offsets **can be traced** (theoretically) but **should not be** (economically).

**Why it matters:**
- Tracing requires: deterministic host + instrumentation + reverse-engineering host internals
- ROI: Fix one port's offset, zero reuse for next port
- Real engineering: Accept boundary conditions, calibrate

**The distinction:**
```
Theoretically unknowable:  Cannot be known even with infinite investigation
                          (e.g., quantum randomness — doesn't apply to DSP)

Economically unknowable:   Can be known, but cost > benefit
                          (e.g., host-specific block delivery pattern)
```

**Incident:** -1.7 dB offset exists, cause unknown (economically).  
**Decision:** Empirical calibration constant. Done.

**Why NOT chase it:**
- Offset is deterministic (same on every test)
- No bugs are masked (gate logic is correct)
- Fix would be host-specific (no generalization)
- Cost: 8+ hours, benefit: 0 (just move offset to different location)

**Defensive action:**
- If offset is **consistent**: calibrate and document (LAW 10)
- If offset is **intermittent**: find the root cause (LAW 6, 7, 8)
- If offset is **unexplainable**: investigate further (but set a time budget)

---

## LAW 10 — Empirical Calibration Is Legitimate Engineering

**Statement:** If a numerical offset is **consistent**, **deterministic**, and **documented**, applying an empirical calibration constant is **valid engineering**, not a hack.

**Why it matters:**
- Professional metering equipment uses calibration offsets
- Standard engineering practice for analog-to-digital / cross-platform conversion
- Better to have a known, documented 1.7 dB offset than a mysterious -1.7 dB error
- Calibration = acceptance of platform realities

**BULLsEYE approach:**
```cpp
// DSPSSOT.h
constexpr double JSFX_CALIBRATION_OFFSET_DB = 1.7;

// BULLsEYEProcessor.h
newLUFS = K_OFFSET_DB + 
          10.0 * std::log10(clampedMean) +
          JSFX_CALIBRATION_OFFSET_DB;  // Empirical offset to match JSFX
```

**Documentation:**
- Constant is named, not magic number
- Rationale documented in LAW_BOOK.md
- Offset is consistent and reproducible
- Root cause documented (economically unknowable, platform-intrinsic)

**Defensive action:**
- Before applying calibration, verify:
  - ✅ Offset is consistent across test vectors (yes)
  - ✅ No bugs are being masked (gate logic correct, tested)
  - ✅ Offset is deterministic (always 1.7 dB, never 1.6 or 1.8)
  - ✅ Root cause investigated thoroughly (yes, hours spent)
- Apply with confidence, document rationale

---

## LAW 11 — Synthetic Tests Lie

**Statement:** Testing with sine waves, pink noise, impulses, and test signals **will hide bugs** that real audio reveals.

**Why it matters:**
- Synthetic signals are "clean" (no transients, no edge cases, no statistical variation)
- Real audio has:
  - Attack/release transients (trigger gate behavior)
  - Dynamic range variation (expose edge cases)
  - Phase relationships (reveal filter state issues)
  - Statistical correlation (stress accumulators)
- Gate threshold bug passed all synthetic tests; failed on real music

**BULLsEYE incident:**
- **Synthetic testing:** Both implementations produced identical LUFS-I on sine, pink noise
- **Real audio testing:** -1.7 dB offset visible immediately
- **Root cause:** Gate switching logic during warmup (hidden in clean signals)

**Defensive action:**
- Never trust synthetic tests alone
- Test with:
  - ✅ Sine (check frequency response)
  - ✅ Pink noise (check RMS levels)
  - ✅ Real music (check real behavior)
  - ✅ Edge cases (near-silence, loud peaks, transients)
- A/B test against reference early (LAW 1)

---

## LAW 12 — UI Must Read the Source of Truth, Not a Cache

**Statement:** If UI reads cached DSP state while audio thread updates APVTS, **race conditions manifest as "stale" UI**.

**Why it matters:**
- Two threads: audio (blocking) + UI (asynchronous)
- Race: parameter changes in UI → APVTS updates (lock-free) → audio thread reads (delayed)
- If UI reads audio thread's cache, it reads stale value
- Result: UI shows wrong value for 16-33ms (one frame)

**Incident:** Header showed "Music Drums: -8.0 LUFS" while dropdown showed "Cinema/Trailer".

**Root cause:**
```
UI Thread (paint)              Audio Thread (processBlock)
  |                              |
  read getContentType()          update APVTS parameter
  |                              |
  ❌ reads DSP cache             cache DSP object
     (old value)                 |
                                 (next callback, UI renders old value)
```

**Fix:**
```cpp
// Before (stale):
audioProcessor.getContentType()  // DSP cached, updated in audio thread

// After (current):
*audioProcessor.getAPVTS().getRawParameterValue(CONTENT_TYPE)  // Lock-free, always current
```

**Defensive action:**
- APVTS is source of truth, not DSP object
- UI reads from APVTS directly (lock-free safe)
- DSP caches for efficiency, but UI must not read the cache
- Never assume audio thread will have updated DSP state by render time

---

## LAW 13 — Thread Safety Bugs Masquerade as "UI Glitches"

**Statement:** When UI "sometimes" shows wrong values, the root cause is usually **temporal inconsistency**, not logic error.

**Why it matters:**
- Logic bugs are consistent (always wrong)
- Timing bugs are intermittent (sometimes wrong, depends on thread interleaving)
- Harder to debug because they're non-deterministic
- Appear as "flicker," "stale values," "occasional wrong display"

**Why BULLsEYE's header bug was hard to spot:**
- Symptom: header showed stale content type (intermittent)
- Not every parameter change; only when UI renders before audio thread updates
- Only visible under specific timing (high CPU load, slow audio callbacks)
- Looks like UI bug (not updating), actually is threading bug (reading wrong source)

**Defensive action:**
- When UI shows "wrong" value: suspect timing, not logic
- Check thread safety:
  - Is DSP state read by UI thread? (LAW 12: don't)
  - Are there unsynchronized state modifications? (use lock-free, APVTS)
  - Is there a race between parameter change and state update?
- Use lock-free structures (APVTS, atomics)
- Never share mutable state across threads

---

## LAW 14 — Host Behavior Is Part of Your System

**Statement:** The DAW/host is not a passive environment. It's an **active agent** that determines:
- Block delivery size and timing
- Transport state representation
- Sample-accurate timing accuracy
- Callback ordering guarantees

Different hosts have different behaviors.

**Why it matters:**
- REAPER (source): synchronous, predictable, calls @init on transport restart
- JUCE/VST3 (target): asynchronous, host-dependent, different playhead API
- Porting code requires porting assumptions **about the host**
- Same plugin code, different host = different behavior

**Examples of host-specific behavior:**
| Behavior | REAPER | JUCE/VST3 | Impact |
|----------|--------|----------|--------|
| processBlock during stop | ❌ No | ✅ Yes | Transport detection strategy |
| Playhead state API | @init call | getPosition()->getIsPlaying() | Reset timing |
| Block size | Fixed | Variable | Filter state, accumulator size |
| @init equivalent | Guaranteed | None (manual) | Lifecycle management |

**Incident:** Silence heuristic worked in lab testing (single host), failed on real DAWs.  
**Cost:** Multiple deploy cycles, user reports.

**Defensive action:**
- Test on **multiple DAWs** (REAPER, Ableton, Logic, Cubase)
- Document host-specific behaviors
- Use authoritative APIs instead of heuristics (LAW 4)
- Budget for "host-specific edge cases" in testing

---

## LAW 15 — Porting Code Is Translation; Porting Intent Is Reconstruction

**Statement:** Code is easy to translate. Intent (why the code is written that way) is hard to reconstruct.

**Intent lives in:**
- Sentinel values and their meanings (LAW 3)
- Lifecycle guarantees (LAW 2)
- Implicit framework contracts (LAW 2)
- Edge case handling (what happens at startup, shutdown, parameter change?)
- Statistical sensitivity (which operations affect integrated result?)

**Why this matters:**
If you don't reconstruct intent, your code is **"almost right"** — which is already wrong (see closing statement).

**The gate threshold example:**
```
Code:  if (L_int <= THRESHOLD) gate = GATE_ABS; else gate = GATE_REL;

Intent (JSFX):  "Switch from absolute to relative gate as soon as valid measurement exists"
Intent (C++):   (missed) "Switch when LUFS exceeds -119 dB"

Result: Same code, different intent → wrong behavior
```

**Defensive action:**
- Before translating code, document **intent** explicitly
- For every algorithm section, write:
  - What problem does this solve?
  - What are the edge cases?
  - What assumptions must hold true?
  - How does this interact with other layers?
- Translate intent first, code second

---

## Closing Statement

> **If your DSP sounds "almost right" after a port, you are already wrong.**

"Almost right" means:
- Intent wasn't reconstructed (LAW 15)
- Implicit contracts weren't made explicit (LAW 2)
- Assumptions weren't validated (LAW 6, 7, 8)
- Real audio wasn't tested early (LAW 1, 11)

Acceptable results:
- ✅ **Exactly right** (within tolerance, tested)
- ✅ **Explicitly wrong** (documented limitation, understood trade-off)
- ✅ **Defensibly calibrated** (consistent offset, documented reason, LAW 10)
- ❌ **Almost right** (system accepted, root cause unknown, lurking bugs)

---

## Quick Reference: Law Index

| Law | Core Statement |
|-----|---|
| 1 | Algorithm ≠ Output |
| 2 | Implicit → Explicit contracts |
| 2.A | Contract mapping template |
| 3 | Sentinel ≠ Real value |
| 3.A | Sentinel audit template |
| 4 | Heuristics fail silently |
| 4.A | Authority decision tree |
| 5 | Transport state ≠ Audio activity |
| 6 | Warmup is algorithm, not noise |
| 7 | Lifecycle mismatch = drift |
| 8 | Math identical ≠ execution identical |
| 9 | Unknowable = economically irrational |
| 10 | Empirical calibration is valid |
| 11 | Synthetic tests hide real bugs |
| 12 | Read source of truth, not cache |
| 13 | UI glitches are timing bugs |
| 14 | Host is active agent, not environment |
| 15 | Code translation ≠ intent reconstruction |

---

## License & Attribution

These laws are derived from the BULLsEYE JSFX→C++ migration (2026-02-06).  
They are offered as engineering reference for audio DSP porting projects.  
Attribution appreciated, not required.

---

*"Porting code is easy. Porting intent is hard. Accept it."*
