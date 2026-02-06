# Migration Plan

**Roadmap for migrating JSFX plugin to JUCE**

---

## Plugin Information

| Field | Value |
|-------|-------|
| **Source Plugin** | YourJSFXPlugin.jsfx |
| **Target Plugin** | YourJUCEPlugin |
| **JSFX Author** | Author Name |
| **Migration Start** | YYYY-MM-DD |
| **Target Completion** | YYYY-MM-DD |

---

## Migration Phases

### Phase 1: Foundation

| Task | Status | Notes |
|------|--------|-------|
| Set up project structure | [ ] | |
| Create SSOT files | [ ] | ModelSSOT, UISSOT, DSPSSOT, ProcessorSSOT |
| Configure CMake | [ ] | Plugin name, company, formats |
| Set up JUCE submodule | [ ] | |
| Create basic processor skeleton | [ ] | |

**Milestone:** Project builds successfully

---

### Phase 2: DSP Core

| Task | Status | Notes |
|------|--------|-------|
| Implement TETRIS-compliant processor | [ ] | |
| Migrate mode logic | [ ] | |
| Implement parameter handling | [ ] | APVTS setup |
| Implement audio processing | [ ] | |
| Test DSP accuracy | [ ] | Compare with JSFX |

**Milestone:** Audio output matches JSFX

---

### Phase 3: UI Implementation

| Task | Status | Notes |
|------|--------|-------|
| Create PluginEditor base | [ ] | |
| Create UI components | [ ] | |
| Connect components to APVTS | [ ] | |
| Implement visual feedback | [ ] | |
| Polish UI design | [ ] | |

**Milestone:** UI matches or improves on JSFX

---

### Phase 4: Integration & Polish

| Task | Status | Notes |
|------|--------|-------|
| Test all functionality | [ ] | |
| Handle edge cases | [ ] | |
| Optimize performance | [ ] | |
| Add LUFS meter (optional) | [ ] | Use LUFS_BOOTSTRAP.md |
| Polish UI | [ ] | |

**Milestone:** Plugin is production-ready

---

### Phase 5: Testing & Validation

| Task | Status | Notes |
|------|--------|-------|
| Unit testing | [ ] | |
| Integration testing | [ ] | |
| DAW testing | [ ] | |
| Performance testing | [ ] | |
| Final review | [ ] | Use REVIEW_TEMPLATE.md |

**Milestone:** Plugin passes all tests

---

## Progress Tracking

| Phase | Status | Start Date | End Date |
|-------|--------|------------|----------|
| Phase 1: Foundation | [ ] | | |
| Phase 2: DSP Core | [ ] | | |
| Phase 3: UI Implementation | [ ] | | |
| Phase 4: Integration & Polish | [ ] | | |
| Phase 5: Testing & Validation | [ ] | | |

---

## JSFX to JUCE Mapping

| JSFX Feature | JUCE Equivalent |
|--------------|-----------------|
| `slider` parameters | `AudioParameterFloat` |
| `switch` / `button` | `AudioParameterBool` |
| `引入` (import) | Include headers |
| `@slider` | `processBlock()` |
| `gdraw` / `g.Graphics` | `juce::Graphics` |
| `file` | File I/O |

---

## Notes

Add project-specific notes here:

-

-

---

**Template Version:** 1.0.0
**Last Updated:** 2026-02-06
