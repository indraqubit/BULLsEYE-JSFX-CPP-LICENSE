# AGENT MENTAL MODEL

**Purpose:** Provide a shared reasoning model for all agents working on BULLsEYE, especially Sprint 5 licensing work.

---

## 1) Core Model: Contract-in-Contract

Think in nested contracts. Every change must satisfy **all** layers:

1. **Product Contract**
   - Deliver sprint goals without regressions.
   - Preserve existing behavior unless intentionally changed and documented.

2. **Framework Contract (JUCE)**
   - Respect lifecycle: construction, prepare, process, state save/restore, teardown.
   - Respect thread ownership: audio thread vs message/UI/background threads.

3. **API Contract (Method-Level)**
   - Each method has invariants (timing, blocking behavior, side effects).
   - `processBlock()` is realtime-critical and must remain deterministic.

4. **Host + Platform Contract**
   - DAWs may call lifecycle methods in different timing patterns.
   - Packaging/signing/runtime linking rules must be satisfied per platform.

If contracts conflict, prioritize: **Audio safety > Correctness > UX convenience > Build ergonomics**.

---

## 2) Non-Negotiables (Red Lines)

### License State Authority (SSOT)
- **Single source of truth:** one atomic license snapshot owned by EngineCore.
- **Write authority:** UI/background paths may update state only through EngineCore commit/swap API.
- **Read authority:** audio thread is read-only (atomic snapshot reads only).
- **Visibility rule:** no partial state publication; publish only complete versioned snapshot swaps.

### Audio Thread Red Lines
- No locks, no heap allocation, no disk I/O, no network calls in `processBlock()`.
- Use atomic or lock-free reads only for license checks in audio path.
- Keep fallback behavior deterministic when license state is stale/unavailable.

### Deterministic Fallback Rule
- If license state is **unknown/stale/unavailable**, runtime behavior must be exactly **TRIAL behavior** as defined by SSOT flags.
- This fallback must be test-covered and identical across all hosts.

### UI / Background Red Lines
- Activation/network/state refresh can run off audio thread only.
- UI timers and refresh must not back-pressure audio.

### State & Persistence Red Lines
- State restore must be safe across DAW reloads.
- Corrupt/invalid license data must degrade gracefully (e.g., safe trial fallback).

---

## 3) Sprint 5 Contract Matrix (Execution Checklist)

### C1 — Audio Thread Contract
- License checks in audio path are non-blocking and atomic.
- No realtime violations introduced by licensing logic.

### C2 — UI Thread Contract
- Activation/status operations run on safe threads.
- UI refresh cadence is bounded (e.g., 1 Hz target).

### C3 — Host Lifecycle Contract
- Engine initialization boundary is explicit: **initialize in `prepareToPlay()`** (not first `processBlock()`).
- `getStateInformation()` / `setStateInformation()` maintain consistent license state.

### C4 — Persistence & Security Contract
- License state survives intended lifecycle events.
- Platform storage backend behavior is explicit (macOS/Windows/Linux).

### C5 — Build/Packaging Contract
- macOS bundle/rpath/codesign checks pass.
- Installer work is **TBD** unless explicitly activated.
- Windows/Linux deployment scope remains **TBD** unless promoted to active sprint scope.

### C6 — Test Contract
- Existing regression suite remains green.
- New license tests prove thread, lifecycle, persistence, and deployment constraints.
- Must include test: **license state change during active playback**.
- Must include test: **DAW reload with corrupt license blob**.

---

## 4) Agent Execution Protocol

### Before Changes
1. Identify affected contract layers.
2. Define pass/fail criteria per layer.
3. Mark any TBD scope explicitly (avoid accidental implied completion).

### During Changes
1. Keep edits minimal and contract-driven.
2. Avoid hidden behavior changes.
3. Maintain phase/status consistency in planning docs.

### After Changes
1. Run contradiction check (status, phase naming, scope labels).
2. Re-validate red lines (especially audio thread safety).
3. Record what contract evidence now exists (tests, commands, artifacts).

---

## 5) Failure Modes & Escalation

Common failure patterns:
- Marking outcomes complete before implementation evidence exists.
- Mixing future-scope (TBD) items into active sprint done criteria.
- Introducing blocking work in realtime callbacks.

Escalation rule:
- If any audio-thread contract is uncertain, stop and request review before merging.

---

## 6) Subagent Brief (Copy-Paste)

Use this prompt when delegating:

> Operate using BULLsEYE contract-in-contract model.
> 
> Validate each change against: (1) Product contract, (2) JUCE framework contract, (3) API method contract, (4) Host/platform contract.
> 
> Hard red lines: no blocking/alloc/I-O/network in processBlock; no status contradictions in sprint docs; no implicit completion for TBD scope.
> 
> Return: risks found, contract violations found (if any), and concrete evidence for compliance.

---

## 7) Scope Notes

- This document is an execution aid, not a replacement for LAW_BOOK, SSOT, or framework docs.
- In conflicts, treat LAW_BOOK + verified tests as authoritative evidence.
