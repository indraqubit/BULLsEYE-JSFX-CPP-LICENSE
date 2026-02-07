# BULLsEYE Sprint 5 Tracker

**License Integration Assessment & Implementation**

---

## 🎯 SPRINT 5 - LICENSE INTEGRATION

| Field | Value |
|-------|-------|
| **Sprint** | 5 |
| **Start Date** | 2026-02-07 (planned) |
| **End Date** | TBD |
| **Goal** | Integrate Portable License Drop-In Kit, create OSX installer with trial + licensed builds |
| **Status** | 📋 PLANNING |
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
3. Create OSX installer with both trial and licensed builds
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

### Phase 5: OSX Installer Creation

| Task | Status | Priority | Estimate | Dependencies |
|------|--------|----------|----------|--------------|
| Design installer requirements | ⏳ TODO | P0 | 0.5d | None |
| Create PKGBUILD or PackageMaker spec | ⏳ TODO | P0 | 1.5d | Requirements |
| Build trial version installer | ⏳ TODO | P0 | 1.0d | Phase 4 |
| Build licensed version installer | ⏳ TODO | P0 | 1.0d | Phase 4 |
| Create uninstaller script | ⏳ TODO | P1 | 0.5d | Installer spec |
| Test install/uninstall cycle | ⏳ TODO | P0 | 0.5d | All installers built |

**Milestone:** OSX installer with trial + licensed builds ready

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
- Days 9-10: Phase 5 (OSX Installer)
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

| Blocker | Impact | Resolution Plan |
|---------|--------|-----------------|
| Network validation spec | Phase 1 OpenSSL config | User to provide reference |
| None currently | - | - |

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
- [x] LicenseSSOT.h created with all constants
- [x] CMakeLists.txt builds successfully with new includes
- [x] No functional changes to plugin behavior

**Phase 2 Complete When:**
- [x] License engine initialized in constructor
- [x] `isLicensedAtomically()` gate in `processBlock`
- [x] All audio thread safety tests pass
- [x] Zero allocations in audio path (verified with profiler)

**Phase 3 Complete When:**
- [x] LicenseStatusComponent displays trial/licensed status
- [x] Activation dialog opens and accepts license key
- [x] Status refreshes correctly at 1 Hz
- [x] UI colors match design (trial=orange, licensed=green, expired=red)

**Phase 4 Complete When:**
- [x] `otool -L` shows no absolute Homebrew paths
- [x] `codesign --verify --deep --strict` passes
- [x] Plugin loads on clean Mac (no Homebrew)
- [x] Entitlements file configured (if network license)

**Phase 5 Complete When:**
- [x] Trial version installer builds successfully
- [x] Licensed version installer builds successfully
- [x] Install/uninstall cycle tested and works
- [x] Installer script documented

**Phase 6 Complete When:**
- [x] All 20 new license tests pass
- [x] All 48 existing tests still pass (0 regressions)
- [x] DAW testing complete (REAPER, Logic, Ableton)
- [x] Deployment verification script passes

**Phase 7 Complete When:**
- [x] CHANGELOG.md updated for v2.1.0
- [x] README.md includes licensing section
- [x] LAW_BOOK.md includes Law 32
- [x] Release tag created with metadata

### Sprint 5 Success Criteria

- ✅ License engine integrated without breaking existing functionality
- ✅ All 48 existing tests pass (100% regression-free)
- ✅ All 20 new license tests pass
- ✅ Trial mode: 14 days, full audio functionality, "TRIAL" watermark
- ✅ Licensed mode: License key activation, status persistence
- ✅ OSX installer created for both trial and licensed builds
- ✅ macOS deployment compliant with ANMO Laws 1-9
- ✅ Documentation complete and up-to-date
- ✅ Ready for cross-platform deployment (Windows/Linux) in future sprint

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

### v2.2.0 - Cross-Platform Deployment

**Estimated:** Sprint 6  
**Goals:**
- Windows VST3 build with license support
- Linux LV2 build with license support
- Installers for both platforms
- Cross-platform testing

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

### Phase 5: OSX Installer
- [ ] Design installer requirements
- [ ] Create PKGBUILD or PackageMaker spec
- [ ] Build trial version installer
- [ ] Build licensed version installer
- [ ] Create uninstaller script
- [ ] Test install/uninstall cycle

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

---

*Last Updated: 2026-02-07*
*Next Update: Phase 1 completion*