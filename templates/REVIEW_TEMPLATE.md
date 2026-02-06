# Code Review Template

**Checklist for reviewing migrated JUCE plugin code**

---

## Review Information

| Field | Value |
|-------|-------|
| **Reviewer** | Name |
| **Date** | YYYY-MM-DD |
| **Phase** | Phase 1-5 |
| **Files Reviewed** | List files |

---

## SSOT Compliance

| Checklist Item | Status | Notes |
|----------------|--------|-------|
| All constants in SSOT files | [ ] | |
| No hardcoded values in .cpp | [ ] | |
| Helper functions in namespaces | [ ] | |
| Consistent naming convention | [ ] | |
| `constexpr` where applicable | [ ] | |

---

## TETRIS Compliance

| Checklist Item | Status | Notes |
|----------------|--------|-------|
| Thread-safe DSP implementation | [ ] | |
| No UI access in audio thread | [ ] | |
| No allocations in processBlock | [ ] | |
| `static_assert` for trivially copyable | [ ] | |
| Parameter smoothing implemented | [ ] | |

---

## Code Quality

| Checklist Item | Status | Notes |
|----------------|--------|-------|
| Consistent naming (PascalCase/camelCase) | [ ] | |
| Proper include order | [ ] | |
| No unused includes | [ ] | |
| Meaningful variable names | [ ] | |
| Functions are focused (< 50 lines) | [ ] | |
| No code duplication | [ ] | |

---

## JUCE Best Practices

| Checklist Item | Status | Notes |
|----------------|--------|-------|
| Module-specific includes used | [ ] | |
| Proper AudioProcessor subclassing | [ ] | |
| APVTS parameter layout defined | [ ] | |
| Process block optimized | [ ] | |
| UI component lifecycle correct | [ ] | |

---

## DSP Accuracy

| Checklist Item | Status | Notes |
|----------------|--------|-------|
| Output matches JSFX reference | [ ] | |
| No introduced distortion | [ ] | |
| Proper gain staging | [ ] | |
| Parameter smoothing working | [ ] | |
| No performance issues | [ ] | |

---

## UI/UX

| Checklist Item | Status | Notes |
|----------------|--------|-------|
| All components visible | [ ] | |
| No overlapping elements | [ ] | |
| Proper sizing/responsiveness | [ ] | |
| Colors match design specs | [ ] | |
| No visual glitches | [ ] | |

---

## Build & Testing

| Checklist Item | Status | Notes |
|----------------|--------|-------|
| Compiles without warnings | [ ] | |
| All platforms build | [ ] | |
| Unit tests pass | [ ] | |
| Integration tests pass | [ ] | |
| No memory leaks detected | [ ] | |

---

## Issues Found

| Severity | Description | File | Line |
|----------|-------------|------|------|
| | | | |
| | | | |
| | | | |

---

## Review Summary

### Approved
- [ ] Yes - Ready to merge
- [ ] No - Changes requested

### Comments
_

---

**Template Version:** 1.0.0
**Last Updated:** 2026-02-06
