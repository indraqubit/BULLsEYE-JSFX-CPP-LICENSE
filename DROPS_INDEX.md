# Drops Index

**Portable JSFX to JUCE Migration - Drop-in Organizer**

---

## Enforcement Levels

| Icon | Level | Description | Flexibility |
|------|-------|-------------|-------------|
| 🔴 | **ENFORCED** | Must complete before any other drops | No exceptions |
| 🟡 | **RECOMMENDED** | Build on enforced, best practice patterns | Can skip with reason |
| 🟢 | **OPTIONAL** | Feature-specific, depends on use case | Free to skip |

---

## Quick Navigation

| Step | Drop-in | Level | Status |
|------|---------|-------|--------|
| 1 | [README.md](./README.md) | 🔴 ENFORCED |  |
| 2 | [LUFS_BOOTSTRAP.md](./LUFS_BOOTSTRAP.md) | 🟡 RECOMMENDED |  |
| 3 | [DSP_TEMPLATE.md](./templates/DSP_TEMPLATE.md) | 🟡 RECOMMENDED |  |
| 4 | [COMPONENT_TEMPLATE.md](./templates/COMPONENT_TEMPLATE.md) | 🟢 OPTIONAL |  |

---

## Dependency Graph

```
🔴 ENFORCED (Complete first - no exceptions)
┌─────────────────────────────────────────────────────────┐
│                                                         │
│  ┌─────────────┐                                        │
│  │   Step 1    │  README.md                             │
│  │  README.md  │  SSOT + TETRIS Framework               │
│  │             │  Project structure                     │
│  └──────┬──────┘                                        │
│         │                                                │
└─────────┼────────────────────────────────────────────────┘
          │
          ▼
┌─────────┴────────────────────────────────────────────────┐
│  🟡 RECOMMENDED (Best practices - skip with reason)     │
│                                                         │
│  ┌─────────────┐     ┌─────────────┐                   │
│  │   Step 2    │     │   Step 3    │                   │
│  │ LUFS_BOOT   │     │ DSP_TEMPLATE│                   │
│  │ STRIP METER │     │ DSP PATTERNS│                   │
│  └──────┬──────┘     └──────┬──────┘                   │
│         │                   │                           │
│         └─────────┬─────────┘                           │
│                   │                                     │
│                   ▼                                     │
│         ┌─────────────────────┐                          │
│         │   Step 4 (Optional) │                          │
│         │  COMPONENT TEMPLATE │                          │
│         │  UI COMPONENTS      │                          │
│         └─────────────────────┘                          │
└─────────────────────────────────────────────────────────┘
```

---

## Drop-in Details

### 🔴 Step 1: README.md (ENFORCED)

**File:** `README.md`

**Purpose:** Base architecture foundation for JSFX to JUCE migration

**What it provides:**
- SSOT (Single Source of Truth) architecture
- TETRIS DSP compliance principles
- Complete project structure
- CMake build configuration
- Migration checklist (5 phases)

**When to apply:** First - before any other drops

**Enforcement:** 🔴 REQUIRED - Cannot skip

**Status:** 

---

### 🟡 Step 2: LUFS_BOOTSTRAP.md (RECOMMENDED)

**File:** `LUFS_BOOTSTRAP.md`

**Purpose:** Add calibrated LED strip LUFS meter for loudness monitoring

**What it provides:**
- LEDStripMeter component
- Deferred bootstrap pattern
- Color-coded zones (green/yellow/orange/red)
- Target marker line
- Smooth animation (30fps)

**Calibration presets:**
- Streaming (Spotify/YouTube): -14 LUFS
- Broadcast (EBU R128): -23 LUFS
- Cinema (Netflix): -27 LUFS
- Mastering: -12 LUFS

**When to apply:** After Step 1, when you need loudness monitoring

**Prerequisites:**
- PluginProcessor with LUFS/RMS calculation
- Basic UI editor structure

**Enforcement:** 🟡 RECOMMENDED - Skip only if you don't need loudness monitoring

**Status:** 

---

### 🟡 Step 3: DSP_TEMPLATE.md (RECOMMENDED)

**File:** `templates/DSP_TEMPLATE.md`

**Purpose:** DSP implementation patterns and templates

**What it provides:**
- TETRIS-compliant processor templates
- Mode logic implementation
- Parameter handling patterns
- Audio processing callbacks

**When to apply:** During DSP Core phase (Phase 2 of migration)

**Prerequisites:**
- Step 1 (README.md) architecture

**Enforcement:** 🟡 RECOMMENDED - Skip only if you have own DSP patterns

**Status:** 

---

### 🟢 Step 4: COMPONENT_TEMPLATE.md (OPTIONAL)

**File:** `templates/COMPONENT_TEMPLATE.md`

**Purpose:** UI component development patterns

**What it provides:**
- Component base templates
- APVTS connection patterns
- Visual feedback implementation
- Component lifecycle management

**When to apply:** During UI Implementation phase (Phase 3 of migration)

**Prerequisites:**
- Step 1 (README.md) architecture
- Step 3 (DSP_TEMPLATE.md) for DSP connection

**Enforcement:** 🟢 OPTIONAL - Free to skip if you have own UI patterns

**Status:** 

---

## Progress Tracker

| Step | Drop-in | Level | Status | Applied Date | Notes |
|------|---------|-------|--------|--------------|-------|
| 1 | README.md | 🔴 ENFORCED |  |  | Base architecture |
| 2 | LUFS_BOOTSTRAP.md | 🟡 RECOMMENDED |  |  | LUFS meter |
| 3 | DSP_TEMPLATE.md | 🟡 RECOMMENDED |  |  | DSP patterns |
| 4 | COMPONENT_TEMPLATE.md | 🟢 OPTIONAL |  |  | UI components |

---

## Enforcement Rules

### 🔴 ENFORCED (Step 1 only)
- Must complete first
- No other drops can be applied before this
- Contains SSOT + TETRIS framework

### 🟡 RECOMMENDED
- Should apply after ENFORCED
- Skipping requires documented reason
- Best practice patterns for migration

### 🟢 OPTIONAL
- Free to skip based on use case
- No penalties or warnings
- Feature-specific enhancements

---

## Adding New Drops

To add a new drop-in:

1. Create markdown file in root or templates/ folder
2. Add entry to this index with:
   - Step number
   - File path
   - Purpose description
   - Prerequisites
   - Status field
3. Update dependency graph if applicable

---

## Related Files

- [README.md](./README.md) - Main migration guide
- [LEARNINGS.md](./LEARNINGS.md) - Development notes
- [MIGRATION_PLAN.md](./MIGRATION_PLAN.md) - Migration roadmap

---

## Directory Quick Reference

```
📁 portable-jsfx-to-juce-migration/
├── 📁 jsfx/       → Original JSFX files
├── 📁 Source/     → JUCE C++ code (SSOT, DSP, Components)
├── 📁 build/      → Build output (VST3, AU, LV2)
└── templates/     → Code templates
```

---

**Last Updated:** 2026-02-06
**Enforcement:** Active - See Enforcement Levels above
