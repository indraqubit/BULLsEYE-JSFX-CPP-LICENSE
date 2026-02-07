# Dependencies Assessment for License System Only

## Overview

This document outlines the platform-specific dependencies and requirements for the Portable License Drop-In Kit license system only. This assessment focuses specifically on the license engine itself, not the surrounding audio plugin frameworks.

## Core Dependencies (All Platforms)

### Minimal Requirements
- **C++11 or later**: Required for standard library features like `std::mutex`, `std::atomic`, and `std::chrono`
- **Standard Library**: No external libraries required for basic license functionality

### Optional Dependencies
- **Network Libraries**: For online license validation only
  - Windows: WinINet or WinHTTP
  - macOS: NSURL or CFNetwork
  - Linux: libcurl

## Platform-Specific Dependencies

### Windows

#### Core Dependencies
- **Windows API**: `windows.h` for hardware identification
  - `GetVolumeInformation()` for disk volume serial
  - `__cpuid()` intrinsic for CPU information

#### Optional Dependencies
- **Registry Access**: For secure license storage
  - `RegCreateKeyEx()`, `RegSetValueEx()`, `RegQueryValueEx()`, `RegCloseKey()`
  - Used for persistent license storage across reboots

- **Crypto API**: For license encryption
  - `CryptEncrypt()`, `CryptDecrypt()`, `CryptCreateHash()`
  - Alternative: Use cross-platform crypto libraries like OpenSSL

#### Implementation Notes
```cpp
#ifdef _WIN32
#include <windows.h>
#include <intrin.h> // for __cpuid

// Machine ID generation on Windows
std::string getMachineId() {
    DWORD volumeSerial;
    GetVolumeInformationA("C:\\", nullptr, 0, &volumeSerial, nullptr, nullptr, nullptr, 0);
    
    int cpuInfo[4] = {0};
    __cpuid(cpuInfo, 1);
    
    return std::to_string(volumeSerial) + "-" + std::to_string(cpuInfo[0]);
}
#endif
```

#### Deployment Considerations
- No additional DLLs required for basic functionality
- Static linking recommended to reduce dependencies
- UAC considerations for registry access

---

### macOS

#### Core Dependencies
- **Foundation Framework**: For system information
  - `NSProcessInfo` for hardware details
  - `IOKit` for hardware identification

#### Optional Dependencies
- **Keychain Services**: For secure license storage
  - `SecKeychainAddGenericPassword()`, `SecKeychainFindGenericPassword()`
  - Preferred over file-based storage for security

- **App Store Receipt Validation**: For App Store distribution
  - `StoreKit` framework
  - Required if distributing through Mac App Store

#### Implementation Notes
```cpp
#ifdef __APPLE__
#include <sys/utsname.h>
#include <TargetConditionals.h>

#if TARGET_OS_MAC
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#endif

// Machine ID generation on macOS
std::string getMachineId() {
    struct utsname sysInfo;
    uname(&sysInfo);
    std::string machineId = std::string(sysInfo.sysname) + "-" + 
                           std::string(sysInfo.nodename) + "-" + 
                           std::string(sysInfo.release);
    
    // Hash for privacy
    std::hash<std::string> hasher;
    return std::to_string(hasher(machineId));
}
#endif
```

#### Deployment Considerations
- Framework linking required: Foundation, IOKit
- Code signing considerations for secure storage
- Sandbox limitations if distributed through Mac App Store

---

### Linux

#### Core Dependencies
- **POSIX**: Standard system calls
  - `uname()` for system information
  - Standard C library for file operations

#### Optional Dependencies
- **libcurl**: For network operations
- **OpenSSL**: For cryptographic operations
- **Systemd**: For service registration (if running background validation)

#### Implementation Notes
```cpp
#ifdef __linux__
#include <sys/utsname.h>
#include <unistd.h>

// Machine ID generation on Linux
std::string getMachineId() {
    struct utsname sysInfo;
    uname(&sysInfo);
    std::string machineId = std::string(sysInfo.sysname) + "-" + 
                           std::string(sysInfo.nodename) + "-" + 
                           std::string(sysInfo.release);
    
    // Try to read from /etc/machine-id if available
    std::ifstream machineIdFile("/etc/machine-id");
    if (machineIdFile.is_open()) {
        std::string id;
        std::getline(machineIdFile, id);
        machineId += "-" + id;
    }
    
    // Hash for privacy
    std::hash<std::string> hasher;
    return std::to_string(hasher(machineId));
}
#endif
```

#### Deployment Considerations
- Package manager dependencies vary by distribution
- Consider static linking to minimize distribution issues
- File permissions for license storage location

---

## License System Integration Dependencies

### Network Licensing
- **HTTP Client Libraries**:
  - Windows: WinINet/WinHTTP
  - macOS: NSURL
  - Linux: libcurl

- **JSON Parsing**: For license server responses
  - Recommended: nlohmann/json (header-only)
  - Alternative: Platform-specific implementations

### Cryptographic Operations
- **Encryption/Decryption**:
  - Windows: CryptoAPI or CNG
  - macOS: Common Crypto
  - Linux: OpenSSL or libgcrypt

- **Digital Signatures**:
  - Windows: CryptoAPI
  - macOS: Security framework
  - Linux: OpenSSL

## License System Security Considerations

### Privacy Regulations
- **GDPR Compliance**: Required for EU distribution
- **CCPA Compliance**: Required for California, USA distribution
- **Local Storage Requirements**: Some regions require local license storage

### Security Standards
- **Encryption Standards**: AES-256 or equivalent
- **Secure Storage**: Keychain (macOS), DPAPI (Windows)
- **Network Security**: HTTPS/TLS for license validation

## License System Deployment Checklist

### Pre-Deployment Verification
- [ ] All platform-specific dependencies tested
- [ ] Minimum system requirements documented
- [ ] Optional dependencies clearly marked
- [ ] Fallback behavior for missing dependencies
- [ ] Error handling for dependency failures

### Package-Specific Requirements
- **Windows Installer**: Runtime dependencies in manifest
- **Mac DMG**: Framework dependencies documented
- **Linux Packages**: Package dependencies specified

## Strategies for Minimizing Dependencies

### Dynamic Loading
```cpp
// Example: Optional dependency loading
#ifdef _WIN32
HMODULE advapi32 = LoadLibraryA("advapi32.dll");
if (advapi32) {
    auto regCreateKey = ( decltype(&RegCreateKeyExA) ) GetProcAddress(advapi32, "RegCreateKeyExA");
    // Use function if available
}
#endif
```

### Feature Flags
```cpp
// Compile-time feature selection
#define LICENSE_ENGINE_CRYPTO_PLATFORM 1
#define LICENSE_ENGINE_CRYPTO_OPENSSL   2

#define LICENSE_ENGINE_CRYPTO_LICENSE_ENGINE_CRYPTO_PLATFORM

#if LICENSE_ENGINE_CRYPTO == LICENSE_ENGINE_CRYPTO_PLATFORM
    // Use platform-specific crypto
#elif LICENSE_ENGINE_CRYPTO == LICENSE_ENGINE_CRYPTO_OPENSSL
    // Use OpenSSL
#endif
```

### Static Linking
- Reduces deployment complexity
- Increases binary size
- Simplifies distribution
- Consider licensing implications of static linking

## License System Troubleshooting

### Windows
- **Missing DLLs**: Use dependency walker to identify missing libraries
- **Registry Access**: Verify UAC permissions
- **Version Conflicts**: Use side-by-side assemblies

### macOS
- **Framework Loading**: Verify @rpath settings
- **Keychain Access**: Handle permission prompts
- **Code Signing**: Required for secure storage APIs

### Linux
- **Library Versions**: Use static linking or version pinning
- **File Permissions**: Check license storage location
- **Distribution Differences**: Test on target distributions

## Conclusion

The Portable License Drop-In Kit license system is designed to minimize dependencies while providing maximum functionality across platforms. By understanding platform-specific requirements and implementing appropriate fallbacks, you can create a robust licensing system that works consistently across all target platforms.