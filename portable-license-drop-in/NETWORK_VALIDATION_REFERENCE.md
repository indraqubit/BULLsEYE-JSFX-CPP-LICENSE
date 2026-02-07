# Network Validation Reference - License System

**Document Version:** 2.0 (Portable)  
**Last Updated:** 2026-02-07  
**Purpose:** Conceptual reference for license system network validation and OpenSSL configuration  
**Scope:** AnalogMorphV333 license system across Windows, macOS, and Linux platforms  
**Approach:** Code patterns + file references (see source files for complete implementation)

---

## Table of Contents

1. [License System Network Architecture](#1-license-system-network-architecture)
2. [OpenSSL Integration Reference](#2-openssl-integration-reference)
3. [Platform-Specific OpenSSL Configurations](#3-platform-specific-openssl-configurations)
4. [Network Request Implementation](#4-network-request-implementation)
5. [Security & Certificate Validation](#5-security--certificate-validation)
6. [Troubleshooting & Diagnostics](#6-troubleshooting--diagnostics)
7. [Compliance & Laws Reference](#7-compliance--laws-reference)
8. [Quick Reference Cards](#8-quick-reference-cards)

---

## 1. License System Network Architecture

### 1.1 API Configuration

**Location:** `AnalogMorphV3/Source/LicenseSSOT/LicenseSSOT_API.h`

**Configuration Pattern:**
- Base URL: `https://byiqaudio.com/api/v1`
- Endpoints: Generate, Verify, Deactivate, Transfer, Health
- Timeout: 30 seconds
- Max Retries: 3 attempts
- Content-Type: `application/json`

**Key Constants:**
```cpp
ApiBaseUrl = "https://byiqaudio.com/api/v1"
timeoutSeconds = 30
maxRetries = 3
ContentType = "application/json"
```

### 1.2 Network Settings Summary

| Setting | Value | Purpose |
|---------|-------|---------|
| **Base URL** | `https://byiqaudio.com/api/v1` | Production API endpoint |
| **Timeout** | 30 seconds | Connection + request timeout |
| **Max Retries** | 3 attempts | Network resilience |
| **Retry Strategy** | Exponential backoff (500ms → 1000ms → 2000ms) | Avoid overwhelming server |
| **Content-Type** | `application/json` | API contract requirement |

### 1.3 API Endpoints

| Method | Endpoint | Purpose | Request Body |
|--------|----------|---------|--------------|
| POST | `/plugins/licenses/generate` | Request new license | `{email, machineId, productCode}` |
| POST | `/plugins/licenses/verify` | Verify existing license | `{license, machineId, productCode}` |
| POST | `/plugins/licenses/deactivate` | Deactivate license | `{license, email, machineId, productCode}` |
| POST | `/plugins/licenses/transfer` | Transfer to new machine | `{license, machineId, productCode}` |
| GET | `/plugins/health` | Health check | None |

### 1.4 Request Flow Diagram

```
┌─────────────────┐
│  LicenseManager │
└────────┬────────┘
         │
         ├───► LicenseAPIClient::requestLicense()
         │
         ├───► Build JSON request
         │
         ├───► makeRequest(endpoint, "POST", body, callback)
         │
         ├───► Background thread launch (juce::Thread::launch)
         │
         ├───► Retry loop (3 attempts with backoff)
         │
         ├───► juce::URL::createInputStream(timeout=30s)
         │
         ├───► Read response
         │
         ├───► Parse JSON response
         │
         └───► callback(success, message)
                  │
                  └──► LicenseManager callback
```

---

## 2. OpenSSL Integration Reference

### 2.1 JWT Signature Verification

**Location:** `AnalogMorphV3/Source/JWTVerifier.cpp`

**Purpose:** Verify JWT license tokens signed by backend using RSA-SHA256

#### OpenSSL Headers Used
```
#include <openssl/rsa.h>      // RSA operations
#include <openssl/pem.h>      // PEM format parsing
#include <openssl/sha.h>      // SHA256 hashing
#include <openssl/evp.h>      // High-level EVP API
#include <openssl/err.h>       // Error handling
#include <openssl/bio.h>       // BIO I/O streams
#include <openssl/x509.h>     // X.509 certificate handling
```

#### Key Functions

**verifyRSASignature()** - Pattern:
1. Load public key from PEM using BIO
2. Validate RSA key type
3. Initialize verification context with EVP_SHA256
4. Update with data (header.payload)
5. Finalize verification
6. Clean up resources
7. Return (verifyResult == 1)

**See:** `JWTVerifier.cpp:verifyRSASignature()` for complete implementation

**verify()** - Main Verification Function:
1. Extract JWT parts (header, payload, signature)
2. Decode signature from Base64URL
3. Verify RSA signature **FIRST** (contract requirement)
4. Extract and validate claims
5. Verify product code (SSOT match)
6. Verify machine ID matches current machine
7. Verify expiration with clock skew tolerance (300s)
8. Return validation result

**See:** `JWTVerifier.cpp:verify()` for complete implementation

### 2.2 Machine ID Hashing (SHA256 Fallback)

**Location:** `AnalogMorphV3/Source/LicenseAPIClient.cpp`

**Purpose:** Normalize machine ID to 16-character hex format for backend compatibility

**Algorithm:**
1. Extract hex characters only from machine ID
2. If < 16 hex chars:
   - Compute SHA256 hash of full machine ID string
   - Use first 16 bytes (32 hex digits) from hash
3. Format as XXXX-XXXX-XXXX-XXXX

**See:** `LicenseAPIClient.cpp:normaliseMachineIdForBackend()` for complete implementation

### 2.3 OpenSSL Initialization

**Location:** `AnalogMorphV3/Source/LicenseAPIClient.cpp`

**Pattern:**
- One-time initialization using `std::once_flag`
- Calls `OPENSSL_init_crypto()` for OpenSSL < 3.0.0
- Logs warning if initialization fails

**See:** `LicenseAPIClient.cpp` for complete implementation

### 2.4 Base64URL Decoding

**Critical Note:** JWT uses Base64URL, not standard Base64!

**Transformations:**
- Add padding if needed (multiple of 4)
- Replace URL-safe characters: `-` → `+`, `_` → `/`
- Decode using JUCE's Base64 converter

**See:** `JWTVerifier.cpp:base64UrlDecode()` for complete implementation

---

## 3. Platform-Specific OpenSSL Configurations

### 3.1 Windows (Static Linking)

**Location:** `cmake/openssl.cmake`

#### Strategy
- **Link Type:** Static libraries (no DLL dependencies)
- **Runtime:** `/MT` (static CRT, zero external dependencies)
- **vcpkg Triplet:** `x64-windows-static`
- **Libraries:** `libcrypto_static.lib`, `libssl_static.lib`

#### CMake Configuration Pattern
```cmake
if(PLATFORM_WINDOWS)
    set(OPENSSL_USE_STATIC_LIBS ON CACHE BOOL "" FORCE)
    set(VCPKG_LIBRARY_LINKAGE "static")
    find_package(OpenSSL REQUIRED)
    target_link_libraries(${PLUGIN_NAME} PRIVATE OpenSSL::SSL OpenSSL::Crypto)
endif()
```

**See:** `cmake/openssl.cmake` for complete implementation

#### Verification
```powershell
dumpbin /dependents AnalogMorphV333.vst3\Contents\x86_64-win\AnalogMorphV333.vst3
# Expected: NO libssl.dll or libcrypto.dll in output
```

### 3.2 macOS (Dynamic Linking + Bundling)

**Location:** `cmake/openssl.cmake`

#### Strategy
- **Link Type:** Dynamic dylibs bundled into plugin
- **Architecture:** Universal binary (x86_64 + arm64)
- **Auto-detection:** Apple Silicon (/opt/homebrew) vs Intel (/usr/local)
- **Dylibs:** `libssl.3.dylib`, `libcrypto.3.dylib`
- **Rpath:** `@loader_path/../Frameworks`
- **Versions Supported:** OpenSSL 3.x (preferred) or 1.1
- **Status:** ✅ Universal build completed (Sprint 0.75)

#### CMake Configuration Pattern
```cmake
if(PLATFORM_MACOS)
    set(OPENSSL_USE_STATIC_LIBS OFF)
    
    # Auto-detect Homebrew architecture
    if(EXISTS "/opt/homebrew")
        set(_brew_prefix "/opt/homebrew")  # Apple Silicon
    elseif(EXISTS "/usr/local")
        set(_brew_prefix "/usr/local")      # Intel
    endif()
    
    # Find OpenSSL (3.x or 1.1)
    find_package(OpenSSL REQUIRED)
    
    # Link with @rpath
    set_target_properties(${PLUGIN_NAME} PROPERTIES
        LINK_FLAGS "-Wl,-rpath,@loader_path/../Frameworks"
    )
    
    target_link_libraries(${PLUGIN_NAME} PRIVATE
        OpenSSL::SSL
        OpenSSL::Crypto
    )
endif()
```

**See:** `cmake/openssl.cmake` for complete implementation

#### Bundling Process

**Location:** `cmake/post-build-openssl.cmake`

**Pattern:**
1. Create `Contents/Frameworks/` directory
2. Copy dylibs to Frameworks directory
3. Fix dylib paths with `install_name_tool` (absolute → @rpath)
4. Ad-hoc sign bundle

**See:** `cmake/post-build-openssl.cmake` for complete implementation

#### Verification
```bash
# Verify dylibs are bundled
ls -la AnalogMorphV333.vst3/Contents/Frameworks/

# Check for @rpath (not absolute paths)
otool -L AnalogMorphV333.vst3/Contents/MacOS/AnalogMorphV333 | grep ssl

# Validate bundle
./scripts/validate-osx-bundle.sh AnalogMorphV333.vst3
# Expected: 13/13 checks passed
```

#### Build Universal OpenSSL (Sprint 0.75)

**Location:** `/tmp/openssl-universal/`

**Pattern:**
1. Download OpenSSL source
2. Configure for arm64 (Apple Silicon)
3. Configure for x86_64 (Intel)
4. Build both architectures
5. Use `lipo -create` to combine into universal binary
6. Verify: `lipo -info libssl.3.dylib` shows both architectures

**See:** `OSX/OPENSSL_BUNDLING_STRATEGY.md` for detailed build instructions

### 3.3 Linux (System Shared Libraries)

**Location:** `cmake/openssl.cmake`

#### Strategy
- **Link Type:** System shared libraries
- **Runtime:** System `libssl.so.3`, `libcrypto.so.3`
- **Packages:** `libssl-dev`, `libcrypto++-dev` (or distro equivalents)

#### CMake Configuration Pattern
```cmake
if(PLATFORM_LINUX)
    find_package(OpenSSL REQUIRED)
    target_link_libraries(${PLUGIN_NAME} PRIVATE
        OpenSSL::SSL
        OpenSSL::Crypto
        curl
        pthread
        dl
        rt
    )
endif()
```

**See:** `cmake/openssl.cmake` for complete implementation

#### Package Installation
```bash
# Ubuntu/Debian
sudo apt-get install libssl-dev libcrypto++-dev

# Verify installation
ls -la /usr/lib/x86_64-linux-gnu/libssl.so.3
```

---

## 4. Network Request Implementation

### 4.1 Thread-Safe Async Execution

**Location:** `AnalogMorphV3/Source/LicenseAPIClient.cpp`

**Critical:** All network calls MUST run in background thread to avoid UI freeze

**Pattern:**
1. Use `juce::Thread::launch()` to execute request in background
2. Perform blocking `createInputStream()` in background thread
3. Use `juce::MessageManager::callAsync()` for UI callback
4. Never block UI thread with network operations

**See:** `LicenseAPIClient.cpp:makeRequest()` for complete implementation

### 4.2 Retry Logic with Exponential Backoff

**Location:** `AnalogMorphV3/Source/LicenseAPIClient.cpp`

**Pattern:**
- Max retries: 2 (3 total attempts)
- Initial delay: 500ms
- Exponential backoff: delay *= 2 (500ms → 1000ms → 2000ms)
- Log each retry attempt
- Return error if all retries fail

**See:** `LicenseAPIClient.cpp:makeRequest()` for complete implementation

### 4.3 Request Construction

**Location:** `AnalogMorphV3/Source/LicenseAPIClient.cpp`

**Pattern:**
1. Create `juce::URL` with full endpoint
2. Add POST data using `withPOSTData()`
3. Strip UTF-8 BOM if present (Windows compatibility)
4. Set content-type header
5. Create input stream with timeout

**See:** `LicenseAPIClient.cpp:makeRequest()` for complete implementation

### 4.4 Response Parsing

**Location:** `AnalogMorphV3/Source/LicenseAPIClient.cpp`

**Pattern:**
1. Parse JSON using `juce::JSON::parse()`
2. Validate response is object
3. Check deterministic success/valid flags
4. For health endpoint: accept `success=true` OR `status=ok`
5. For verify endpoint: check both `success=true` AND `valid=true`
6. Extract error message if validation fails
7. Call callback with success/failure

**See:** `LicenseAPIClient.cpp:makeRequest()` for complete implementation

---

## 5. Security & Certificate Validation

### 5.1 SSL/TLS Configuration

**Location:** `AnalogMorphV3/Source/JuceHeader.h`

**Configuration:**
```cpp
#define JUCE_USE_CURL 1
#define JUCE_WEB_BROWSER 0
```

**Mechanism:** JUCE uses system SSL/TLS stack via libcurl for HTTPS requests

### 5.2 Certificate Validation

**Mechanism:**
- Relies on system certificate store
- No explicit SSL certificate validation configuration in code
- JUCE's `juce::URL::createInputStream()` uses system SSL/TLS

**Platform Differences:**

| Platform | Certificate Store | Location |
|----------|------------------|----------|
| Windows | Windows Certificate Store | `Cert:\LocalMachine\Root` |
| macOS | System Keychain | `/System/Library/Keychains/` |
| Linux | CA Certificates | `/etc/ssl/certs/`, `/usr/share/ca-certificates/` |

### 5.3 Code Signing Requirements

#### Windows

**Requirements:**
- **EV Certificate** (Extended Validation) from trusted provider
- Cost: ~$150-400/year
- **RFC 3161 Timestamping** (signature survives certificate expiration)
- **Sign Both:** Installer (.exe) and Plugin binary (.vst3)

**Commands:**
```powershell
# Sign installer
signtool sign /f certificate.pfx /p password `
  /tr http://timestamp.digicert.com `
  /td sha256 /fd sha256 AnalogMorphV333_Installer.exe

# Sign plugin binary
signtool sign /f certificate.pfx /p password `
  /tr http://timestamp.digicert.com `
  /td sha256 /fd sha256 AnalogMorphV333.vst3

# Verify signature
signtool verify /pa AnalogMorphV333_Installer.exe
```

**Reference:** `laws/windows/WINDOWS_VST3_DEPLOYMENT_LAWS.md` (LAW 8)

#### macOS

**Requirements:**
- **Apple Developer ID Application certificate** ($99/year)
- **Hardened Runtime** enabled (`--options runtime`)
- **Notarization** (required for macOS 10.15+)

**Commands:**
```bash
# Development: Ad-hoc signing
codesign --force --deep --sign - AnalogMorphV333.vst3

# Production: Developer ID signing
codesign --force --deep --sign "Developer ID Application: Your Name" \
  --options runtime AnalogMorphV333.vst3

# Notarize (macOS 10.15+)
xcrun notarytool submit AnalogMorphV333.vst3.zip \
  --apple-id "your.email@example.com" \
  --password "app-specific-password" \
  --team-id "YOUR_TEAM_ID" --wait

# Staple notarization ticket
xcrun stapler staple AnalogMorphV333.vst3
```

**Reference:** `laws/osx/MACOS_VST3_DEPLOYMENT_LAWS.md` (LAW 8)

### 5.4 Network Error Detection

**Location:** `AnalogMorphV3/Source/LicenseManager.cpp`

**Error Classification:**
- Network error: "Network error - please try again"
- Connection failed: "Connection failed - check internet connection"
- Storage error: "Unable to save license - check file permissions"

**Offline Mode:**
- Detect network errors (curl, timeout, network)
- Accept cached license if valid
- Log offline mode activation

**See:** `LicenseManager.cpp:translateErrorMessage()` and `LicenseManager.cpp` for complete implementation

---

## 6. Troubleshooting & Diagnostics

### 6.1 Debug Logging

**Location:** `~/Library/Application Support/AnalogMorphV3/license_debug.log`

**Log Format:**
```
2026-02-07 17:00:00 [LicenseAPI] POST /plugins/licenses/generate body={...}
2026-02-07 17:00:01 [LicenseAPI] response(/plugins/licenses/generate)={...}
2026-02-07 17:00:01 [LicenseAPI] Platform: macOS 15.7.3
```

**Viewing Logs:**
```bash
# View recent log entries
tail -f ~/Library/Application\ Support/AnalogMorphV3/license_debug.log

# Search for errors
grep -i error ~/Library/Application\ Support/AnalogMorphV3/license_debug.log
```

### 6.2 Common Network Issues

| Issue | Symptom | Diagnosis | Solution |
|-------|----------|------------|----------|
| **Connection timeout** | "Network error after 3 attempts" | Firewall blocking port 443 | Check firewall settings |
| **SSL handshake failed** | "curl: (35) SSL certificate problem" | CA certificates missing | Install ca-certificates |
| **DNS resolution failed** | "curl: (6) Could not resolve host" | DNS server issue | Check DNS configuration |
| **Backend 500 error** | "Invalid server response" | Backend server error | Contact backend team |
| **Invalid license token** | "License verification failed" | Token corrupted/expired | Request new license |

### 6.3 OpenSSL Verification Commands

#### macOS
```bash
# Check OpenSSL version
openssl version

# Verify dylib architecture
lipo -info /usr/local/opt/openssl@3/lib/libssl.3.dylib

# Check bundled dylibs
ls -la AnalogMorphV333.vst3/Contents/Frameworks/

# Check dylib dependencies
otool -L AnalogMorphV333.vst3/Contents/MacOS/AnalogMorphV333 | grep ssl

# Verify @rpath usage
otool -l AnalogMorphV333.vst3/Contents/MacOS/AnalogMorphV333 | grep LC_RPATH

# Check code signature
codesign -dv AnalogMorphV333.vst3
```

#### Windows
```powershell
# Check OpenSSL installation
where openssl.exe
openssl version

# Check DLL dependencies
dumpbin /dependents AnalogMorphV333.vst3\Contents\x86_64-win\AnalogMorphV333.vst3
```

#### Linux
```bash
# Check OpenSSL version
openssl version

# Check shared libraries
ldd AnalogMorphV3.so | grep ssl

# Verify CA certificates
ls -la /etc/ssl/certs/ca-certificates.crt
```

### 6.4 Network Connectivity Tests

```bash
# Health check endpoint
curl https://byiqaudio.com/api/v1/plugins/health

# Test with verbose SSL info
curl -v https://byiqaudio.com/api/v1/plugins/health 2>&1 | grep -i "ssl\|certificate"

# Test with timeout
curl --max-time 30 https://byiqaudio.com/api/v1/plugins/health

# Test DNS resolution
nslookup byiqaudio.com
dig byiqaudio.com
```

### 6.5 Bundle Validation (macOS)

**Location:** `scripts/validate-osx-bundle.sh`

**Validation:**
```bash
# Validate VST3 bundle
./scripts/validate-osx-bundle.sh AnalogMorphV333.vst3

# Expected output: 13/13 checks passed
```

**Checks Performed:**
1. Bundle structure exists
2. Info.plist present
3. Main binary exists and is executable
4. Code signature valid
5. No absolute paths in dylib dependencies
6. OpenSSL dylibs bundled
7. Universal binary (x86_64 + arm64)
8. Hardened runtime enabled (production)
9. No architecture mismatches
10. All nested code signed
11. No ad-hoc signature issues
12. No provisioning profile issues
13. Gatekeeper acceptance

---

## 7. Compliance & Laws Reference

### 7.1 macOS Deployment Laws (Laws 1-10)

**Reference:** `laws/osx/MACOS_VST3_DEPLOYMENT_LAWS.md`

| Law | Topic | Status | Implementation |
|-----|-------|--------|----------------|
| **LAW 1** | Bundle Integrity | ✅ Implemented | `cmake/openssl.cmake` creates Contents/Frameworks/ |
| **LAW 2** | Dylib Linking | ✅ Implemented | Dynamic bundling with post-build commands |
| **LAW 3** | Architecture-Agnostic | ✅ Implemented | Auto-detects /opt/homebrew vs /usr/local |
| **LAW 4** | Clean Testing | ✅ Resolved | Running on macOS 15.7.3 (Sequoia) |
| **LAW 5** | Silent Codesigning | ✅ Implemented | Ad-hoc signing in post-build |
| **LAW 6** | Cross-Architecture | ✅ Implemented | Universal binary (x86_64 + arm64) |
| **LAW 7** | Release Tagging | ✅ Documented | Tag format: v{X}.{Y}.{Z}-macos-{arch}-{date} |
| **LAW 8** | Signing & Notarization | 📋 Documented | Production workflow documented |
| **LAW 9** | Rpath Management | ✅ Implemented | `install_name_tool` patching in post-build |
| **LAW 10** | Compiler Agent Awareness | 📋 Documented | Trust build over LSP, JUCE is special |

### 7.2 Windows Deployment Laws (Laws 1-9)

**Reference:** `laws/windows/WINDOWS_VST3_DEPLOYMENT_LAWS.md`

| Law | Topic | Status | Implementation |
|-----|-------|--------|----------------|
| **LAW 1** | VST3 Specification | ✅ Compliant | JUCE VST3 SDK v3.7.4 |
| **LAW 2** | Static Linking Doctrine | ✅ Implemented | vcpkg x64-windows-static triplet |
| **LAW 3** | vcpkg Isolation | ✅ Implemented | Separate vcpkg triplet |
| **LAW 4** | Deterministic Build | ✅ Implemented | CMake presets with cache freeze |
| **LAW 5** | Silent Distribution | ✅ Implemented | No UI before wizard |
| **LAW 6** | Cross-Architecture | ✅ Implemented | x64 only (ARM64 planned) |
| **LAW 7** | Release Tagging | ✅ Documented | Tag format: v{X}.{Y}.{Z}-windows-{arch}-{date} |
| **LAW 8** | Code Signing | 📋 Documented | EV certificate + RFC 3161 timestamp |
| **LAW 9** | vcpkg Triplet Specification | ✅ Implemented | x64-windows-static enforced |

### 7.3 License System Laws (Laws V3)

**Reference:** License system internal laws

| Law | Topic | Status | Implementation |
|-----|-------|--------|----------------|
| **LAW V3 §1** | Lock-Free Audio Thread | ✅ Implemented | `std::atomic<LicenseDecision>` |
| **LAW V3 §2** | Grace Period | ✅ Implemented | 7-day grace for expired licenses |
| **LAW V3 §3** | Offline Mode | ✅ Implemented | Cached license validation |
| **LAW V3 §4** | Clock Skew Tolerance | ✅ Implemented | 300-second tolerance in JWTVerifier |
| **LAW V3 §5** | Retry Logic | ✅ Implemented | 3 retries with exponential backoff |

---

## 8. Quick Reference Cards

### 8.1 API Endpoints

| Endpoint | Method | Purpose | Success Response |
|----------|--------|---------|------------------|
| `/plugins/licenses/generate` | POST | Request new license | `{success:true, license:"JWT..."}` |
| `/plugins/licenses/verify` | POST | Verify existing license | `{success:true, valid:true}` |
| `/plugins/licenses/deactivate` | POST | Deactivate license | `{success:true}` |
| `/plugins/licenses/transfer` | POST | Transfer license | `{success:true, deviceLimit:N}` |
| `/plugins/health` | GET | Health check | `{status:"ok"}` |

### 8.2 OpenSSL Library Locations

| Platform | Library Type | Location | Build System |
|----------|--------------|----------|--------------|
| **Windows** | Static | `vcpkg/installed/x64-windows-static/` | vcpkg |
| **macOS** | Dynamic | `/opt/homebrew/opt/openssl@3/lib/` (arm64) | Homebrew |
| **macOS** | Dynamic | `/usr/local/opt/openssl@3/lib/` (x86_64) | Homebrew |
| **macOS** | Bundled | `AnalogMorphV333.vst3/Contents/Frameworks/` | Post-build |
| **Linux** | Shared | `/usr/lib/x86_64-linux-gnu/` | System |

### 8.3 Build Verification Commands

| Platform | Command | Purpose |
|----------|----------|---------|
| **macOS** | `./scripts/validate-osx-bundle.sh` | Validate bundle integrity |
| **macOS** | `otool -L <binary> \| grep ssl` | Check dylib dependencies |
| **macOS** | `lipo -info <dylib>` | Verify universal binary |
| **Windows** | `dumpbin /dependents <binary>` | Check DLL dependencies |
| **Linux** | `ldd <binary> \| grep ssl` | Check shared libraries |

### 8.4 Network Configuration Values

| Setting | Value | Location |
|---------|-------|----------|
| **Base URL** | `https://byiqaudio.com/api/v1` | `LicenseSSOT_API.h` |
| **Timeout** | 30 seconds | `LicenseSSOT_API.h` |
| **Max Retries** | 3 | `LicenseSSOT_API.h` |
| **Retry Delay** | 500ms, 1000ms, 2000ms | `LicenseAPIClient.cpp` |
| **Content-Type** | `application/json` | `LicenseSSOT_API.h` |
| **Grace Period** | 7 days | `LicenseState.h` |
| **Clock Skew** | 300 seconds | `JWTVerifier.cpp` |

### 8.5 Debugging Quick Checklist

- [ ] Check network connectivity: `curl https://byiqaudio.com/api/v1/plugins/health`
- [ ] Review debug log: `tail -f ~/Library/Application Support/AnalogMorphV3/license_debug.log`
- [ ] Verify OpenSSL installation: `openssl version`
- [ ] Check bundled dylibs (macOS): `ls -la AnalogMorphV333.vst3/Contents/Frameworks/`
- [ ] Validate bundle (macOS): `./scripts/validate-osx-bundle.sh AnalogMorphV333.vst3`
- [ ] Check code signature (macOS): `codesign -dv AnalogMorphV333.vst3`
- [ ] Verify static linking (Windows): `dumpbin /dependents AnalogMorphV333.vst3`
- [ ] Test with DAW: Load plugin in REAPER/Logic/Cubase

---

## Appendix

### A. Related Documentation

| Document | Purpose |
|----------|---------|
| `OSX/STATUS.md` | macOS build status and sprint progress |
| `OSX/OPENSSL_BUNDLING_STRATEGY.md` | macOS OpenSSL bundling details |
| `OSX/MACOS_FREEZE.md` | Baseline freeze documentation |
| `laws/osx/MACOS_VST3_DEPLOYMENT_LAWS.md` | macOS deployment laws |
| `laws/windows/WINDOWS_VST3_DEPLOYMENT_LAWS.md` | Windows deployment laws |
| `AnalogMorphV3/Source/LicenseSSOT/LicenseSSOT.h` | License system SSOT |
| `cmake/openssl.cmake` | OpenSSL CMake configuration |
| `cmake/post-build-openssl.cmake` | macOS OpenSSL bundling script |

### B. Key Files Reference

| File | Purpose |
|------|---------|
| `AnalogMorphV3/Source/LicenseAPIClient.cpp` | Network request implementation |
| `AnalogMorphV3/Source/JWTVerifier.cpp` | JWT signature verification |
| `AnalogMorphV3/Source/LicenseManager.cpp` | License state management |
| `AnalogMorphV3/Source/LicenseSSOT/LicenseSSOT_API.h` | API configuration |
| `AnalogMorphV3/Source/LicenseSSOT/LicenseSSOT_Key.h` | RSA public key |
| `AnalogMorphV3/Source/JuceHeader.h` | JUCE_USE_CURL configuration |
| `cmake/openssl.cmake` | Platform-specific OpenSSL config |
| `cmake/post-build-openssl.cmake` | macOS OpenSSL bundling |
| `scripts/validate-osx-bundle.sh` | Bundle validation script |

### C. Glossary

| Term | Definition |
|-------|------------|
| **JWT** | JSON Web Token - License format used by backend |
| **PEM** | Privacy-Enhanced Mail - Base64-encoded certificate format |
| **RSA-SHA256** | RSA signature algorithm with SHA-256 hash |
| **@rpath** | Runtime path - Relative dylib path for macOS bundles |
| **vcpkg** | Microsoft C++ Package Manager for Windows |
| **Homebrew** | Package manager for macOS |
| **Universal Binary** | Binary containing multiple architectures (x86_64 + arm64) |
| **Hardened Runtime** | macOS security feature for code signing |
| **Notarization** | Apple's code signing verification service |
| **SSOT** | Single Source of Truth |
| **Grace Period** | Post-expiration period where license remains valid |

---

**Document End**

For questions or issues, refer to:
- `OSX/README.md` - macOS build entry point
- `OSX/MASTER_PLAN.md` - macOS execution plan
- `README.md` - Project overview

**Note:** For complete code implementation, see source files referenced in each section.