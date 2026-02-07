# Sprint 4 Agent Playbook - Review Fixes

**Document Purpose:** Document blindspots, blockers, and remediation for SPRINT_4_AGENT_PLAYBOOK.md  
**Date:** February 7, 2026  
**Status:** Remediation Plan

---

## Executive Summary

SPRINT_4_AGENT_PLAYBOOK.md was created but contains critical blockers that must be resolved before Sprint 4 execution can begin. This document identifies all issues and provides specific fixes.

**Summary:**
- **3 P0 Blockers** - Must fix before Sprint 4 starts
- **7 P1 Issues** - Should address before Sprint 4 starts
- **Remediation Time:** 1-2 hours to fix all issues

---

## 🔴 P0 Blockers (Must Fix First)

### Blocker 1: build.sh Missing Architecture Options

**Issue:**
- Current build.sh has no ARCH parameter
- No architecture selection (universal/x86_64/arm64)
- No architecture verification output
- Cannot support multi-architecture builds

**Current State:**
```bash
# build.sh has no architecture parameter handling
cmake .. -DCMAKE_BUILD_TYPE=Release
```

**Required Fix:**
Add architecture parameter selection and verification to build.sh

**Code Changes:**
```bash
# Add after JUCE check section

# Architecture selection
ARCH=${1:-"universal"}

case "$ARCH" in
    universal)
        ARCHS="x86_64;arm64"
        echo "Building universal binary (Intel + Apple Silicon)"
        ;;
    x86_64)
        ARCHS="x86_64"
        echo "Build for Intel (x86_64) only"
        ;;
    arm64)
        ARCHS="arm64"
        echo "Build for Apple Silicon (ARM64) only"
        ;;
    *)
        echo "Usage: $0 [universal|x86_64|arm64]"
        exit 1
        ;;
esac

# Update CMake configuration
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DCMAKE_OSX_ARCHITECTURES="$ARCHS" \
         -DCMAKE_OSX_DEPLOYMENT_TARGET="12.0"

# Add after build artifacts section
echo ""
echo "Binary architecture:"
file BULLsEYE_artefacts/VST3/*/BULLsEYE 2>/dev/null || echo "  Binary not found"
```

**Verification:**
```bash
./build.sh universal
./build.sh x86_64
./build.sh arm64
```

**Files to Modify:**
- `build.sh` - Add architecture selection and verification

---

### Blocker 2: Info.plist.in Missing

**Issue:**
- `build/macOS/` directory is empty
- CMakeLists.txt references `${CMAKE_CURRENT_SOURCE_DIR}/build/macOS/Info.plist.in`
- Build will fail at CMake configuration step
- Bundle metadata cannot be configured

**Current State:**
```bash
$ ls -la build/macOS/
ls: build/macOS/: No such file or directory
```

**Required Fix:**
Create build/macOS directory and Info.plist.in with proper bundle configuration

**Code Changes:**
Create `build/macOS/Info.plist.in`:
```xml
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleName</key>
    <string>${PRODUCT_NAME}</string>
    
    <key>CFBundleDisplayName</key>
    <string>${PRODUCT_NAME}</string>
    
    <key>CFBundleIdentifier</key>
    <string>${BUNDLE_ID}</string>
    
    <key>CFBundleVersion</key>
    <string>${VERSION}</string>
    
    <key>CFBundleShortVersionString</key>
    <string>${VERSION}</string>
    
    <key>CFBundlePackageType</key>
    <string>BNDL</string>
    
    <key>CFBundleSignature</key>
    <string>${PLUGIN_MANUFACTURER_CODE}</string>
    
    <key>NSHumanReadableCopyright</key>
    <string>Copyright © 2025 IQ. All rights reserved.</string>
    
    <key>LSMinimumSystemVersion</key>
    <string>12.0</string>
    
    <key>NSHighResolutionCapable</key>
    <true/>
    
    <key>NSRequiresAquaSystemAppearance</key>
    <false/>
</dict>
</plist>
```

**Verification:**
```bash
mkdir -p build/macOS
# Create Info.plist.in with content above
cmake ..  # Should configure successfully without errors
```

**Files to Create:**
- `build/macOS/Info.plist.in` - Bundle configuration file

**Directories to Create:**
- `build/macOS/` - Directory for macOS-specific build files

---

### Blocker 3: macos-build.yml Missing

**Issue:**
- `.github/workflows/` only contains linux-build.yml and windows-build.yml
- No CI/CD workflow for macOS builds
- Cannot automate universal binary builds
- Cannot verify architectures in CI/CD

**Current State:**
```bash
$ ls -la .github/workflows/
linux-build.yml
windows-build.yml
# No macos-build.yml
```

**Required Fix:**
Create `.github/workflows/macos-build.yml` with universal binary build support

**Code Changes:**
Create `.github/workflows/macos-build.yml`:
```yaml
name: macOS Universal Build

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]
  release:
    types: [ created ]

jobs:
  build-macos:
    runs-on: macos-latest
    
    steps:
    - name: Checkout repository
      uses: actions/checkout@v3
      with:
        submodules: recursive
    
    - name: Install dependencies
      run: |
        brew install cmake
    
    - name: Configure CMake (Universal)
      run: |
        mkdir build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release \
                 -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64" \
                 -DCMAKE_OSX_DEPLOYMENT_TARGET="12.0"
    
    - name: Build
      run: |
        cd build
        cmake --build . --config Release -j$(sysctl -n hw.ncpu)
    
    - name: Verify universal binary
      run: |
        lipo -info build/BULLsEYE_artefacts/VST3/*/BULLsEYE
    
    - name: Upload artifacts (Release only)
      if: github.event_name == 'release'
      uses: actions/upload-artifact@v3
      with:
        name: macOS-Universal
        path: |
          build/BULLsEYE_artefacts/VST3/
          build/BULLsEYE_artefacts/AU/
```

**Verification:**
```bash
# Push to GitHub
git add .github/workflows/macos-build.yml
git commit -m "Add macOS universal build workflow"
git push

# Check GitHub Actions tab - workflow should run
```

**Files to Create:**
- `.github/workflows/macos-build.yml` - CI/CD workflow for macOS

---

## 🟠 P1 High Priority Issues

### Issue 1: CMakeLists.txt Needs Universal Binary Config

**Issue:**
- Missing CMAKE_OSX_ARCHITECTURES configuration
- Missing CMAKE_OSX_DEPLOYMENT_TARGET configuration
- No architecture selection conditional logic
- Has macOS section but lacks universal binary support

**Current State:**
```cmake
# macOS-specific settings
if(APPLE)
    set_target_properties(BULLsEYE PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/build/macOS/Info.plist.in"
    )
endif()
```

**Required Fix:**
Add universal binary configuration to CMakeLists.txt

**Code Changes:**
```cmake
# macOS-specific settings
if(APPLE)
    # Universal binary support (Intel x86_64 + Apple Silicon ARM64)
    if(NOT DEFINED CMAKE_OSX_ARCHITECTURES)
        set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Target architectures")
    endif()
    
    # Set minimum macOS version (Monterey 12.0 through Tahoe 15.0)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0" CACHE STRING "Minimum macOS version")
    
    set_target_properties(BULLsEYE PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/build/macOS/Info.plist.in"
        MACOSX_BUNDLE_INFO_STRING "LUFS-I Gated Loudness Meter with True Peak Detection"
    )
    
    message(STATUS "Building for macOS ${CMAKE_OSX_DEPLOYMENT_TARGET}+ (${CMAKE_OSX_ARCHITECTURES})")
endif()
```

**Verification:**
```bash
mkdir build && cd build
cmake .. -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64" -DCMAKE_OSX_DEPLOYMENT_TARGET="12.0"
# Expected: "Building for macOS 12.0+ (x86_64;arm64)"
```

**Files to Modify:**
- `CMakeLists.txt` - Add universal binary configuration

---

### Issue 2: Phase 6 Timeline Contradiction

**Issue:**
- Playbook claims Phase 6 (Testing) takes 30 minutes
- Reality: 24+ test scenarios across multiple configurations
- Actual time: 2-3+ hours (which playbook also states)
- Timeline contradiction causes confusion

**Current State:**
```
Phase 6 (Testing) - Estimated Time: 30 minutes
```
But also:
```
**Testing Coordinator**
**Estimated Time:** 2-3 hours (requires access to different Macs)
```

**Required Fix:**
Clarify timeline in SPRINT_4_AGENT_PLAYBOOK.md

**Fix:**
Remove the "30 minutes" claim from Phase 6, keep only "2-3 hours"

**Code Changes:**
In SPRINT_4_AGENT_PLAYBOOK.md, Phase 6 section:
```
**Phase:** 6 (Testing)
**Agent:** Agent 4 (Testing Coordinator)
**Estimated Time:** 2-3 hours (requires access to different Macs and multiple DAWs)
```

**Rationale:**
- 3 architectures (universal, x86_64, arm64)
- 4 macOS versions (12.0, 13.0, 14.0, 15.0)
- 3 DAWs (REAPER, Logic, Ableton)
- 5 functionality tests per configuration
- Minimum: 3 × 4 × 3 = 36 scenarios, ~2 hours realistic

**Files to Modify:**
- `SPRINT_4_AGENT_PLAYBOOK.md` - Fix Phase 6 timeline

---

### Issue 3: Agent Infrastructure Unverified

**Issue:**
- Playbook defines 6 specialized agents
- No verification that agents exist or are available
- Assumes agent infrastructure that may not exist
- Risk: Cannot execute as designed

**Current State:**
Playbook assumes:
- Agent 1: CMake & Build Specialist
- Agent 2: CI/CD & Bundle Specialist
- Agent 3: Documentation Specialist
- Agent 4: Testing Coordinator
- Agent 5: Law Enforcement Auditor
- Agent 6: Test Execution Engine

**Required Fix:**
Add agent verification section to playbook or document that agents are conceptual roles

**Code Changes:**
Add to SPRINT_4_AGENT_PLAYBOOK.md, after Agent Roles section:

```markdown
### Agent Availability Verification

**Status:** Conceptual Design (Requires Verification)

Before Sprint 4 execution, verify:
- [ ] Are 6 independent agents available?
- [ ] Can agents communicate with each other?
- [ ] Is agent handoff protocol implemented?
- [ ] Are audit/test gate mechanisms functional?

**If Agents Are Not Available:**
The playbook can still be used as a guide for sequential execution:
1. Execute Phase 1-2 (CMake + Build) as single developer
2. Execute Phase 3-4 (CI/CD + Bundle) as single developer
3. Execute Phase 5 (Documentation) as single developer
4. Execute Phase 6 (Testing) as single developer
5. Perform law audits after each phase
6. Run test suite after each phase

**Agent Simulation:**
If agents are not available but playbook structure is desired:
- Use Cline CLI tool to spawn sub-agents: `cline "implement Phase 1: CMake Configuration"`
- Track phase completion with manual checklist
- Perform audits and tests manually
- Proceed sequentially through all 6 phases
```

**Verification:**
```bash
# Check if agent infrastructure exists
# (This is project-specific - no standard command)
```

**Files to Modify:**
- `SPRINT_4_AGENT_PLAYBOOK.md` - Add agent verification section

---

### Issue 4: JUCE Submodule Check Too Weak

**Issue:**
- Current check: `[ ! -d "modules/JUCE" ]` only checks directory existence
- Problem: Doesn't verify submodule is initialized
- Risk: Build fails due to uninitialized submodule

**Current State:**
```bash
if [ ! -d "modules/JUCE" ]; then
    echo "ERROR: JUCE not found!"
fi
```

**Required Fix:**
Improve JUCE verification in build.sh

**Code Changes:**
Replace JUCE check in build.sh:

```bash
# Check for JUCE
if [ ! -d "modules/JUCE" ]; then
    echo ""
    echo "ERROR: JUCE not found!"
    echo ""
    echo "Please set up JUCE before building:"
    echo ""
    echo "Option 1: Clone JUCE submodule"
    echo "  git submodule add https://github.com/juce-framework/JUCE.git modules/JUCE"
    echo "  git submodule update --init --recursive"
    echo ""
    echo "Option 2: Symlink to existing JUCE installation"
    echo "  ln -s /path/to/JUCE modules/JUCE"
    echo ""
    exit 1
fi

# Verify JUCE submodule is initialized
if [ -d "modules/JUCE/.git" ]; then
    # It's a git submodule - check if it's initialized
    if git submodule status modules/JUCE 2>/dev/null | grep -q "^-"; then
        echo ""
        echo "ERROR: JUCE submodule exists but is not initialized!"
        echo ""
        echo "Please initialize JUCE submodule:"
        echo "  git submodule update --init --recursive"
        echo ""
        exit 1
    fi
fi
```

**Verification:**
```bash
# Test with uninitialized submodule
rm -rf modules/JUCE
git clone https://github.com/juce-framework/JUCE.git modules/JUCE
./build.sh
# Should pass JUCE check

# Test with initialized submodule
git submodule deinit -f modules/JUCE
./build.sh
# Should fail with "not initialized" error
```

**Files to Modify:**
- `build.sh` - Improve JUCE submodule verification

---

### Issue 5: Code Signing/Notarization Unclear

**Issue:**
- CMakeLists.txt has commented signing code
- Playbook mentions "if enabled" but unclear if it should be enabled
- NOTARIZATION.md exists but connection unclear
- Need: Clear decision on signing requirement

**Current State:**
CMakeLists.txt has:
```cmake
# Example: Add code signing as a post-build step (disabled by default)
# Uncomment and modify the following to enable CMake-based signing:
#
# if(APPLE AND DEFINED CMAKE_SIGNING_CERT)
```

Playbook Phase 6 mentions:
```
8. **Verify code signing (if enabled)**
   - Test signed universal binary loading
```

**Required Fix:**
Document code signing decision in PLAYBOOK_REVIEW_FIXES.md

**Decision:**
**For Sprint 4: Code signing should remain DISABLED by default.**

**Rationale:**
1. Sprint 4 focuses on universal binary build support, not distribution
2. Code signing adds complexity and requires Apple Developer account
3. Unsigned plugins work fine for local DAW testing
4. Can enable signing in future sprint focused on distribution

**Action Items:**
1. Keep code signing commented in CMakeLists.txt (as-is)
2. Update Phase 6 playbook to explicitly state: "Code signing is NOT required for Sprint 4"
3. Add note: "Enable code signing in Sprint 5 (Distribution/Release)"
4. Document signing procedure in NOTARIZATION.md (already exists)

**Verification:**
```bash
# Build without signing (current state should work)
./build.sh universal

# Plugin should load in DAW without Gatekeeper warnings
# (Gatekeeper only blocks downloaded apps, not locally built plugins)
```

**Files to Modify:**
- `SPRINT_4_AGENT_PLAYBOOK.md` - Clarify code signing not required for Sprint 4
- `NOTARIZATION.md` - Reference for future signing (already exists)

---

### Issue 6: No Rollback Plan

**Issue:**
- If Phase 1-2 breaks build, no documented rollback procedure
- If CI/CD fails, no documented recovery steps
- Risk: Team could be stuck with broken build

**Current State:**
No rollback procedures documented

**Required Fix:**
Add rollback procedures to SPRINT_4_AGENT_PLAYBOOK.md

**Code Changes:**
Add new section to SPRINT_4_AGENT_PLAYBOOK.md:

```markdown
## Appendix B: Rollback Procedures

### Rollback Triggers

Rollback should be triggered if:
- Build fails after Phase 1 or Phase 2 changes
- CI/CD workflow fails repeatedly
- Tests fail and cannot be resolved within 2 attempts
- Law audit flags CRITICAL violations that cannot be fixed

### Phase-by-Phase Rollback

**Rollback Phase 1 (CMake Configuration):**
```bash
# Discard CMakeLists.txt changes
git checkout -- CMakeLists.txt

# Clean build directory
rm -rf build/
mkdir build
cd build

# Reconfigure with previous version
cmake .. -DCMAKE_BUILD_TYPE=Release
```

**Rollback Phase 2 (Build Script):**
```bash
# Discard build.sh changes
git checkout -- build.sh

# Clean build directory
rm -rf build/

# Test previous build script
./build.sh
```

**Rollback Phase 3 (CI/CD):**
```bash
# Remove CI/CD workflow
rm .github/workflows/macos-build.yml

# Commit removal
git add .github/workflows/macos-build.yml
git commit -m "Rollback: Remove macOS CI/CD workflow"
git push
```

**Rollback Phase 4 (Bundle Configuration):**
```bash
# Discard Info.plist.in changes
git checkout -- build/macOS/Info.plist.in

# Clean and rebuild
rm -rf build/
./build.sh
```

**Rollback Phase 5 (Documentation):**
```bash
# Discard documentation changes
git checkout -- QUICKSTART.md
git checkout -- README.md
git rm docs/MACOS_BUILD.md
git commit -m "Rollback: Remove Sprint 4 documentation"
```

**Rollback Phase 6 (Testing):**
```bash
# No rollback needed - testing is validation, not changes
# If tests fail, fix code instead of rolling back
```

### Complete Sprint Rollback

If entire Sprint 4 needs to be rolled back:

```bash
# Discard all Sprint 4 changes
git checkout HEAD -- CMakeLists.txt build.sh

# Remove new files
rm -f .github/workflows/macos-build.yml
rm -f build/macOS/Info.plist.in
rm -f docs/MACOS_BUILD.md

# Discard documentation changes
git checkout HEAD -- QUICKSTART.md README.md

# Clean build directory
rm -rf build/

# Rollback commit
git reset --hard HEAD~1
git push origin HEAD --force
```

### Emergency Rollback Procedure

If build is completely broken and cannot be fixed:

1. **Stop all work immediately**
2. **Notify team** of rollback
3. **Execute complete Sprint rollback** (see above)
4. **Verify previous version works**:
   ```bash
   git checkout main
   ./build.sh
   ```
5. **Document what broke** in sprint review notes
6. **Plan fix for next sprint**

### Rollback Decision Matrix

| Severity | Rollback Required? | Action |
|----------|-------------------|--------|
| Build broken, no fix in 30 min | ✅ Yes | Rollback affected phase |
| Tests fail, no fix in 2 attempts | ✅ Yes | Rollback affected phase |
| CRITICAL law violation | ✅ Yes | Rollback entire sprint |
| MAJOR law violation | ⚠️ Maybe | Fix or rollback |
| MINOR law violation | ❌ No | Document, continue |
| Documentation error | ❌ No | Fix in next commit |
```
```

**Verification:**
```bash
# Test rollback procedure on test branch
git checkout -b test-rollback
# Make changes, then rollback
git checkout -- CMakeLists.txt
# Verify previous version works
```

**Files to Modify:**
- `SPRINT_4_AGENT_PLAYBOOK.md` - Add rollback procedures appendix

---

### Issue 7: Handoff Criteria Vague

**Issue:**
- Playbook has handoff format but success metrics could be more specific
- Missing: Clear "DONE" criteria beyond checklist checkboxes
- Risk: Subjective phase completion

**Current State:**
Handoff format requires:
- Work completed (list)
- Files modified (list)
- Success criteria met (checkboxes)
- Audit status (PASS/FAIL)
- Test status (PASS/FAIL)
- Artifacts (list)
- Next phase

**Required Fix:**
Add objective verification criteria to handoff format

**Code Changes:**
Update handoff format in SPRINT_4_AGENT_PLAYBOOK.md:

```markdown
### Agent Handoff Format

When an agent completes a phase, they must provide:

**Required Information:**

## Phase X Handoff

**Phase:** X (Phase Name)
**Agent:** [Agent Name]
**Date:** [Date]
**Status:** ✅ COMPLETE

### Work Completed

1. [Task 1 description] - ✅ DONE
2. [Task 2 description] - ✅ DONE
3. [Task 3 description] - ✅ DONE

### Files Modified

- [File 1] - [Brief description of changes]
- [File 2] - [Brief description of changes]

### Success Criteria Met

✅ [Success criterion 1]
✅ [Success criterion 2]
✅ [Success criterion 3]

### Objective Verification

**Build Status:**
```bash
[Copy actual build command output]
```

**Build Output:**
```bash
[Copy actual build output]
```

**Verification Commands:**
```bash
[Copy actual verification command outputs]
```

**Audit Status**

**Auditor:** Agent 5
**Result:** PASS/FAIL
**Violations:** None / [List violations]
**Compliance Matrix:** [X/31 laws compliant]

**Test Status**

**Tester:** Agent 6
**Result:** PASS/FAIL
**Tests Passed:** 48/48
**Test Output:**
```bash
[Copy actual test output]
```

### Artifacts

- [Artifact 1 path] - [Description]
- [Artifact 2 path] - [Description]
- [Artifact 3 path] - [Description]

### Next Phase

**Phase:** [Next Phase Number] (Next Phase Name)
**Agent:** [Next Agent Name]
**Handoff Date:** [Date]

**Blocking Issues:** None / [List any blocking issues]
**Risks:** None / [List any risks]
**Recommendations:** [Any recommendations for next phase]
```

**Examples:**

**Phase 1 Handoff Example:**
```markdown
### Objective Verification

**Build Status:**
```bash
$ cmake .. -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64" -DCMAKE_OSX_DEPLOYMENT_TARGET="12.0"
-- The C compiler identification is AppleClang 15.0.0.15000040
-- The CXX compiler identification is AppleClang 15.0.0.15000040
-- Building for macOS 12.0+ (x86_64;arm64)
-- Configuring done
```

**Verification Commands:**
```bash
$ grep CMAKE_OSX_ARCHITECTURES build/CMakeCache.txt
CMAKE_OSX_ARCHITECTURES:STRING=x86_64;arm64

$ grep CMAKE_OSX_DEPLOYMENT_TARGET build/CMakeCache.txt
CMAKE_OSX_DEPLOYMENT_TARGET:STRING=12.0
```
```

**Phase 2 Handoff Example:**
```markdown
### Objective Verification

**Build Status:**
```bash
$ ./build.sh universal
Building universal binary (Intel + Apple Silicon)
Building...
Build complete!
```

**Verification Commands:**
```bash
$ lipo -info ~/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3/Contents/x86_64/BULLsEYE
Architectures in the fat file: x86_64 arm64 are: x86_64 arm64
```
```

**Files to Modify:**
- `SPRINT_4_AGENT_PLAYBOOK.md` - Add objective verification to handoff format

---

## Remediation Priority

### Immediate Actions (Do First)

1. **Fix P0 Blocker 1:** Update build.sh with architecture options (10 minutes)
2. **Fix P0 Blocker 2:** Create build/macOS/Info.plist.in (10 minutes)
3. **Fix P0 Blocker 3:** Create .github/workflows/macos-build.yml (15 minutes)
4. **Fix P1 Issue 1:** Update CMakeLists.txt with universal binary config (15 minutes)

**Total Time:** 50 minutes

### Secondary Actions (Do Soon)

5. **Fix P1 Issue 2:** Clarify Phase 6 timeline in playbook (5 minutes)
6. **Fix P1 Issue 3:** Add agent verification section to playbook (10 minutes)
7. **Fix P1 Issue 4:** Improve JUCE submodule check in build.sh (10 minutes)
8. **Fix P1 Issue 5:** Document code signing decision (5 minutes)
9. **Fix P1 Issue 6:** Add rollback procedures to playbook (20 minutes)
10. **Fix P1 Issue 7:** Add objective verification to handoff format (15 minutes)

**Total Time:** 65 minutes

### Total Remediation Time: ~2 hours

---

## Verification Checklist

After fixing all issues, verify:

### P0 Blockers
- [ ] build.sh has ARCH parameter (universal/x86_64/arm64)
- [ ] build.sh architecture selection works
- [ ] build.sh verification output works
- [ ] build/macOS/Info.plist.in exists
- [ ] build/macOS/Info.plist.in has correct content
- [ ] CMake configures with Info.plist.in
- [ ] .github/workflows/macos-build.yml exists
- [ ] GitHub Actions workflow runs successfully

### P1 Issues
- [ ] CMakeLists.txt has CMAKE_OSX_ARCHITECTURES
- [ ] CMakeLists.txt has CMAKE_OSX_DEPLOYMENT_TARGET
- [ ] Phase 6 timeline is realistic (2-3 hours)
- [ ] Agent verification section added to playbook
- [ ] JUCE submodule check improved
- [ ] Code signing decision documented
- [ ] Rollback procedures added to playbook
- [ ] Handoff format has objective verification

### End-to-End Test
- [ ] ./build.sh universal builds successfully
- [ ] ./build.sh x86_64 builds successfully
- [ ] ./build.sh arm64 builds successfully
- [ ] lipo -info verifies correct architectures
- [ ] CI/CD workflow runs on push
- [ ] All tests pass (48/48)

---

## Implementation Order

### Step 1: Fix P0 Blockers (50 minutes)
1. Update build.sh with architecture options
2. Create build/macOS/Info.plist.in
3. Create .github/workflows/macos-build.yml
4. Update CMakeLists.txt with universal binary config

### Step 2: Fix P1 Issues (65 minutes)
5. Clarify Phase 6 timeline
6. Add agent verification section
7. Improve JUCE submodule check
8. Document code signing decision
9. Add rollback procedures
10. Add objective verification to handoff format

### Step 3: Verify All Fixes (30 minutes)
11. Test all P0 blocker fixes
12. Test all P1 issue fixes
13. End-to-end build test
14. Create final verification report

### Step 4: Document Completion (10 minutes)
15. Update SPRINT.md with remediation status
16. Update STATUS.md with playbook readiness

**Total Estimated Time: 2 hours 35 minutes**

---

## Next Steps

### If You Want to Proceed:

1. **Review this document** - Ensure all fixes are understood
2. **Decide on implementation approach:**
   - Option A: Implement all fixes sequentially (2+ hours)
   - Option B: Fix P0 blockers only (50 minutes), defer P1 to later
   - Option C: Start Sprint 4 with known issues (not recommended)

3. **Execute fixes** - I can implement all fixes automatically
4. **Verify implementation** - Test each fix
5. **Update documentation** - Mark playbook as ready

### Recommendation:

**Implement all fixes before Sprint 4 execution.** The playbook is well-designed but has critical blockers that will prevent successful Sprint 4 implementation. Investing 2+ hours now will save 4+ hours of debugging later.

---

**Document Version:** 1.0  
**Last Updated:** February 7, 2026  
**Status:** Remediation Plan Complete - Ready for Implementation