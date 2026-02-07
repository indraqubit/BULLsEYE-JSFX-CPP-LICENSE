# Contributing to BULLsEYE

Thank you for your interest in contributing to BULLsEYE! This document provides guidelines and instructions for contributing.

## Development Philosophy

### TETRIS Principles

BULLsEYE follows the TETRIS architecture for audio plugin development:

- **Thread-Safe**: All communication between audio and UI threads uses atomic operations
- **Efficient**: DSP operations optimized with coefficient caching and batched updates
- **Testable**: 42 integration tests covering DSP core, UI components, and parameter handling
- **Robust**: Comprehensive edge case handling (NaN, infinity, denormals, overflow)
- **Iterative**: Modular components that can be developed and tested independently
- **Single-Pass**: DSP processing completes in a single pass through the buffer
- **Simple**: Clear separation between Model (DSP), View (UI), and Controller (Processor)

### Single Source of Truth (SSOT)

All plugin constants must be defined in the SSOT files:

| Constant Type | File | Namespace |
|---------------|------|-----------|
| Model constants | `Source/SSOT/ModelSSOT.h` | `ModelSSOT` |
| UI constants | `Source/SSOT/UISSOT.h` | `UISSOT` |
| DSP constants | `Source/SSOT/DSPSSOT.h` | `DSPSSOT` |
| Processor constants | `Source/SSOT/ProcessorSSOT.h` | `ProcessorSSOT` |

**Rules:**
- Never hardcode values in implementation files
- All tunable parameters must have SSOT definitions
- SSOT files document the purpose and constraints of each constant
- Changes to SSOT values should be documented in LEARNINGS.md

## Getting Started

### Prerequisites

- macOS (for AU/VST3 builds)
- CMake 3.15 or higher
- JUCE 8 framework (included as git submodule)
- C++17 compatible compiler

### Building

```bash
# Clone the repository
git clone https://github.com/yourusername/BULLsEYE-JSFX-CPP.git
cd BULLsEYE-JSFX-CPP

# Initialize submodules
git submodule update --init --recursive

# Build
./build.sh
```

### Development Workflow

1. **Create a feature branch** from `main`
2. **Make changes** following SSOT and TETRIS principles
3. **Run tests** to verify functionality
4. **Update documentation** (REFERENCE.md, LEARNINGS.md)
5. **Submit pull request** with clear description

## Code Standards

### C++ Guidelines

- Use C++17 features where appropriate
- Prefer `std::atomic` for thread-safe variables
- Use `constexpr` for compile-time constants
- Follow JUCE coding conventions
- Document all public interfaces

### DSP Implementation

- All DSP must complete in single pass
- Use cached coefficients for filter operations
- Batch atomic updates (100Hz max for meter values)
- Handle edge cases: NaN, infinity, denormals, overflow

### UI Implementation

- Never access APVTS from audio thread
- Use timer-based updates (30Hz recommended)
- Cache expensive calculations (LED positions, colors)
- Follow UISSOT for colors and dimensions

### Testing Requirements

All contributions must include:

1. **Unit tests** for new DSP functionality
2. **Integration tests** for parameter changes
3. **Manual testing** for UI changes

Run tests with:
```bash
cd build && make test
```

## Submitting Changes

### Pull Request Checklist

- [ ] Branch is up to date with main
- [ ] All tests pass
- [ ] Code follows style guidelines
- [ ] SSOT constants updated if needed
- [ ] LEARNINGS.md updated with new insights
- [ ] CHANGELOG.md updated (add entry under [Unreleased])
- [ ] Build succeeds for both AU and VST3

### Commit Messages

Follow conventional commits:
- `feat: Add new content type mode`
- `fix: Correct True Peak calculation`
- `perf: Optimize filter coefficient caching`
- `docs: Update REFERENCE.md`
- `test: Add integration tests for APVTS`

## Reporting Issues

When reporting issues, include:
1. Operating system and version
2. DAW and version
3. Plugin format (AU/VST3)
4. Steps to reproduce
5. Expected vs actual behavior
6. Any error messages or crash logs

## Questions

For questions about contributing, open a GitHub discussion or contact the maintainers.

---

**Note:** This project follows the principles established in LEARNINGS.md. Read it to understand past decisions and trade-offs.

