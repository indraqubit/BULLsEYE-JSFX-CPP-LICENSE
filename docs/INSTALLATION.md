# BULLsEYE Plugin Installation Guide

**Version:** 1.0.0  
**Last Updated:** 2025-02-06

BULLsEYE is a professional audio plugin for loudness normalization and dynamic range control. This guide covers installation procedures for macOS, Windows, and Linux platforms.

---

## Table of Contents

1. [System Requirements](#system-requirements)
2. [Download Options](#download-options)
3. [macOS Installation](#macos-installation)
4. [Windows Installation](#windows-installation)
5. [Linux Installation](#linux-installation)
6. [DAW Integration](#daw-integration)
7. [Troubleshooting](#troubleshooting)
8. [Uninstallation](#uninstallation)

---

## System Requirements

### macOS

| Requirement | Minimum | Recommended |
|------------|---------|-------------|
| macOS Version | 10.14 (Mojave) | 11.0 (Big Sur) or later |
| Processor | Intel or Apple Silicon | Apple Silicon (M1/M2/M3) |
| RAM | 4 GB | 8 GB or more |
| Storage | 100 MB | 200 MB for plugins and cache |
| DAW | Any AU-compatible DAW | Logic Pro, Ableton Live, Cubase |

### Windows

| Requirement | Minimum | Recommended |
|------------|---------|-------------|
| Windows Version | 10 (64-bit) | 11 |
| Processor | x86_64 compatible | x86_64 with AVX support |
| RAM | 4 GB | 8 GB or more |
| Storage | 100 MB | 200 MB |
| DAW | Any VST3-compatible DAW | Reaper, Cubase, FL Studio |

### Linux

| Requirement | Minimum | Recommended |
|------------|---------|-------------|
| Distribution | Ubuntu 20.04, Fedora 35 | Ubuntu 22.04 LTS or later |
| Architecture | x86_64 | x86_64 with AVX2 |
| RAM | 4 GB | 8 GB or more |
| JACK Audio | Optional but recommended | JACK or ALSA |
| DAW | Any VST3-compatible DAW | Ardour, Reaper, Bitwig |

---

## Download Options

### Option 1: DMG (macOS Only)

For macOS users, download the DMG file for a graphical installation experience.

**Download:** `BULLsEYE-1.0.0-macOS.dmg`

Benefits:
- Drag-and-drop installation
- Automatic shortcut creation
- Includes all documentation

### Option 2: ZIP Archives

Cross-platform ZIP files available for all operating systems:

| File | Contents |
|------|----------|
| `BULLsEYE-1.0.0-macOS-AU.zip` | Audio Unit component (macOS) |
| `BULLsEYE-1.0.0-macOS-VST3.zip` | VST3 plugin (macOS) |
| `BULLsEYE-1.0.0-Windows-VST3.zip` | VST3 plugin (Windows) |
| `BULLsEYE-1.0.0-Linux-VST3.zip` | VST3 plugin (Linux) |
| `BULLsEYE-1.0.0-All-Platforms.zip` | All platforms in one archive |

### Option 3: Homebrew Cask (macOS)

If you use Homebrew, install via:

```bash
brew install --cask bullseye
```

---

## macOS Installation

### Audio Unit (AU) Installation

#### Method 1: Using DMG

1. Download `BULLsEYE-1.0.0-macOS.dmg`
2. Double-click the DMG file to mount it
3. Locate `BULLsEYE.component` in the DMG window
4. Copy to system-wide location (requires administrator access):
   ```bash
   sudo cp -R "/Volumes/BULLsEYE/BULLsEYE.component" /Library/Audio/Plug-Ins/Components/
   ```
   Or copy to user location:
   ```bash
   cp -R "/Volumes/BULLsEYE/BULLsEYE.component" ~/Library/Audio/Plug-Ins/Components/
   ```
5. Eject the DMG

#### Method 2: Using ZIP

1. Download `BULLsEYE-1.0.0-macOS-AU.zip`
2. Extract the ZIP file:
   ```bash
   unzip BULLsEYE-1.0.0-macOS-AU.zip
   ```
3. Copy component:
   ```bash
   # System-wide (requires sudo)
   sudo cp -R BULLsEYE.component /Library/Audio/Plug-Ins/Components/
   
   # Or user-specific
   mkdir -p ~/Library/Audio/Plug-Ins/Components
   cp -R BULLsEYE.component ~/Library/Audio/Plug-Ins/Components/
   ```

#### Method 3: Using Homebrew Cask

```bash
brew install --cask bullseye
```

### VST3 Installation (macOS)

1. Download `BULLsEYE-1.0.0-macOS-VST3.zip`
2. Extract the ZIP file:
   ```bash
   unzip BULLsEYE-1.0.0-macOS-VST3.zip
   ```
3. Copy VST3 plugin:
   ```bash
   # System-wide (requires sudo)
   sudo cp -R BULLsEYE.vst3 /Library/Audio/Plug-Ins/VST3/
   
   # Or user-specific
   mkdir -p ~/Library/Audio/Plug-Ins/VST3
   cp -R BULLsEYE.vst3 ~/Library/Audio/Plug-Ins/VST3/
   ```

### Verify Installation

Check that the plugin is installed:

```bash
# List installed AU plugins
ls /Library/Audio/Plug-Ins/Components/ | grep -i bullseye
ls ~/Library/Audio/Plug-Ins/Components/ | grep -i bullseye

# List installed VST3 plugins
ls /Library/Audio/Plug-Ins/VST3/ | grep -i bullseye
ls ~/Library/Audio/Plug-Ins/VST3/ | grep -i bullseye
```

---

## Windows Installation

### VST3 Installation

#### Standard Installation

1. Download `BULLsEYE-1.0.0-Windows-VST3.zip`
2. Extract the ZIP file to a temporary location
3. Copy the `BULLsEYE.vst3` folder to your VST3 plugins directory:

   **Option A - All Users (requires Administrator):**
   ```
   C:\Program Files\Common Files\VST3\
   ```

   **Option B - Current User:**
   ```
   C:\Users\[YourUsername]\Documents\VST3\
   ```

#### Manual Installation

1. Open File Explorer
2. Navigate to the extracted BULLsEYE folder
3. Right-click on `BULLsEYE.vst3` folder
4. Select "Copy"
5. Navigate to your VST3 directory
6. Right-click and select "Paste"

### Verify Installation

Check installation in common locations:

```
# Check if installed
dir "C:\Program Files\Common Files\VST3\BULLsEYE.vst3"
dir "%USERPROFILE%\Documents\VST3\BULLsEYE.vst3"
```

---

## Linux Installation

### VST3 Installation

#### Standard Installation

1. Download `BULLsEYE-1.0.0-Linux-VST3.zip`
2. Extract the archive:
   ```bash
   unzip BULLsEYE-1.0.0-Linux-VST3.zip
   ```
3. Copy VST3 plugin:
   ```bash
   # User-specific installation (recommended)
   mkdir -p ~/.vst3
   cp -r BULLsEYE.vst3 ~/.vst3/
   
   # System-wide (requires sudo)
   sudo cp -r BULLsEYE.vst3 /usr/lib/vst3/
   ```

#### Flatpak Installation (if applicable)

If you use Flatpak, you may need to copy to the Flatpak VST3 path:
```bash
cp -r BULLsEYE.vst3 ~/.var-app/org.freedesktop.Jack/packages/vst3/
```

### Verify Installation

```bash
# Check user installation
ls ~/.vst3/ | grep -i bullseye

# Check system installation
ls /usr/lib/vst3/ | grep -i bullseye
```

---

## DAW Integration

After installing BULLsEYE, you need to rescan your DAW's plugin list to detect the new plugin.

### Common DAW Procedures

#### Logic Pro (macOS)

1. Go to **Logic Pro > Settings > Plugins and Sound Libraries**
2. Click **Rescan** in the Audio Units section
3. Wait for scan to complete
4. BULLsEYE will appear in the Audio Units > Music category

#### Ableton Live (macOS/Windows)

1. Go to **Options > Preferences**
2. Navigate to the **Plugins** tab
3. Click **Rescan**
4. Ensure "Use Audio Units" is checked (macOS)

#### Cubase (macOS/Windows)

1. Go to **Studio > Studio Management**
2. Click **VST Audio System** tab
3. Click **Scan for Plug-ins**
4. Restart Cubase if prompted

#### Reaper (macOS/Windows/Linux)

1. Go to **Options > Preferences**
2. Navigate to **Plug-ins > VST**
3. Click **Rescan** or "Clear cache / Re-scan"
4. Restart Reaper if needed

#### FL Studio (Windows)

1. Go to **Options > Manage Plugins**
2. Click **Find Plugins**
3. Ensure VST3 path is included
4. Rescan the plugin folder

#### Ardour (Linux)

1. Go to **Edit > Preferences**
2. Navigate to **Plugins > VST**
3. Add your VST3 path if not auto-detected
4. Rescan plugins

---

## Troubleshooting

### macOS Issues

#### "BULLsEYE.component cannot be opened because the developer cannot be verified"

This is a Gatekeeper security warning. To resolve:

**Option 1: Right-click to open (temporary)**
1. Right-click on the DMG file
2. Select "Open"
3. Click "Open" in the dialog

**Option 2: Remove quarantine attribute (permanent)**
```bash
sudo xattr -rd com.apple.quarantine /Library/Audio/Plug-Ins/Components/BULLsEYE.component
```

**Option 3: System Settings (most permanent)**
1. Go to **System Settings > Privacy & Security**
2. Scroll to "Security" section
3. Click "Open Anyway" next to the blocked message

#### Plugin appears but crashes on load

1. Clear plugin cache:
   ```bash
   rm -rf ~/Library/Caches/com.apple.audiounits.cache
   rm -rf ~/Library/Caches/AudioUnitCache/
   ```
2. Rescan plugins in your DAW
3. Check DAW console for error messages

#### Plugin not appearing in DAW

1. Verify installation location:
   ```bash
   ls ~/Library/Audio/Plug-Ins/Components/BULLsEYE.component
   ls /Library/Audio/Plug-Ins/Components/BULLsEYE.component
   ```
2. Check component validity:
   ```bash
   file ~/Library/Audio/Plug-Ins/Components/BULLsEYE.component/Contents/MacOS/BULLsEYE
   ```
3. Rescan AU in your DAW

### Windows Issues

#### "Windows protected your PC"

This is Windows SmartScreen. To resolve:

1. Click "More info"
2. Click "Run anyway"
3. Alternatively, right-click the file and select "Properties"
4. Check "Unblock" at the bottom
5. Click "Apply"

#### Plugin not detected by DAW

1. Verify VST3 path in DAW settings
2. Run DAW as Administrator (first time only)
3. Check 32-bit vs 64-bit compatibility
4. Install Visual C++ Redistributable

#### Audio dropouts or crackling

1. Increase buffer size in DAW settings
2. Update audio driver
3. Disable other CPU-intensive plugins temporarily

### Linux Issues

#### "Cannot load shared library"

Ensure dependencies are installed:
```bash
# On Ubuntu/Debian
sudo apt install libglib2.0-0 libx11-6 libxcb1

# On Fedora
sudo dnf install glib2 libX11 libxcb
```

#### No audio output

1. Check JACK or ALSA configuration
2. Verify plugin is loaded in DAW mixer
3. Check DAW audio settings

#### Plugin crashes DAW on load

1. Check for missing dependencies:
   ```bash
   ldd ~/.vst3/BULLsEYE.vst3/Contents/x86_64-linux/BULLsEYE
   ```
2. Install missing libraries
3. Check system journal:
   ```bash
   journalctl -k | grep -i bullseye
   ```

### Common Solutions (All Platforms)

#### Clear Plugin Cache

Many DAWs cache plugin information. Clear this cache if you experience issues:

| DAW | Cache Location |
|-----|----------------|
| Ableton Live | ~/Library/Caches/Ableton/ (macOS) |
| Logic Pro | ~/Library/Caches/com.apple.audio.audiounits.cache |
| Cubase | ~/Library/Preferences/Cubase/ (macOS) |
| Reaper | %APPDATA%\REAPER\reaper-ccache.txt (Windows) |

#### Reset DAW Settings

1. Back up your DAW settings
2. Reset to default
3. Rescan plugins
4. Restore custom settings gradually

#### Check System Resources

1. Monitor CPU usage during plugin use
2. Increase buffer size
3. Disable other plugins temporarily
4. Restart your DAW and computer

#### Reinstall

If all else fails:

1. Uninstall completely (see below)
2. Download fresh copy
3. Reinstall
4. Rescan DAW plugins

---

## Uninstallation

### macOS Uninstall

#### Remove AU Component

**System-wide (requires sudo):**
```bash
sudo rm -rf /Library/Audio/Plug-Ins/Components/BULLsEYE.component
```

**User-specific:**
```bash
rm -rf ~/Library/Audio/Plug-Ins/Components/BULLsEYE.component
```

#### Remove VST3 Plugin

**System-wide (requires sudo):**
```bash
sudo rm -rf /Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3
```

**User-specific:**
```bash
rm -rf ~/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3
```

#### Homebrew Cask Uninstall
```bash
brew uninstall --cask bullseye
```

### Windows Uninstall

**Standard removal:**
1. Go to Control Panel > Programs and Features
2. Find "BULLsEYE"
3. Click Uninstall

**Manual removal:**
```cmd
rmdir /s "C:\Program Files\Common Files\VST3\BULLsEYE.vst3"
```
Or:
```cmd
rmdir /s "%USERPROFILE%\Documents\VST3\BULLsEYE.vst3"
```

### Linux Uninstall

**User-specific:**
```bash
rm -rf ~/.vst3/BULLsEYE.vst3
```

**System-wide (requires sudo):**
```bash
sudo rm -rf /usr/lib/vst3/BULLsEYE.vst3
```

---

## Getting Help

If you encounter issues not covered in this guide:

1. Check the [User Manual](../docs/UserManual.md)
2. Review the [README](../README.md)
3. Search [existing issues](https://github.com/indraqadarsih/BULLsEYE-JSFX-CPP/issues)
4. [Open a new issue](https://github.com/indraqadarsih/BULLsEYE-JSFX-CPP/issues/new) with:
   - Operating system and version
   - DAW name and version
   - Plugin format tried (AU/VST3)
   - Steps to reproduce the problem
   - Any error messages

---

## Version History

| Version | Date | Changes |
|---------|------|---------|
| 1.0.0 | 2025-02-06 | Initial release |

---

*This installation guide is part of the BULLsEYE project.*
*For the latest version, visit: https://github.com/indraqadarsih/BULLsEYE-JSFX-CPP*
