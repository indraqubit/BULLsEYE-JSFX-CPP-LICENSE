# BULLsEYE Sprint 5 Tracker

**License Integration Assessment & Implementation**

---

## 🎯 SPRINT 5 - LICENSE INTEGRATION

| Field | Value |
|-------|-------|
| **Sprint** | 5 |
| **Start Date** | 2026-02-07 (planned) |
| **End Date** | TBD |
| **Goal** | Integrate Portable License Drop-In Kit, define macOS installer plan (execution TBD) |
| **Status** | ✅ PRE-FLIGHT ASSESSMENT COMPLETE — Ready for Phase 1 execution |
| **Phase** | Staging (no production, no Apple Developer ID yet) |

---

## Current Context

### Previous Sprint Status
- **Sprint 3:** Architecture Laws & Behavioral Analysis - ✅ COMPLETE
- **Sprint 4:** Universal Binary Support - ✅ COMPLETE
- **Current State:** v2.0.0 Production Ready, 48/48 tests passing

### Sprint 5 Objectives
1. Assess and integrate Portable License Drop-In Kit
2. Implement license engine (trial, activation, offline validation)
3. Define macOS installer approach with both trial and licensed build targets (execution TBD)
4. Maintain all existing functionality and test coverage
5. Prepare for cross-platform deployment (Windows/Linux) in future sprint

### Trial Configuration (Confirmed)
- **Trial Duration:** 14 days (adjustable via `LicenseSSOT::TRIAL_DAYS`)
- **Trial Restrictions:** No audio muting during trial (`TRIAL_MUTE_AUDIO = false`)
- **Trial Watermark:** "TRIAL" overlay visible (`TRIAL_SHOW_WATERMARK = true`)
- **Grace Period:** 7 days after subscription expiration
- **Offline Validation:** 30+ days support

### License Configuration
- **Network Validation:** Reference available (see `portable-license-drop-in/NETWORK_VALIDATION_REFERENCE.md`)
- **API Base URL:** `https://byiqaudio.com/api/v1`
- **Endpoints:** Generate, Verify, Deactivate, Transfer, Health
- **Timeout:** 30 seconds
- **Max Retries:** 3 attempts with exponential backoff (500ms → 1000ms → 2000ms)
- **Storage:** macOS Keychain (secure), file-based fallback
- **Activation:** License key input dialog in UI
- **Persisted State:** License survives DAW session reload and plugin reinstall

---

## Sprint 5 Backlog

### Phase 1: Foundation — CMake & Include Path Setup

| Task | Status | Priority | Estimate | Dependencies |
|------|--------|----------|----------|--------------|
| Create LicenseSSOT.h constants | ⏳ TODO | P0 | 0.5d | None |
| Add network API configuration to LicenseSSOT.h | ⏳ TODO | P0 | 0.5d | Network validation reference |
| Update CMakeLists.txt include paths | ⏳ TODO | P0 | 0.5d | LicenseSSOT.h |
| Add OpenSSL conditional build option | ⏳ TODO | P1 | 0.5d | CMakeLists.txt |
| Build verification (no functional changes) | ⏳ TODO | P0 | 0.5d | All above |

**Milestone:** Project builds successfully with license engine headers included

**Notes:**
- Network API configuration based on `NETWORK_VALIDATION_REFERENCE.md`
- API Base URL: `https://byiqaudio.com/api/v1`
- Endpoints: `/plugins/licenses/generate`, `/plugins/licenses/verify`, `/plugins/licenses/deactivate`, `/plugins/licenses/transfer`, `/plugins/health`
- Timeout: 30 seconds, Max Retries: 3

---

### Phase 2: Core Engine — PluginProcessor Integration

| Task | Status | Priority | Estimate | Dependencies |
|------|--------|----------|----------|--------------|
| Add license engine member to PluginProcessor | ⏳ TODO | P0 | 0.5d | Phase 1 |
| Implement initializeLicenseEngine() | ⏳ TODO | P0 | 0.5d | LicenseSSOT.h |
| Add isLicensedAtomically() gate in processBlock | ⏳ TODO | P0 | 1.0d | License engine init |
| Add license accessor methods for UI | ⏳ TODO | P1 | 0.5d | Engine integration |
| Verify audio thread safety (no allocations) | ⏳ TODO | P0 | 0.5d | All above |

**Milestone:** License checks integrated into audio path, zero allocations

---

### Phase 3: UI — License Components

| Task | Status | Priority | Estimate | Dependencies |
|------|--------|----------|----------|--------------|
| Create LicenseStatusComponent.h/cpp | ⏳ TODO | P0 | 1.0d | PluginProcessor accessors |
| Add license status bar to PluginEditor | ⏳ TODO | P0 | 0.5d | LicenseStatusComponent |
| Implement activation dialog | ⏳ TODO | P1 | 1.0d | LicenseStatusComponent |
| Add license UI constants to UISSOT.h | ⏳ TODO | P1 | 0.5d | None |
| Timer-based status refresh (1 Hz) | ⏳ TODO | P2 | 0.5d | Component integration |

**Milestone:** License status visible in UI, activation flow functional

---

### Phase 4: macOS Deployment Compliance

| Task | Status | Priority | Estimate | Dependencies |
|------|--------|----------|----------|--------------|
| Verify bundle structure integrity | ⏳ TODO | P0 | 0.5d | Phase 1 |
| Create deployment verification script | ⏳ TODO | P1 | 1.0d | LAW 1-9 review |
| Configure OpenSSL bundling with @rpath | ⏳ TODO | P0 | 1.0d | Network validation reference |
| Test otool -L (no absolute paths) | ⏳ TODO | P0 | 0.5d | Verification script |
| Configure codesigning for license builds | ⏳ TODO | P0 | 1.0d | Bundle verified |
| Create entitlements file (if network license) | ⏳ TODO | P1 | 0.5d | Network validation spec |
| Clean Mac testing (no Homebrew) | ⏳ TODO | P0 | 1.0d | Codesigning configured |

**Milestone:** macOS deployment compliant with ANMO Laws 1-9

**Notes:**
- OpenSSL dylibs: `libssl.3.dylib`, `libcrypto.3.dylib`
- Auto-detect Homebrew: `/opt/homebrew` (Apple Silicon) vs `/usr/local` (Intel)
- Bundling: Copy to `Contents/Frameworks/`, patch with `install_name_tool` to use `@rpath`
- Verification script based on `NETWORK_VALIDATION_REFERENCE.md` section 6.5

---

### Phase 5: macOS Installer Creation (TBD)

| Task | Status | Priority | Estimate | Dependencies |
|------|--------|----------|----------|--------------|
| Design installer requirements | 💤 TBD | P0 | TBD | None |
| Create `pkgbuild`/`productbuild` spec | 💤 TBD | P0 | TBD | Requirements |
| Build trial version installer | 💤 TBD | P0 | TBD | Phase 4 |
| Build licensed version installer | 💤 TBD | P0 | TBD | Phase 4 |
| Create uninstaller script | 💤 TBD | P1 | TBD | Installer spec |
| Test install/uninstall cycle | 💤 TBD | P0 | TBD | All installers built |

**Milestone:** macOS installer plan finalized (implementation TBD)

---

### Phase 6: Testing & Validation

| Task | Status | Priority | Estimate | Dependencies |
|------|--------|----------|----------|--------------|
| Create 20 new license test cases | ⏳ TODO | P0 | 1.5d | Test requirements doc |
| Implement audio thread safety tests | ⏳ TODO | P0 | 0.5d | Test framework |
| Implement UI license tests | ⏳ TODO | P0 | 0.5d | Test framework |
| Implement state persistence tests | ⏳ TODO | P0 | 0.5d | Test framework |
| Verify all 48 existing tests still pass | ⏳ TODO | P0 | 1.0d | All phases |
| DAW testing (REAPER, Logic, Ableton) | ⏳ TODO | P0 | 2.0d | Plugin builds |
| Deployment verification script run | ⏳ TODO | P0 | 0.5d | Verification script |

**Milestone:** All tests passing, DAW validation complete

---

### Phase 7: Documentation & Release

| Task | Status | Priority | Estimate | Dependencies |
|------|--------|----------|----------|--------------|
| Update CHANGELOG.md for v2.1.0 | ⏳ TODO | P1 | 0.5d | All features complete |
| Update README.md with licensing section | ⏳ TODO | P1 | 1.0d | Feature complete |
| Add Law 32 to LAW_BOOK.md (License Thread Safety) | ⏳ TODO | P1 | 0.5d | Law review |
| Create INSTALLER_GUIDE.md | ⏳ TODO | P1 | 1.0d | Installer ready |
| Update REFERENCE.md with LicenseSSOT | ⏳ TODO | P2 | 0.5d | LicenseSSOT complete |
| Create release tag with metadata | ⏳ TODO | P1 | 0.5d | All documentation |

**Milestone:** Documentation complete, ready for v2.1.0 release

---

## Sprint 5 Timeline

### Week 1: Foundation & Core Integration
- Days 1-2: Phase 1 (Foundation)
- Days 3-4: Phase 2 (PluginProcessor Integration)

### Week 2: UI & macOS Compliance
- Days 5-6: Phase 3 (License Components)
- Days 7-8: Phase 4 (macOS Deployment Compliance)

### Week 3: Installer & Testing
- Days 9-10: Phase 5 (macOS Installer, TBD)
- Days 11-12: Phase 6 (Testing & Validation)

### Week 4: Documentation & Release
- Days 13-14: Phase 7 (Documentation & Release)

**Total Estimated Duration:** ~12-14 working days

---

## Dependencies & Blockers

### External Dependencies

| Dependency | Status | Impact |
|------------|--------|--------|
| Portable License Drop-In Kit | ✅ Available | P0 - Required |
| Network validation specification | ✅ Available | P1 - Required for online validation |
| OpenSSL 3.x | ⚠️ Optional | macOS/Linux for HTTPS validation |
| WinHTTP | ⚠️ Optional | Windows for HTTPS validation |
| Apple Developer ID | ❌ Not needed yet | Staging phase only |

### Internal Dependencies

| Component | Required For | Status |
|-----------|--------------|--------|
| LicenseSSOT.h | All phases | ⏳ To be created |
| PluginProcessor integration | UI components | ⏳ Pending Phase 2 |
| macOS compliance | Installer | ⏳ Pending Phase 4 |

### Known Blockers

**All Pre-Flight Blockers RESOLVED ✅**

| Blocker | Status | Resolution |
|---------|--------|-----------|
| Network validation spec not verified | ✅ RESOLVED | `NETWORK_VALIDATION_REFERENCE.md` verified (765 lines, comprehensive API spec, OpenSSL strategies, ANMO compliance) |
| License engine architecture unconfirmed | ✅ RESOLVED | Confirmed header-only, zero compilation, dual thread-safe API, no pre-built binaries |
| Portable License Drop-In Kit integration approach | ✅ RESOLVED | Simple Drop-In approach selected (cleanest for BULLsEYE), full compatibility confirmed, no build conflicts |

**No Current Blockers for Phase 1 Execution**

---

## Phase 1 Pre-flight Assessment Plan

**Objective:** Validate three critical blockers before Phase 1 execution starts

### Assessment Task 1: Network Validation Spec Verification

**Goal:** Confirm `NETWORK_VALIDATION_REFERENCE.md` exists and contains required API configuration

| Task | Owner | Status | Acceptance Criteria |
|------|-------|--------|-------------------|
| Check if `portable-license-drop-in/NETWORK_VALIDATION_REFERENCE.md` exists | TBD | ✅ PASS | File exists and contains sections 6.1-8.3 |
| Extract API endpoints, timeouts, retry strategy | TBD | ✅ PASS | Document shows: `/generate`, `/verify`, `/deactivate`, `/transfer`, `/health` endpoints |
| Validate URL and configuration format | TBD | ✅ PASS | Base URL format: `https://byiqaudio.com/api/v1`, timeout=30s, retries=3 |
| Cross-reference with LicenseSSOT requirements | TBD | ✅ PASS | API spec matches Phase 1 task expectations (line 68-71) |

**Output:** ✅ Network Validation Spec Verified
- File: `portable-license-drop-in/NETWORK_VALIDATION_REFERENCE.md` (765 lines, comprehensive)
- API Base URL: `https://byiqaudio.com/api/v1` ✅
- Endpoints: `/plugins/licenses/generate`, `/plugins/licenses/verify`, `/plugins/licenses/deactivate`, `/plugins/licenses/transfer`, `/plugins/health` ✅
- Timeout: 30 seconds ✅
- Max Retries: 3 attempts with exponential backoff (500ms → 1000ms → 2000ms) ✅
- Content-Type: `application/json` ✅
- OpenSSL configuration: Detailed platform-specific strategies (Windows static, macOS dynamic, Linux shared) ✅
- Verification scripts: Bundle validation, dylib checks, codesign verification ✅
- ANMO compliance: Laws 1-10 mapped and verified ✅

**Status:** ✅ **BLOCKER 1 RESOLVED** - Network validation spec is comprehensive and ready for Phase 1 LicenseSSOT.h configuration

---

### Assessment Task 2: License Engine Architecture Review

**Goal:** Confirm Portable License Drop-In Kit is truly header-only (no runtime compilation needed)

| Task | Owner | Status | Acceptance Criteria |
|------|-------|--------|-------------------|
| Audit `portable-license-drop-in/` directory structure | TBD | ✅ PASS | Identify all `.h`, `.hpp`, `.cpp` files; document architecture |
| Identify header-only vs compiled components | TBD | ✅ PASS | List any `.cpp` files that require compilation |
| Check for runtime dependencies (OpenSSL, WinHTTP, etc.) | TBD | ✅ PASS | Document platform-specific dependencies in drop-in kit |
| Verify no pre-built binaries included | TBD | ✅ PASS | Confirm no `.a`, `.lib`, `.so`, `.dylib` in kit |
| Extract integration interface (includes, namespaces) | TBD | ✅ PASS | Document main include path and public API |

**Output:** ✅ License Engine Architecture Verified
- **Header-Only Core:** ✅ Three implementations all `.hpp` only, no `.cpp` compilation required
  - `LICENSE_ENGINE.hpp` — Main engine with audio-safe atomic API
  - `LICENSE_ENGINE_EXTRACTOR.hpp` — Source extraction layer for AnalogMorph reuse
  - `LICENSE_ENGINE_FACTORIZED.hpp` — Factored implementation with dependency injection
- **No Pre-Built Binaries:** ✅ No `.a`, `.lib`, `.so`, `.dylib` files in kit
- **Platform Dependencies:** ⚠️ Optional (not bundled, configured at build time)
  - Windows: WinHTTP (built-in, no vcpkg needed)
  - macOS: OpenSSL 3.x (optional, via Homebrew auto-detection)
  - Linux: OpenSSL 3.x (optional, system package)
- **Integration Interface:**
  - Main include: `#include "portable-license-drop-in/core/LICENSE_ENGINE.hpp"`
  - Namespace: `LicenseEngine::`
  - Core class: `LicenseEngine::LicenseEngine`
  - Config: `LicenseEngine::LicenseConfig`
- **Thread Safety Model:** ✅ Dual API design
  - Audio thread: `isLicensedAtomically()`, `getDaysRemainingAtomically()`, `isFeatureEnabledAtomically()` (non-blocking, atomic)
  - UI thread: `initialize()`, `activateLicense()`, `refreshLicenseStatus()`, `isLicensed()` (may block, mutex-protected)
- **Key Features:**
  - Trial periods (14+ days configurable)
  - Subscription licenses with grace periods (7 days default)
  - Perpetual licenses
  - Offline validation (30+ days)
  - Drop-data fallback with `getLastKnownGoodState()`

**Status:** ✅ **BLOCKER 2 RESOLVED** - License engine is truly header-only, zero compilation overhead, ready for Phase 2 integration

---

### Assessment Task 3: Portable License Drop-In Kit Integration Audit

**Goal:** Map drop-in kit components to Phase 1-7 tasks

| Task | Owner | Status | Acceptance Criteria |
|------|-------|--------|-------------------|
| List all public headers in drop-in kit | TBD | ✅ PASS | Document file paths for `LicenseEngine.h`, `LicenseSSOT.h`, etc. |
| Identify required constants and enums | TBD | ✅ PASS | Match drop-in constants to Phase 1 task requirements (trial days, mute flag, watermark) |
| Check for CMakeLists.txt or build configuration | TBD | ✅ PASS | Document if kit provides CMake integration or manual configuration needed |
| Validate compatibility with JUCE/existing build system | TBD | ✅ PASS | Confirm drop-in kit doesn't conflict with current CMakeLists.txt |
| Extract UI component requirements | TBD | ✅ PASS | Document what Phase 3 UI components must implement |
| Review platform-specific code (macOS Keychain, Windows Registry, Linux) | TBD | ✅ PASS | Confirm platform support matches deployment target (macOS staging) |

**Output:** ✅ Integration Mapping Verified
- **Public API Headers:** Core engine ready for direct inclusion
  - `portable-license-drop-in/core/LICENSE_ENGINE.hpp` — Main engine
- **Integration Guide Provided:** ✅ Three approaches documented
  1. Simple Drop-In (Recommended for BULLsEYE) — Cleanest, no AnalogMorph coupling
  2. Source Extraction — For AnalogMorph reuse
  3. Factored Implementation — Maximum flexibility with DI
- **Recommended Approach for BULLsEYE:** Simple Drop-In (#1)
  - No external dependencies
  - Header-only integration
  - SSOT-friendly configuration
  - Matches BULLsEYE's own identity
- **UI Component Interface:** Clearly defined
  - `LicenseEngine::getLicenseStatusMessage()` → Status display
  - `LicenseEngine::activateLicense()` → Activation dialog
  - `LicenseEngine::isLicensed()` → Feature gates
- **Platform Support:** ✅ Staging phase target met
  - **macOS:** ✅ Keychain storage (Foundation.framework + Security.framework)
  - **Windows:** ✅ Registry storage (WinHTTP built-in)
  - **Linux:** ✅ File-based storage (~/.config/)
- **Build System:** ✅ No CMakeLists conflicts
  - Kit is header-only, no build configuration needed
  - OpenSSL is conditional (optional, configured at build time)
  - CMake changes: Only add include paths and source files
- **Documentation Provided:** ✅ Comprehensive
  - `LICENSE_DROP_IN_LAWS.md` — Threading rules (Laws 1-9)
  - `INTEGRATION_GUIDE.md` — Step-by-step with code examples
  - `DEPENDENCIES_ASSESSMENT.md` — Platform-specific details
  - `LEARNING_CURVE_ANALYSIS.md` — Learning curve (3-4 days expected)
  - `VERIFICATION_REPORT.md` — Verification checklist
  - `LICENSE_ENGINE_INTEGRATION_PLAN.md` — Phase-by-phase integration for BULLsEYE
- **Related Documentation in Kit:**
  - `baseline/01_THREAD_SAFETY.md` — Audio thread safety laws
  - `examples/` — VST3, AU, Standalone, AnalogMorphV3 examples

**Status:** ✅ **BLOCKER 3 RESOLVED** - Drop-in kit is fully audited, compatible with BULLsEYE architecture, Simple Drop-In approach ready for Phase 1-7 execution

---

### Assessment Deliverables

| Deliverable | Owner | Due | Format |
|-------------|-------|-----|--------|
| Network Validation Spec Report | TBD | Before Phase 1 | Markdown in `docs/` or inline comments |
| License Engine Architecture Document | TBD | Before Phase 1 | Markdown in `docs/` or reference in sprint_2.md |
| Integration Mapping & Action Items | TBD | Before Phase 1 | Table in sprint_2.md with updated Phase 1 tasks |

---

### Assessment Done Criteria

**Phase 1 Pre-Flight Status: ✅ ALL GATES PASSED**

- ✅ BLOCKER 1 RESOLVED: Network validation spec is comprehensive (API config, endpoints, timeouts, OpenSSL strategies, ANMO compliance)
- ✅ BLOCKER 2 RESOLVED: License engine is header-only, zero compilation overhead, dual thread-safe API confirmed
- ✅ BLOCKER 3 RESOLVED: Drop-in kit fully compatible with BULLsEYE, Simple Drop-In approach selected, no build conflicts
- ✅ Integration mapping identifies NO conflicts with existing build system
- ✅ LicenseSSOT.h constants are documented and ready for Phase 1 task (see NETWORK_VALIDATION_REFERENCE.md section 1.1-1.4)
- ✅ CMakeLists.txt include strategy is confirmed (simple: add include path + source files, no build config changes needed)
- ✅ No critical architectural surprises discovered

---

## Assessment Summary

### Key Findings

1. **Portable License Drop-In Kit Quality: EXCELLENT** ✅
   - Comprehensive documentation (765-line network spec, 262-line integration guide, phase-by-phase plan already created)
   - Header-only implementation (zero compilation overhead)
   - Proven thread-safe dual API design (audio-atomic + UI-blocking)
   - All three integration approaches documented with examples

2. **BULLsEYE Integration Strategy: SIMPLE DROP-IN** ✅
   - Cleanest approach for BULLsEYE's own product identity
   - No AnalogMorphV333 source extraction needed
   - SSOT-friendly configuration
   - Minimal CMake changes (include paths only)

3. **Compliance Risk Assessment: LOW** ✅
   - ANMO macOS Laws 1-9: All covered by drop-in kit docs and NETWORK_VALIDATION_REFERENCE
   - Thread safety: Dual API design prevents audio thread blocking
   - Build system: Header-only, no dependency conflicts
   - Platform support: macOS (Keychain), Windows (Registry), Linux (file) all documented

4. **Estimated Effort Reaffirmed: Flexible Timeline** ✅
   - Phase 1 (Foundation): 1 day — CMake + LicenseSSOT.h
   - Phase 2 (Core): 2 days — PluginProcessor integration + atomic API
   - Phase 3 (UI): 2 days — LicenseStatusComponent + activation dialog
   - Phase 4 (macOS): 2 days — Deployment verification + codesign
   - Phase 5 (macOS Installer): TBD — installer work intentionally deferred
   - Phase 6 (Testing): 3 days — 20 new tests + DAW validation
   - Phase 7 (Documentation): 1 day — Changelogs, release tag

5. **Ready to Proceed: YES** ✅
   - All blockers resolved
   - No architectural surprises
   - Clear path forward with detailed guidance in kit docs
   - Existing 48 tests must continue passing (regression check)

### Action Items Before Phase 1 Execution

1. ✅ **Review Decision:** Simple Drop-In approach confirmed (no extraction, no DI complexity)
2. ✅ **Pre-Integration:** Read `portable-license-drop-in/LICENSE_DROP_IN_LAWS.md` (Laws 1-9, threading rules)
3. ✅ **API Reference:** Bookmark `portable-license-drop-in/core/LICENSE_ENGINE.hpp` (public API)
4. ✅ **Build Verification:** Ensure existing 48 tests pass baseline before Phase 1 changes
5. ⏳ **Phase 1 Start:** Create `Source/SSOT/LicenseSSOT.h` using NETWORK_VALIDATION_REFERENCE.md as source

---

## Risk Assessment

| Risk | Likelihood | Impact | Mitigation |
|------|-----------|--------|------------|
| Audio thread stall from blocking license call | Low | **Critical** | Only use `isLicensedAtomically()` in `processBlock`, code review enforced |
| OpenSSL dylib missing on user Mac | Medium | High | Bundle in `Frameworks/`, verify with `otool -L` |
| Absolute Homebrew path in binary | Medium | High | CMake `install_name_tool` post-build, LAW 9 verification |
| Codesign breaks with license engine | Low | High | License engine is header-only, test `codesign --deep` |
| License file corruption | Low | Medium | Graceful degradation to trial, `getLastKnownGoodState()` fallback |
| Existing tests fail due to license integration | Medium | Medium | Regression testing after each phase |
| Network validation complexity | Medium | Medium | Start with offline-only, add network later if needed |

---

## Success Criteria

### Phase Completion Criteria

**Phase 1 Complete When:**
- [ ] LicenseSSOT.h created with all constants
- [ ] CMakeLists.txt builds successfully with new includes
- [ ] No functional changes to plugin behavior

**Phase 2 Complete When:**
- [ ] License engine initialized in constructor
- [ ] `isLicensedAtomically()` gate in `processBlock`
- [ ] All audio thread safety tests pass
- [ ] Zero allocations in audio path (verified with profiler)

**Phase 3 Complete When:**
- [ ] LicenseStatusComponent displays trial/licensed status
- [ ] Activation dialog opens and accepts license key
- [ ] Status refreshes correctly at 1 Hz
- [ ] UI colors match design (trial=orange, licensed=green, expired=red)

**Phase 4 Complete When:**
- [ ] `otool -L` shows no absolute Homebrew paths
- [ ] `codesign --verify --deep --strict` passes
- [ ] Plugin loads on clean Mac (no Homebrew)
- [ ] Entitlements file configured (if network license)

**Phase 5 Complete When (TBD):**
- [ ] Trial version installer builds successfully
- [ ] Licensed version installer builds successfully
- [ ] Install/uninstall cycle tested and works
- [ ] Installer script documented

**Phase 6 Complete When:**
- [ ] All 20 new license tests pass
- [ ] All 48 existing tests still pass (0 regressions)
- [ ] DAW testing complete (REAPER, Logic, Ableton)
- [ ] Deployment verification script passes

**Phase 7 Complete When:**
- [ ] CHANGELOG.md updated for v2.1.0
- [ ] README.md includes licensing section
- [ ] LAW_BOOK.md includes Law 32
- [ ] Release tag created with metadata

### Sprint 5 Target Success Criteria

- [ ] License engine integrated without breaking existing functionality
- [ ] All 48 existing tests pass (100% regression-free)
- [ ] All 20 new license tests pass
- [ ] Trial mode: 14 days, full audio functionality, "TRIAL" watermark
- [ ] Licensed mode: License key activation, status persistence
- [ ] macOS installer created for both trial and licensed builds (TBD)
- [ ] macOS deployment compliant with ANMO Laws 1-9
- [ ] Documentation complete and up-to-date
- [ ] Windows/Linux cross-platform deployment remains future scope (TBD)

---

## File Change Summary

### New Files to Create

| File | Purpose | Phase |
|------|---------|-------|
| `Source/SSOT/LicenseSSOT.h` | License configuration constants | 1 |
| `Source/Components/LicenseStatusComponent.h` | License status UI component | 3 |
| `Source/Components/LicenseStatusComponent.cpp` | License status implementation | 3 |
| `tests/License/TestLicenseIntegration.cpp` | 20 new license test cases | 6 |
| `BULLsEYE.entitlements` | Hardened runtime entitlements (macOS) | 4 |
| `build/verify_license_integration.sh` | Deployment verification script | 4 |
| `docs/INSTALLER_GUIDE.md` | Installer usage documentation | 7 |

### Files to Modify

| File | Changes | Phase |
|------|---------|-------|
| `CMakeLists.txt` | Include paths, sources, OpenSSL config | 1 |
| `Source/PluginProcessor.h` | Add license engine member, accessors | 2 |
| `Source/PluginProcessor.cpp` | Initialize engine, processBlock gate | 2 |
| `Source/PluginEditor.h` | Add LicenseStatusComponent | 3 |
| `Source/PluginEditor.cpp` | Layout license status bar | 3 |
| `Source/SSOT/UISSOT.h` | Add license UI constants | 3 |
| `tests/CMakeLists.txt` | Add license test sources | 6 |
| `build.sh` | Add license build options | 4 |
| `CHANGELOG.md` | Add v2.1.0 entry | 7 |
| `README.md` | Add licensing section | 7 |
| `LAW_BOOK.md` | Add Law 32 | 7 |
| `REFERENCE.md` | Add LicenseSSOT constants | 7 |

---

## Architecture Overview

### License Engine Integration

```
┌─────────────────────────────────────────────────────────────┐
│                    BULLsEYE Plugin                           │
│                                                              │
│  ┌──────────────────────┐    ┌────────────────────────────┐ │
│  │  BULLsEYEProcessor   │    │    BULLsEYEEditor          │ │
│  │  (Audio Thread)       │    │    (UI Thread)             │ │
│  │                       │    │                            │ │
│  │  ┌─────────────────┐ │    │  ┌────────────────────┐    │ │
│  │  │ dspCore          │ │    │  │ CircularMeter      │    │ │
│  │  │ (BULLsEYEProc)  │ │    │  │ StatusDisplay      │    │ │
│  │  └─────────────────┘ │    │  │ ModeSelector       │    │ │
│  │                       │    │  │ ┌──────────────┐   │    │ │
│  │  ┌─────────────────┐ │    │  │ │LicenseStatus │   │    │ │
│  │  │ licenseEngine    │ │    │  │ │Component     │   │    │ │
│  │  │ .isLicensed      │◄├────┤  │ │(NEW)         │   │    │ │
│  │  │  Atomically()    │ │    │  │ └──────────────┘   │    │ │
│  │  └─────────────────┘ │    │  └────────────────────┘    │ │
│  └──────────────────────┘    └────────────────────────────┘ │
│                                                              │
│  ┌──────────────────────────────────────────────────────┐   │
│  │              SSOT Layer                                │   │
│  │  ModelSSOT.h  │  UISSOT.h  │  LicenseSSOT.h (NEW)   │   │
│  └──────────────────────────────────────────────────────┘   │
│                                                              │
│  ┌──────────────────────────────────────────────────────┐   │
│  │       portable-license-drop-in/core/                   │   │
│  │       LICENSE_ENGINE.hpp (header-only)                  │   │
│  └──────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────┘
```

### Thread Safety Model

```
AUDIO THREAD (processBlock)          UI THREAD (editor, timer)
━━━━━━━━━━━━━━━━━━━━━━━━━━          ━━━━━━━━━━━━━━━━━━━━━━━━━
licenseEngine                        licenseEngine
  .isLicensedAtomically()   ←atomic→   .isLicensed()
  .isFeatureEnabledAtomically()        .activateLicense()
  .getLastKnownGoodState()             .tryRefreshStatus()
                                       .getLicenseStatusMessage()
                                       .initialize()
```

---

## Network Validation Reference

### Overview

The network validation reference document (`portable-license-drop-in/NETWORK_VALIDATION_REFERENCE.md`) provides comprehensive guidance for implementing online license validation with OpenSSL integration.

### Key Reference Sections

| Section | Purpose | Relevance to BULLsEYE |
|---------|---------|---------------------|
| **1. License System Network Architecture** | API endpoints, configuration | Phase 1 (LicenseSSOT.h) |
| **2. OpenSSL Integration Reference** | JWT verification, machine ID hashing | Phase 1, Phase 4 |
| **3. Platform-Specific Configurations** | macOS/Windows/Linux OpenSSL setup | Phase 4 |
| **4. Network Request Implementation** | Async execution, retry logic | Phase 6 (testing) |
| **5. Security & Certificate Validation** | SSL/TLS, certificate validation | Phase 4 |
| **6. Troubleshooting & Diagnostics** | Debug logging, common issues | Phase 6 (testing) |
| **7. Compliance & Laws Reference** | ANMO laws, license system laws | Phase 4 |
| **8. Quick Reference Cards** | Endpoints, commands, values | All phases |

### API Configuration Constants

Based on `NETWORK_VALIDATION_REFERENCE.md` section 1.1:

```cpp
// In LicenseSSOT.h (to be created)
namespace LicenseSSOT::Network
{
    constexpr const char* API_BASE_URL = "https://byiqaudio.com/api/v1";
    constexpr int TIMEOUT_SECONDS = 30;
    constexpr int MAX_RETRIES = 3;
    constexpr const char* CONTENT_TYPE = "application/json";
    
    // Endpoints
    constexpr const char* ENDPOINT_GENERATE = "/plugins/licenses/generate";
    constexpr const char* ENDPOINT_VERIFY = "/plugins/licenses/verify";
    constexpr const char* ENDPOINT_DEACTIVATE = "/plugins/licenses/deactivate";
    constexpr const char* ENDPOINT_TRANSFER = "/plugins/licenses/transfer";
    constexpr const char* ENDPOINT_HEALTH = "/plugins/health";
}
```

### OpenSSL Platform Configuration

Based on `NETWORK_VALIDATION_REFERENCE.md` section 3:

**macOS (Dynamic Linking + Bundling):**
- Dylibs: `libssl.3.dylib`, `libcrypto.3.dylib`
- Auto-detect: `/opt/homebrew` (Apple Silicon) vs `/usr/local` (Intel)
- Bundling: Copy to `Contents/Frameworks/`, patch with `install_name_tool` to use `@rpath`
- Verification: `otool -L` shows no absolute Homebrew paths

**Windows (Static Linking):**
- Libraries: `libcrypto_static.lib`, `libssl_static.lib`
- Runtime: `/MT` (static CRT, zero external dependencies)
- Verification: `dumpbin /dependents` shows no DLL dependencies

**Linux (System Shared):**
- Libraries: `libssl.so.3`, `libcrypto.so.3`
- Packages: `libssl-dev`, `libcrypto++-dev`
- Verification: `ldd` shows shared library dependencies

### Security Requirements

Based on `NETWORK_VALIDATION_REFERENCE.md` section 5:

| Requirement | Implementation | Phase |
|-------------|---------------|-------|
| **SSL/TLS Configuration** | JUCE's system SSL/TLS stack via libcurl | Phase 4 |
| **Certificate Validation** | System certificate stores (Windows/macOS/Linux) | Phase 4 |
| **JWT Signature Verification** | RSA-SHA256 using OpenSSL | Phase 1, 2 |
| **Machine ID Hashing** | SHA256 fallback for backend compatibility | Phase 1 |
| **Code Signing** | EV certificate (Windows), Developer ID (macOS) | Phase 4 |

### Verification Commands

Based on `NETWORK_VALIDATION_REFERENCE.md` section 8.3:

**macOS:**
```bash
# Check OpenSSL version
openssl version

# Verify bundled dylibs
ls -la BULLsEYE.vst3/Contents/Frameworks/

# Check dylib dependencies (should show @rpath, not absolute paths)
otool -L BULLsEYE.vst3/Contents/MacOS/BULLsEYE | grep ssl

# Check code signature
codesign -dv BULLsEYE.vst3

# Validate bundle
./build/verify_license_integration.sh BULLsEYE.vst3
```

**Windows:**
```powershell
# Check OpenSSL installation
where openssl.exe
openssl version

# Check DLL dependencies (should show NO libssl.dll or libcrypto.dll)
dumpbin /dependents BULLsEYE.vst3\Contents\x86_64-win\BULLsEYE.vst3
```

**Linux:**
```bash
# Check OpenSSL version
openssl version

# Check shared libraries
ldd BULLsEYE.so | grep ssl
```

### Network Troubleshooting

Based on `NETWORK_VALIDATION_REFERENCE.md` section 6.2:

| Issue | Symptom | Solution |
|-------|----------|----------|
| **Connection timeout** | "Network error after 3 attempts" | Check firewall blocking port 443 |
| **SSL handshake failed** | "curl: (35) SSL certificate problem" | Install CA certificates |
| **DNS resolution failed** | "curl: (6) Could not resolve host" | Check DNS configuration |
| **Backend 500 error** | "Invalid server response" | Contact backend team |
| **Invalid license token** | "License verification failed" | Request new license |

### Debug Logging

Based on `NETWORK_VALIDATION_REFERENCE.md` section 6.1:

**Log Location:** `~/Library/Application Support/IQ/BULLsEYE/license_debug.log`

**View Logs:**
```bash
# View recent log entries
tail -f ~/Library/Application\ Support/IQ/BULLsEYE/license_debug.log

# Search for errors
grep -i error ~/Library/Application\ Support/IQ/BULLsEYE/license_debug.log
```

---

## ANMO macOS Deployment Laws Compliance

| Law | Requirement | BULLsEYE Compliance Action | Phase |
|-----|-------------|---------------------------|-------|
| **LAW 1: Bundle Integrity** | `.vst3`/`.component` are directory bundles | License data stored outside bundle (Application Support) | 4 |
| **LAW 2: Dylib Linking** | Dylibs in `Contents/Frameworks/` with `@rpath` | OpenSSL bundled if `BULLSEYE_ENABLE_NETWORK_LICENSE=ON` | 4 |
| **LAW 3: Architecture-Agnostic** | Auto-detect Homebrew paths | CMake conditional: `/opt/homebrew` vs `/usr/local` | 4 |
| **LAW 4: Clean System Testing** | Test without Homebrew | License engine uses Keychain (no Homebrew dependency) | 4 |
| **LAW 5: Silent Codesigning** | All builds must be signed | License engine has no anti-tamper that conflicts with `codesign --deep` | 4 |
| **LAW 6: Cross-Architecture** | Test arm64 + x86_64 | License engine uses `std::atomic` (portable), machine ID hashing is arch-independent | 4 |
| **LAW 7: Release Tag** | Tags must include license version | Tag format: `v2.1.0-macos-universal-licensed-feb2026` | 7 |
| **LAW 8: Notarization** | Hardened Runtime + notarization | License engine must not require disabled hardened runtime entitlements | 4 |
| **LAW 9: Rpath Management** | No absolute Homebrew paths in binary | `install_name_tool` patches applied in CMake post-build step | 4 |

---

## Testing Strategy

### Unit Tests (New)

| Test ID | Category | Description | Thread |
|---------|----------|-------------|--------|
| LIC-001 | Init | License engine initializes with LicenseSSOT config | UI |
| LIC-002 | Init | Graceful degradation on init failure → trial mode | UI |
| LIC-003 | Audio | `isLicensedAtomically()` returns within 1μs | Audio |
| LIC-004 | Audio | No allocation in `processBlock` license path | Audio |
| LIC-005 | Audio | Licensed: audio passes through unchanged | Audio |
| LIC-006 | Audio | Trial (mute=true): buffer cleared | Audio |
| LIC-007 | Audio | Trial (mute=false): audio passes through | Audio |
| LIC-008 | UI | `isLicensed()` matches `isLicensedAtomically()` | UI |
| LIC-009 | UI | Status message updates correctly | UI |
| LIC-010 | UI | Activation dialog flow works | UI |
| LIC-011 | State | License persists across DAW session reload | UI |
| LIC-012 | State | License persists across plugin reinstall | UI |
| LIC-013 | Platform | macOS Keychain read/write works | UI |
| LIC-014 | Platform | Windows Registry read/write works | UI |
| LIC-015 | Platform | Linux file-based storage works | UI |
| LIC-016 | Deploy | `otool -L` shows no absolute Homebrew paths | Build |
| LIC-017 | Deploy | `codesign --verify --deep --strict` passes | Build |
| LIC-018 | Deploy | Plugin loads on clean Mac (no Homebrew) | Manual |
| LIC-019 | Offline | Offline validation works for 30+ days | UI |
| LIC-020 | Grace | Grace period allows continued use for 7 days | UI |

### Integration Tests (Existing)

All 48 existing tests must continue to pass after license integration:

| Suite | Tests | Status | Priority |
|-------|-------|--------|----------|
| BULLsEYEProcessorCoreTest | 16 | Must pass | P0 |
| APVTSDSPBinding | 3 | Must pass | P0 |
| StateManagement | 4 | Must pass | P0 |
| ChannelConfiguration | 2 | Must pass | P0 |
| SampleRateConfiguration | 2 | Must pass | P0 |
| BufferSizeConfiguration | 1 | Must pass | P0 |
| ContentTypeIntegration | 2 | Must pass | P0 |
| LufsIntegration | 5 | Must pass | P0 |
| NormalizationTests | 3 | Must pass | P0 |
| DeviationTests | 2 | Must pass | P0 |
| EdgeCaseTests | 5 | Must pass | P0 |
| PerformanceTests | 3 | Must pass | P0 |

### DAW Testing Matrix

| DAW | macOS arm64 | macOS x86_64 | Status |
|-----|:-----------:|:------------:|:------:|
| REAPER | ○ | ○ | Required |
| Logic Pro | ○ | ○ | Required |
| Ableton Live | ○ | ○ | Required |

○ = Needs testing

---

## Future Work (Post-Sprint 5)

### v2.2.0 - Cross-Platform Deployment (TBD)

**Estimated:** TBD  
**Goals:**
- Windows VST3 build with license support (TBD)
- Linux LV2 build with license support (TBD)
- Installers for both platforms (TBD)
- Cross-platform testing (TBD)

### v2.3.0 - Advanced Features

**Estimated:** Sprint 7+  
**Potential Features:**
- Network license validation (OpenSSL integration)
- Subscription management UI
- License transfer functionality
- Feature-based licensing (e.g., advanced True Peak modes)

---

## Progress Tracking

### Phase 1: Foundation
- [ ] Create LicenseSSOT.h constants
- [ ] Update CMakeLists.txt include paths
- [ ] Add OpenSSL conditional build option
- [ ] Build verification (no functional changes)

### Phase 2: Core Engine
- [ ] Add license engine member to PluginProcessor
- [ ] Implement initializeLicenseEngine()
- [ ] Add isLicensedAtomically() gate in processBlock
- [ ] Add license accessor methods for UI
- [ ] Verify audio thread safety (no allocations)

### Phase 3: UI Components
- [ ] Create LicenseStatusComponent.h/cpp
- [ ] Add license status bar to PluginEditor
- [ ] Implement activation dialog
- [ ] Add license UI constants to UISSOT.h
- [ ] Timer-based status refresh (1 Hz)

### Phase 4: macOS Deployment
- [ ] Verify bundle structure integrity
- [ ] Create deployment verification script
- [ ] Test otool -L (no absolute paths)
- [ ] Configure codesigning for license builds
- [ ] Create entitlements file (if network license)
- [ ] Clean Mac testing (no Homebrew)

### Phase 5: macOS Installer (TBD)
- [ ] [TBD] Design installer requirements
- [ ] [TBD] Create `pkgbuild`/`productbuild` spec
- [ ] [TBD] Build trial version installer
- [ ] [TBD] Build licensed version installer
- [ ] [TBD] Create uninstaller script
- [ ] [TBD] Test install/uninstall cycle

### Phase 6: Testing
- [ ] Create 20 new license test cases
- [ ] Implement audio thread safety tests
- [ ] Implement UI license tests
- [ ] Implement state persistence tests
- [ ] Verify all 48 existing tests still pass
- [ ] DAW testing (REAPER, Logic, Ableton)
- [ ] Deployment verification script run

### Phase 7: Documentation
- [ ] Update CHANGELOG.md for v2.1.0
- [ ] Update README.md with licensing section
- [ ] Add Law 32 to LAW_BOOK.md
- [ ] Create INSTALLER_GUIDE.md
- [ ] Update REFERENCE.md with LicenseSSOT
- [ ] Create release tag with metadata

---

## Notes

### Trial Configuration Details
- **Duration:** 14 days (configurable in `LicenseSSOT::TRIAL_DAYS`)
- **Audio Muting:** Disabled (`TRIAL_MUTE_AUDIO = false`)
- **Watermark:** Enabled (`TRIAL_SHOW_WATERMARK = true`)
- **Grace Period:** 7 days after subscription expiration
- **Offline Validation:** 30+ days support

### Installer Strategy
- Both trial and licensed versions included in installer
- User can activate license from trial to licensed
- Separate build artifacts for trial vs licensed
- Uninstaller removes all plugin bundles and files

### Staging Phase Notes
- No Apple Developer ID required yet
- Code signing for testing purposes only (ad-hoc)
- Notarization not needed in staging
- Public release deferred to future phase

### Agent Guidance
- See `docs/AGENT_MENTAL_MODEL.md` for the contract-in-contract execution model and agent brief.

---

*Last Updated: 2026-02-07*
*Next Update: Phase 1 completion*