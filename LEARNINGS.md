# Learnings

**Development notes, decisions, and lessons learned during migration**

---

## Date: YYYY-MM-DD

### Topic: Brief description

#### Problem
What issue or question was encountered?

#### Solution
How was it resolved?

#### Key Takeaways
- Point 1
- Point 2
- Point 3

#### Related Files
- `Source/...`
- `templates/...`

---

## Previous Entries

### Date: 2025-12-20

#### Topic: Initial setup

**Problem:** Understanding JUCE 8 module includes

**Solution:** JUCE 8 uses module-specific includes instead of JuceHeader.h

```cpp
// JUCE 8 style
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_graphics/juce_graphics.h>

// Old style (not recommended in JUCE 8)
#include <JuceHeader.h>
```

**Key Takeaways:**
- Always use module-specific includes
- Check JUCE 8 migration guide for breaking changes

---

## Categories

| Category | Description |
|----------|-------------|
| **Architecture** | SSOT, TETRIS, design patterns |
| **DSP** | Audio processing, algorithms |
| **UI** | Components, visual design |
| **Build** | CMake, JUCE configuration |
| **Debug** | Troubleshooting, workarounds |

---

## Adding New Entries

Use this format:

```markdown
## Date: YYYY-MM-DD

### Topic: Brief description

#### Problem
...

#### Solution
...

#### Key Takeaways
- ...

#### Related Files
- ...
```

---

**Start adding your learnings here!**
