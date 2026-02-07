# BULLsEYE Distribution Package Sizes

**Version:** 1.0.0  
**Last Updated:** 2025-02-06

This document records the sizes of all distribution packages for BULLsEYE plugin. Sizes will be updated after each release build.

---

## Current Release: v1.0.0

### Package Size Summary

| Package | Format | Estimated Size | Compressed | Status |
|---------|--------|----------------|-------------|--------|
| macOS DMG | DMG | ~150-200 MB | ~40-60 MB | Pending Build |
| macOS AU | ZIP | ~40-60 MB | ~35-55 MB | Pending Build |
| macOS VST3 | ZIP | ~40-60 MB | ~35-55 MB | Pending Build |
| Windows VST3 | ZIP | ~40-60 MB | ~35-55 MB | Pending Build |
| Linux VST3 | ZIP | ~30-50 MB | ~25-45 MB | Pending Build |
| All Platforms | ZIP | ~150-200 MB | ~130-180 MB | Pending Build |

### Detailed Size Breakdown

#### macOS Audio Unit (AU) Component
```
BULLsEYE.component/
├── Contents/
│   ├── Info.plist                    (~2 KB)
│   ├── PkgInfo                       (~8 B)
│   ├── Resources/
│   │   ├── English.lproj/           (~10 KB)
│   │   ├── AudioUnitResources.rsrc  (~50 KB)
│   │   └── Icon.icns                 (~50 KB)
│   └── MacOS/
│       └── BULLsEYE                  (~35-55 MB, universal binary)
└──
Total: ~35-55 MB (uncompressed)
```

#### macOS VST3 Plugin
```
BULLsEYE.vst3/
├── Contents/
│   ├── Info.plist                   (~2 KB)
│   ├── PkgInfo                      (~8 B)
│   ├── Resources/
│   │   ├── English.lproj/          (~10 KB)
│   │   └── Icon.icns               (~50 KB)
│   ├── x86_64-darwin/
│   │   └── BULLsEYE                 (~35-55 MB)
│   └── aarch64-darwin/
│       └── BULLsEYE                 (~35-55 MB, if universal)
└── Contents.meta.json               (~1 KB)
Total: ~35-55 MB (uncompressed)
```

#### Windows VST3 Plugin
```
BULLsEYE.vst3/
├── Contents/
│   ├── x86_64-win/
│   │   ├── BULLsEYE.dll             (~35-55 MB)
│   │   └── BULLsEYE.vst3/           (~10 KB)
│   └── Resources/
│       └── Icon.icns                (~50 KB)
└── Contents.meta.json               (~1 KB)
Total: ~35-55 MB (uncompressed)
```

#### Linux VST3 Plugin
```
BULLsEYE.vst3/
├── Contents/
│   ├── x86_64-linux/
│   │   └── BULLsEYE.so              (~25-45 MB)
│   └── Resources/
│       └── Icon.icns                (~50 KB)
└── Contents.meta.json               (~1 KB)
Total: ~25-45 MB (uncompressed)
```

---

## Size Optimization

### Current Compression Ratios

| Component | Uncompressed | ZIP (Deflate) | DMG (UDZO) |
|-----------|--------------|---------------|------------|
| AU Component | 40 MB | ~37 MB (7%) | ~50 MB (25%) |
| VST3 Plugin | 40 MB | ~37 MB (7%) | ~50 MB (25%) |
| With Docs | 42 MB | ~39 MB (7%) | ~52 MB (25%) |

### Size Reduction Strategies

1. **Universal Binary Consideration**
   - Apple Silicon binary only: ~25-35 MB savings
   - Intel binary only: ~25-35 MB savings
   - Universal binary: Maximum compatibility

2. **Symbol Stripping**
   - Debug symbols: +10-15 MB
   - Stripped release: Default (recommended)

3. **Resource Optimization**
   - Remove unused language resources: ~5-10 KB savings
   - Compress icons: ~2-5 KB savings

---

## Build Commands

### Generate Package Sizes

After building the plugins, run these commands to generate actual sizes:

```bash
# Build the project first
cd /path/to/BULLsEYE-JSFX-CPP
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release

# Copy artifacts to artefacts directory
mkdir -p BULLsEYE_artefacts
cp -R BULLsEYE.component BULLsEYE.vst3 BULLsEYE_artefacts/

# Create distribution packages
cd ../build
./create_dmg.sh
./create_zip.sh --all

# Generate size report
echo "=== Package Sizes ===" > ../distribution/SIZE_REPORT.md
echo "" >> ../distribution/SIZE_REPORT.md
echo "Generated: $(date)" >> ../distribution/SIZE_REPORT.md
echo "" >> ../distribution/SIZE_REPORT.md
for pkg in ../distribution/*.{dmg,zip}; do
    if [ -f "$pkg" ]; then
        echo "- $(basename $pkg): $(du -h "$pkg" | cut -f1)" >> ../distribution/SIZE_REPORT.md
        echo "  SHA256: $(shasum -a 256 "$pkg" | cut -d' ' -f1)" >> ../distribution/SIZE_REPORT.md
    fi
done

# Display report
cat ../distribution/SIZE_REPORT.md
```

### Calculate Checksums

```bash
# SHA256 checksums for all packages
cd /path/to/BULLsEYE-JSFX-CPP/distribution
shasum -a 256 *.dmg *.zip

# Individual package details
for file in *.zip; do
    echo "=== $file ==="
    unzip -l "$file" | tail -5
done
```

---

## Historical Sizes

### v1.0.0 (Initial Release)

| Package | Size | Checksum | Build Date |
|---------|------|----------|------------|
| BULLsEYE-1.0.0-macOS.dmg | TBD | TBD | TBD |
| BULLsEYE-1.0.0-macOS-AU.zip | TBD | TBD | TBD |
| BULLsEYE-1.0.0-macOS-VST3.zip | TBD | TBD | TBD |
| BULLsEYE-1.0.0-Windows-VST3.zip | TBD | TBD | TBD |
| BULLsEYE-1.0.0-Linux-VST3.zip | TBD | TBD | TBD |
| BULLsEYE-1.0.0-All-Platforms.zip | TBD | TBD | TBD |

---

## Recommended Upload Sizes

| Platform | Max Size | Notes |
|----------|----------|-------|
| GitHub Releases | 2 GB | Free, recommended |
|itch.io | 500 MB | For game audio use |
|Steam Workshop | 1 GB | If distributed via Steam |
|Plugin Boutique | 500 MB | Standard marketplace limit |
|Apple App Store | N/A | Not applicable (AU via DMG) |

---

## Disk Space Requirements

### Build Requirements

```
Source code:                    ~10 MB
Build artifacts (intermediate): ~500 MB
Final distribution packages:    ~200 MB
Total workspace:                ~1 GB
```

### Installation Requirements

| Platform | Format | Disk Space |
|----------|--------|------------|
| macOS | AU | ~40 MB |
| macOS | VST3 | ~40 MB |
| Windows | VST3 | ~40 MB |
| Linux | VST3 | ~30 MB |

---

## Related Documentation

- [Installation Guide](../docs/INSTALLATION.md)
- [User Manual](../docs/UserManual.md)
- [README](../README.md)
- [CMakeLists.txt](../CMakeLists.txt)

---

*This document is part of the BULLsEYE distribution packaging system.*
*For updates, visit: https://github.com/indraqadarsih/BULLsEYE-JSFX-CPP*
