# BULLsEYE Screenshot Capture Guide

**Purpose:** Step-by-step instructions for capturing BULLsEYE plugin screenshots for documentation

**Time Required:** 15-30 minutes

---

## Prerequisites

Before beginning screenshot capture, ensure the following:

1. **BULLsEYE is built and installed** - Verify the plugin loads correctly in your DAW
2. **DAW is installed and configured** - REAPER or Ableton Live (or both)
3. **Test audio is available** - A short audio clip with varying loudness levels
4. **Screenshot utility is accessible** - macOS built-in shortcuts or dedicated app

---

## Screenshot Directory

All screenshots should be saved to:
```
docs/screenshots/
```

Expected files:
- `screenshot-main-interface.png`
- `screenshot-mode-selector.png`
- `screenshot-balanced-display.png`
- `screenshot-hot-display.png`
- `screenshot-quiet-display.png`
- `screenshot-led-meter-detail.png`
- `screenshot-deviation-bar.png`
- `screenshot-reaper-integration.png`
- `screenshot-ableton-integration.png`

---

## REAPER Screenshot Workflow

### Step 1: Launch REAPER
1. Open REAPER from your Applications folder
2. Create a new project (**File > New Project**) or open an existing project
3. Ensure audio engine is active (check Transport panel)

### Step 2: Set Up Audio Track
1. Go to **Track > Insert New Track**
2. Name the track "BULLsEYE Test"
3. Add audio to the track by dragging a WAV or MP3 file to the track area
4. Ensure track is not muted and volume fader is at 0 dB

### Step 3: Add BULLsEYE to FX Chain
1. Click the **FX** button on the track (looks like a plug icon)
2. The FX Chain window opens
3. Click **Add** to browse available plugins
4. Search for "BULLsEYE" in the plugin list
5. Select **BULLsEYE** and click **Add**
6. Close the FX browser, keep FX Chain window open

### Step 4: Open Plugin Window
1. In the FX Chain, find BULLsEYE in the list
2. Double-click BULLsEYE or click the **Edit** button
3. The BULLsEYE plugin window opens

### Step 5: Capture Screenshots

**Screenshot 1: Main Interface**
1. Ensure audio is playing through the track
2. Set Mode Selector to "Music Non-drums" (target -11 LUFS)
3. Adjust track volume so loudness is near target (check LED meter)
4. Use screenshot shortcut: `Cmd + Shift + 4`, then `Space`, click window
5. Save as: `screenshot-main-interface.png`

**Screenshot 2: Mode Selector Expanded**
1. With plugin window active, click the dropdown arrow on mode selector
2. Wait for dropdown to fully expand showing all three options
3. Capture: `Cmd + Shift + 4`, click the dropdown area
4. Save as: `screenshot-mode-selector.png`

**Screenshot 3: Balanced State (Green)**
1. Set Mode to "Music Non-drums"
2. Adjust track volume so LUFS-I reads approximately -11 LUFS
3. Wait for integrated measurement to stabilize (10-20 seconds)
4. Confirm status indicators are green
5. Capture the plugin window
6. Save as: `screenshot-balanced-display.png`

**Screenshot 4: Hot State (Red)**
1. Increase track volume by approximately +4 to +6 dB
2. LUFS-I should now read above -7 LUFS (depending on content)
3. Confirm status indicators turn red/amber
4. Capture the plugin window
5. Save as: `screenshot-hot-display.png`

**Screenshot 5: Quiet State (Blue)**
1. Decrease track volume by approximately -6 to -10 dB
2. LUFS-I should now read below -17 LUFS
3. Confirm status indicators turn blue/cyan
4. Capture the plugin window
5. Save as: `screenshot-quiet-display.png`

**Screenshot 6: LED Meter Detail**
1. With plugin window open, zoom in if possible (REAPER may support UI scaling)
2. Position window to maximize LED strip visibility
3. Ensure individual LED segments are clearly visible
4. Capture a focused view of just the LED meter area
5. Save as: `screenshot-led-meter-detail.png`

**Screenshot 7: Deviation Bar States**
1. Return to balanced volume (near -11 LUFS)
2. Capture showing deviation indicator at center
3. Increase volume for hot state, capture showing right-side deviation
4. Decrease volume for quiet state, capture showing left-side deviation
5. Save as: `screenshot-deviation-bar.png`

**Screenshot 8: REAPER Integration**
1. Zoom out to show full REAPER interface
2. Include: Track lane, FX button, FX Chain, Transport bar
3. Show BULLsEYE in context within DAW workflow
4. Capture the entire REAPER window or a wide selection
5. Save as: `screenshot-reaper-integration.png`

---

## Ableton Live Screenshot Workflow

### Step 1: Launch Ableton Live
1. Open Ableton Live from your Applications folder
2. Create a new Live Set (**File > New Live Set**) or open an existing set
3. Switch to Session or Arrangement view as preferred

### Step 2: Set Up Audio Track
1. Go to **Track > Insert Audio Track**
2. Name the track "BULLsEYE Test"
3. Drag audio from the Browser to the track, or record-arm to record
4. Ensure track volume is at 0 dB and not muted

### Step 3: Add BULLsEYE to Device Chain
1. Click the **Device View** button (looks like a chip icon) to show device chain
2. Click **Add Plugin** in the device chain area
3. Search for "BULLsEYE" in the plugin browser
4. Double-click BULLsEYE or drag it to the device chain
5. The plugin appears in the device chain

### Step 4: Open Plugin Window
1. Find BULLsEYE in the device chain
2. Click the **Expand** arrow (x) to show plugin interface
3. The BULLsEYE window expands to show all controls

### Step 5: Capture Screenshots

Follow the same capture sequence as REAPER workflow above, adjusting for Ableton Live interface:

**Key Ableton-specific captures:**
- Show BULLsEYE in device chain with expander expanded
- Capture with Session view showing clip slots
- Capture with Arrangement view showing full timeline
- Include browser with BULLsEYE visible in plugin list

**Screenshot 9: Ableton Integration**
1. Zoom out to show full Ableton Live interface
2. Include: Mixer view, device chain, browser, transport
3. Show BULLsEYE device in context within Ableton workflow
4. Capture the entire Ableton window or appropriate selection
5. Save as: `screenshot-ableton-integration.png`

---

## macOS Screenshot Reference

### Built-in Screenshot Shortcuts

| Shortcut | Action |
|----------|--------|
| `Cmd + Shift + 4` | Capture selected area |
| `Cmd + Shift + 4 + Space` | Capture selected window |
| `Cmd + Control + Shift + 4` | Copy selected area to clipboard |
| `Cmd + Control + Shift + 3` | Copy entire screen to clipboard |
| `Cmd + Shift + 5` | Open Screenshot app (macOS Mojave+) |

### Using Screenshot App (macOS Mojave and Later)

1. Press `Cmd + Shift + 5` to open Screenshot toolbar
2. Select capture mode:
   - **Capture Entire Screen** - Click to capture, or click specific screen
   - **Capture Selected Window** - Click window to capture
   - **Capture Selected Portion** - Drag to select area
3. Options menu allows:
   - Save to desktop, documents, or clipboard
   - Show mouse pointer in capture
   - Timer options
4. Click **Capture** or press return

### Recommended Settings

- **Format:** PNG (lossless, best quality)
- **Location:** Desktop (for sorting), then move to `docs/screenshots/`
- **Show Mouse Pointer:** No (cleaner screenshots)
- **Timer:** 5 seconds (for capturing dropdowns/menus)

---

## Quality Guidelines

### Image Requirements

| Requirement | Specification |
|-------------|---------------|
| Format | PNG |
| Color Space | sRGB (default macOS) |
| Background | Transparent or DAW background |
| Text | Legible at thumbnail size |
| UI Elements | Clearly visible |

### Checklist Before Capturing

- [ ] Plugin window is fully visible and not obscured
- [ ] Audio is playing and meters are active
- [ ] Measurements have stabilized (wait 10-20 seconds)
- [ ] Correct mode is selected for the screenshot type
- [ ] Status indicators show expected colors
- [ ] LED meter shows readable segment pattern
- [ ] No other windows overlap capture area

### Post-Capture Review

After each screenshot:
1. Open the image to verify quality
2. Check that all UI elements are visible
3. Confirm text is readable
4. Verify correct color indicators are visible
5. Retake if necessary

---

## File Naming Convention

Use exact filenames as listed below:

```
docs/screenshots/
├── screenshot-main-interface.png
├── screenshot-mode-selector.png
├── screenshot-balanced-display.png
├── screenshot-hot-display.png
├── screenshot-quiet-display.png
├── screenshot-led-meter-detail.png
├── screenshot-deviation-bar.png
├── screenshot-reaper-integration.png
└── screenshot-ableton-integration.png
```

**Naming Rules:**
- All lowercase
- Hyphens between words
- `.png` extension
- No spaces or special characters

---

## Verification Checklist

After capturing all screenshots, verify each file exists and is readable:

```bash
cd docs/screenshots
ls -la *.png
```

Expected output should show 9 PNG files with reasonable file sizes (50KB-500KB each, depending on capture area).

---

## Troubleshooting Common Issues

### Screenshot Appears Blurry
- **Cause:** Scaling or resolution mismatch
- **Solution:** Capture at native resolution, avoid scaling

### Plugin Window Cuts Off
- **Cause:** Window too large for screen
- **Solution:** Reduce UI zoom, or capture selected portion

### Dropdown Menu Closes Immediately
- **Cause:** Screenshot preview interrupted the capture
- **Solution:** Use `Cmd + Shift + 5` with 5-second timer

### LED Meter Shows No Activity
- **Cause:** Audio not playing or plugin not receiving signal
- **Solution:** Verify track is armed or playing, check plugin is in chain

### Wrong Colors in Screenshot
- **Cause:** Color profile mismatch between DAW and screenshot utility
- **Solution:** Use default sRGB color profile

---

## Estimated Time

| Task | Time |
|------|------|
| Setting up DAW session | 5 minutes |
| Capturing REAPER screenshots | 10-15 minutes |
| Capturing Ableton screenshots (optional) | 5-10 minutes |
| Review and verification | 5 minutes |
| **Total** | **25-40 minutes** |

---

**End of Screenshot Capture Guide**
