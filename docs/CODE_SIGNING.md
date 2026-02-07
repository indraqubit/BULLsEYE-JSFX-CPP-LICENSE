# Code Signing Guide for BULLsEYE

This document provides comprehensive guidance for code signing BULLsEYE audio plugins for macOS distribution. Code signing is essential for distributing plugins outside the Mac App Store, ensuring that Gatekeeper recognizes your software as legitimately authored.

## Table of Contents

1. [Overview](#overview)
2. [Prerequisites](#prerequisites)
3. [Apple Developer Program Enrollment](#apple-developer-program-enrollment)
4. [Certificate Types](#certificate-types)
5. [Obtaining Certificates](#obtaining-certificates)
6. [Keychain Setup](#keychain-setup)
7. [Code Signing Process](#code-signing-process)
8. [Verification](#verification)
9. [Troubleshooting](#troubleshooting)

## Overview

macOS Gatekeeper enforces code signing requirements for all applications downloaded from the internet. For audio plugins, proper code signing ensures:

- **Gatekeeper Approval**: Users can launch your plugin without security warnings
- **Developer Identity**: Users can verify the plugin originates from you
- **Notarization Compatibility**: Required for Catalina (10.15) and later
- **Professional Distribution**: Expected by DAW users and distributors

### Current Status

As of the latest build environment check:
- **Xcode Command Line Tools**: Installed (version 2410)
- **Code Signing Certificates**: None found (0 valid identities)
- **Development Signature**: Ad-hoc signature available (sufficient for testing)
- **Distribution Signature**: Required for sharing with users

## Prerequisites

### Required Software

Before obtaining code signing certificates, ensure you have:

1. **Xcode Command Line Tools**
   ```bash
   xcode-select --version
   ```
   Should display version 2410 or higher.

2. **Apple Developer Account**
   - Individual: $99/year (USD)
   - Organization: $99/year (requires D-U-N-S number)

3. **Access to Developer Portal**
   - https://developer.apple.com/account/
   - Must be enrolled in Apple Developer Program

### Understanding Team ID

Your Team ID is a unique 10-character identifier assigned by Apple. You will need this for certificate requests.

**To find your Team ID:**
1. Log in to Apple Developer Account
2. Navigate to Membership page
3. Team ID appears under your membership details

**Alternative method (if you have any existing certificate):**
```bash
security find-identity -p codesigning -v
```

## Apple Developer Program Enrollment

### Step 1: Create Apple ID

If you do not already have an Apple ID:
1. Visit https://appleid.apple.com/account
2. Provide valid email and personal information
3. Verify email address

### Step 2: Enroll in Developer Program

1. Visit https://developer.apple.com/programs/enroll/
2. Sign in with your Apple ID
3. Choose enrollment type:
   - **Individual**: For solo developers
   - **Organization**: For companies (requires D-U-N-S verification)
4. Complete payment ($99/year)
5. Agree to license agreements

### Step 3: Verify Enrollment

After enrollment approval (may take up to 48 hours):
1. Sign in to https://developer.apple.com/account/
2. Confirm "Developer" status in top navigation
3. Access Certificates, IDs & Profiles section

## Certificate Types

### Developer ID Application

**Purpose**: Signing software for distribution outside the Mac App Store.

**Use Case**: Primary certificate for BULLsEYE plugin distribution.

**Characteristics:**
- Identifies you as the developer by name
- Allows Gatekeeper approval
- Required for notarization
- Valid for distribution to users

**Obtaining:**
1. In Developer Portal, navigate to Certificates > +
2. Select "Developer ID Application"
3. Generate Certificate Signing Request (CSR)
4. Download and install certificate

### Mac App Store Certificate

**Purpose**: Signing software for distribution through Mac App Store.

**Use Case**: Not applicable for typical plugin distribution.

**Characteristics:**
- Requires app sandboxing
- Limited DAW compatibility
- Additional entitlements required

**Note**: This certificate type is not recommended for audio plugins due to sandbox restrictions that may interfere with DAW integration.

### Development Certificate

**Purpose**: Testing during development.

**Use Case**: Local development and debugging.

**Characteristics:**
- Easier to obtain
- Not valid for distribution
- Good for initial testing

## Obtaining Certificates

### Step 1: Create Certificate Signing Request (CSR)

1. Open Keychain Access application
2. Select Keychain > System
3. Navigate to Keychain Access > Certificate Assistant > Request a Certificate from a Certificate Authority
4. Enter:
   - **User Email Address**: Your Apple Developer email
   - **Common Name**: Your name or company name
   - **Request**: Save to disk
5. Save the CSR file to your desktop

### Step 2: Generate Certificate in Developer Portal

1. Navigate to https://developer.apple.com/account/resources/certificates/add
2. Select certificate type:
   - **Developer ID Application** (recommended for distribution)
   - **Apple Development** (for testing)
3. Upload your CSR file
4. Download the generated certificate (.cer file)

### Step 3: Install Certificate

1. Double-click the downloaded .cer file
2. Keychain Access will open
3. Select **Login** keychain
4. Click Add
5. Verify certificate appears in My Certificates

### Step 4: Export Certificate for Signing

For use in build scripts, export as Personal Information Exchange (.p12):

1. In Keychain Access, find your certificate
2. Expand the certificate to show private key
3. Select both certificate and private key
4. Right-click and select Export 2 items
5. Choose .p12 format
6. Set a password (you will need this for signing)
7. Save to secure location

**Security Note**: Never commit .p12 files to version control. Add to .gitignore.

## Keychain Setup

### Default Keychain

For most signing operations, the login keychain is sufficient:
```bash
# Verify certificates are accessible
security find-identity -p codesigning -v
```

### System Keychain

For certain deployment scenarios, certificates may need to be in the system keychain:

1. Open Keychain Access
2. Select System keychain
3. Import certificate and private key
4. Lock and unlock the keychain to apply changes

**Note**: Requires administrator privileges.

### Keychain Access for Build Tools

Build tools must have access to your signing certificate. Ensure Keychain Access settings allow applications to access certificates:

1. Right-click certificate in Keychain Access
2. Select Get Info
3. Under Access tab, ensure build tools have access
4. Add Terminal or Xcode if needed

## Code Signing Process

### Manual Signing

For AU component:
```bash
# Replace CERTIFICATE_NAME with your certificate name or hash
codesign --force --sign "Certificate Name" --deep --timestamp --options=runtime ~/Library/Audio/Plug-Ins/Components/BULLsEYE.component
```

For VST3 component:
```bash
codesign --force --sign "Certificate Name" --deep --timestamp --options=runtime ~/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3
```

### Using the Provided Script

The `build/sign_plugin.sh` script automates the signing process:

```bash
# Make script executable
chmod +x build/sign_plugin.sh

# Sign with certificate name
./build/sign_plugin.sh "Developer ID Application: Your Name"

# Sign with certificate hash
./build/sign_plugin.sh 1B2C3D4E5F67890

# Sign and verify
./build/sign_plugin.sh "Certificate Name" --verify

# Verbose output
./build/sign_plugin.sh "Certificate Name" --verbose
```

### Certificate Name Formats

The script accepts various certificate identification formats:

1. **Full Name**: `codesign --sign "Developer ID Application: Your Name"`
2. **Common Name**: `codesign --sign "Your Name"`
3. **Team ID**: `codesign --sign "YOURTEAMID1234"`
4. **SHA-1 Hash**: `codesign --sign 1B2C3D4E5F67890`

Find your certificate hash:
```bash
security find-identity -p codesigning -v
```

## Verification

### Check Signature

```bash
# Verify a signed component
codesign --verify --verbose ~/Library/Audio/Plug-Ins/Components/BULLsEYE.component
codesign --verify --verbose ~/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3
```

### Display Signature Details

```bash
# Get detailed signature information
codesign --display --verbose=2 ~/Library/Audio/Plug-Ins/Components/BULLsEYE.component
```

### Gatekeeper Test

1. Move signed plugin to a new location
2. Attempt to open in DAW
3. Check System Settings > Privacy & Security for approval

### Notarization Status

After notarization, check status:
```bash
xcrun altool --notarization-info <RequestUUID> -u appleid@email.com -p app-password
```

## Troubleshooting

### Certificate Not Found

**Error**: `sec_keychain_search_legacy: The specified item could not be found in the keychain.`

**Solutions**:
1. Verify certificate is installed in login keychain
2. Check certificate has not expired
3. Ensure certificate is not revoked
4. Run `security list-keychains` to verify keychain list

### Multiple Certificates

**Error**: `ambiguous name`

**Solutions**:
1. Use full certificate name with team prefix
2. Use SHA-1 hash instead of name
3. Delete duplicate certificates from keychain

### Private Key Missing

**Error**: `CSSMERR_TP_CERT_EXPIRED` or signature fails

**Solutions**:
1. Export certificate with private key as .p12
2. Re-import to ensure private key is present
3. Request new certificate if private key was lost

### Deep Signing Requirements

**Error**: `code object is not signed at all`

**Solutions**:
1. Use `--deep` flag with codesign
2. Ensure all nested bundles are signed
3. Check for unsigned frameworks or resources

### Timestamp Server Issues

**Error**: `timestamp service unavailable`

**Solutions**:
1. Try with `--timestamp=none` for offline signing
2. Verify internet connectivity
3. Use Apple timestamp server: `http://timestamp.apple.com/ts01`

### Code Signing for Testing

During development, use ad-hoc signing for testing:

```bash
# Ad-hoc signing (no certificate required)
codesign --force --sign - ~/Library/Audio/Plug-Ins/Components/BULLsEYE.component
```

**Note**: Ad-hoc signatures are not valid for distribution but sufficient for local testing.

### Renewal Before Expiration

Certificates expire after one year. To renew:

1. Generate new CSR in Developer Portal
2. Create new certificate (do not revoke old one immediately)
3. Test new certificate before revoking old one
4. Update build scripts with new certificate name
5. Notify users of update if binaries are redistributed

## Additional Resources

- [Apple Developer Code Signing Guide](https://developer.apple.com/library/archive/documentation/Security/Conceptual/CodeSigningGuide/Introduction/Introduction.html)
- [Developer ID Program](https://developer.apple.com/developer-id/)
- [Notarizing Your Software](https://developer.apple.com/documentation/xcode/notarizing_your_software_before_distribution)
- [Keychain Access Help](https://support.apple.com/keychain-access)
- [Certificate Revocation](https://developer.apple.com/account/resources/certificates/revoke)

## Quick Reference

```bash
# Check environment
xcode-select --version
security find-identity -p codesigning -v

# Manual signing (replace CERTIFICATE_NAME)
codesign --force --sign "CERTIFICATE_NAME" --deep --timestamp --options=runtime ~/Library/Audio/Plug-Ins/Components/BULLsEYE.component
codesign --force --sign "CERTIFICATE_NAME" --deep --timestamp --options=runtime ~/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3

# Verification
codesign --verify --verbose ~/Library/Audio/Plug-Ins/Components/BULLsEYE.component
codesign --verify --verbose ~/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3

# Ad-hoc signing for testing
codesign --force --sign - --deep ~/Library/Audio/Plug-Ins/Components/BULLsEYE.component
```
