# Apple Notarization Guide for BULLsEYE

This document provides comprehensive guidance for notarizing BULLsEYE audio plugins for macOS distribution. Notarization is a mandatory requirement for distributing software on macOS Catalina (10.15) and later versions.

## Table of Contents

1. [Overview](#overview)
2. [Why Notarization Matters](#why-notarization-matters)
3. [Prerequisites](#prerequisites)
4. [Apple ID App-Specific Password](#apple-id-app-specific-password)
5. [Notarization Process](#notarization-process)
6. [Stapling](#stapling)
7. [Troubleshooting](#troubleshooting)
8. [Automation](#automation)
9. [Verification](#verification)

## Overview

Apple Notarization is a automated process that scans your software for malicious content and security issues. When users download notarized software, macOS Gatekeeper displays a clear confirmation that Apple has scanned the software and found no issues.

**Notarization Required For:**
- All software distributed outside the Mac App Store
- macOS Catalina (10.15) and later
- Applications, plugins, and executables

**Notarization Does NOT Provide:**
- Code signing (must be done separately)
- Sandboxing (optional)
- App Store distribution (separate process)

## Why Notarization Matters

### User Experience

Without notarization, users encounter:

1. **Gatekeeper Warning**: "Apple could not check [software] for malicious software"
2. **Blocked Launch**: Users must manually approve in System Settings
3. **Reduced Trust**: Users may hesitate to use unnotarized software

With notarization, users see:

1. **Seamless Launch**: Software opens without warnings
2. **Verified Developer**: Confirmation of Apple's scan
3. **Professional Presentation**: Polished user experience

### Distribution Requirements

**Required For:**
- Distributing to users outside your development machine
- Hosting downloads on your website
- Distributing through plugin stores (e.g., Plugin Boutique)
- Sharing via email or cloud storage

**Not Required For:**
- Local development and testing
- Ad-hoc distribution to testers
- Internal company use (may vary by security policy)

## Prerequisites

### Developer Requirements

Before starting notarization:

1. **Apple Developer Account**: Enrolled in Apple Developer Program ($99/year)
2. **Developer ID Certificate**: Obtained from Developer Portal
3. **Software Signed**: Code signing completed (see docs/CODE_SIGNING.md)
4. **Xcode Command Line Tools**: Installed and configured

### Check Current Status

```bash
# Verify Xcode tools
xcode-select --version

# Verify code signing
security find-identity -p codesigning -v

# Check altool availability
xcrun altool --version
```

### Certificate Requirements

Ensure your Developer ID certificate:
- Is installed in your keychain
- Has a corresponding private key
- Has not expired
- Has been used to sign your plugin

```bash
# Verify signed components
codesign --verify --verbose ~/Library/Audio/Plug-Ins/Components/BULLsEYE.component
```

## Apple ID App-Specific Password

For notarization, you must use an App-Specific Password rather than your Apple ID password.

### Why App-Specific Password?

- Security: Limits exposure of your main Apple ID password
- Revocation: Can revoke without affecting your Apple ID
- Tracking: See which applications use your account

### Creating App-Specific Password

1. **Sign in to Apple ID Account**
   - Visit https://appleid.apple.com/account/manage
   - Sign in with your Apple ID

2. **Navigate to Security Section**
   - Click "App-Specific Passwords"
   - Or visit https://appleid.apple.com/account/manage/apppasswords

3. **Generate Password**
   - Click "Generate Password"
   - Enter a descriptive name (e.g., "BULLsEYE Notarization")
   - Copy the generated password
   - Store securely (you cannot view it again)

4. **Use Password in Scripts**
   - Store in secure location (Keychain Access, 1Password, etc.)
   - Never commit to version control
   - Use as `--password` argument in altool commands

### Managing App-Specific Passwords

- View active passwords: Apple ID Account > App-Specific Passwords
- Revoke compromised passwords immediately
- Generate new password if forgotten (old one cannot be retrieved)

## Notarization Process

### Step 1: Prepare Your Software

Ensure your plugin is properly code-signed before notarization:

```bash
# Sign AU component
codesign --force --sign "Developer ID Application: Your Name" --deep --timestamp --options=runtime ~/Library/Audio/Plug-Ins/Components/BULLsEYE.component

# Sign VST3 component
codesign --force --sign "Developer ID Application: Your Name" --deep --timestamp --options=runtime ~/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3
```

### Step 2: Upload for Notarization

Use `altool` to submit your software for notarization:

```bash
# Submit AU component
xcrun altool --notarize-app \
    --primary-bundle-id "com.iq.bullseye" \
    --username "your-apple-id@example.com" \
    --password "your-app-specific-password" \
    --file ~/Library/Audio/Plug-Ins/Components/BULLsEYE.component \
    --output-format json
```

```bash
# Submit VST3 component
xcrun altool --notarize-app \
    --primary-bundle-id "com.iq.bullseye.vst3" \
    --username "your-apple-id@example.com" \
    --password "your-app-specific-password" \
    --file ~/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3 \
    --output-format json
```

### Step 3: Save Request UUID

The notarization service returns a Request UUID. Save this:

```json
{
  "notarization-upload": {
    "RequestUUID": "2YOUR-UUID-HERE-ABC123"
  }
}
```

Example:
```bash
# Store UUID for later polling
export UUID="2YOUR-UUID-HERE-ABC123"
```

### Step 4: Check Notarization Status

Wait for Apple to process your submission (typically 5-30 minutes):

```bash
# Check status
xcrun altool --notarization-info "$UUID" \
    --username "your-apple-id@example.com" \
    --password "your-app-specific-password" \
    --output-format json
```

### Step 5: Interpret Status Response

**Successful Response:**
```json
{
  "notarization-info": {
    "RequestUUID": "2YOUR-UUID-HERE-ABC123",
    "Status": "Approved",
    "Status Code": 0,
    "Status Message": "Package Approved",
    "LogFileURL": "https://..."
  }
}
```

**In-Progress Response:**
```json
{
  "notarization-info": {
    "RequestUUID": "2YOUR-UUID-HERE-ABC123",
    "Status": "In Progress",
    "Status Code": 2
  }
}
```

**Failed Response:**
```json
{
  "notarization-info": {
    "RequestUUID": "2YOUR-UUID-HERE-ABC123",
    "Status": "Invalid",
    "Status Code": 1,
    "StatusMessage": "The binary is invalid and cannot be opened.",
    "LogFileURL": "https://..."
  }
}
```

If notarization fails:
1. Download and review the log file
2. Fix identified issues
3. Re-sign and resubmit

## Stapling

Stapling embeds the notarization ticket directly into your software. This allows offline verification and faster first launches.

### Why Staple?

- **Offline Verification**: Users can verify without contacting Apple servers
- **Faster First Launch**: Reduces initial verification time
- **Reliability**: Works even if Apple servers are unreachable
- **Best Practice**: Recommended for all distributed software

### Stapling Process

```bash
# Staple AU component
xcrun stapler staple ~/Library/Audio/Plug-Ins/Components/BULLsEYE.component

# Staple VST3 component
xcrun stapler staple ~/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3
```

### Verify Stapling

```bash
# Check if staple is applied
xcrun stapler validate ~/Library/Audio/Plug-Ins/Components/BULLsEYE.component

# Expected output for valid staple:
# The validate action worked!
```

## Troubleshooting

### Common Issues

#### Upload Fails: "Service Unavailable"

**Cause**: Apple Notarization service is temporarily unavailable.

**Solution**: Wait and retry. Check Apple System Status page.

#### Upload Fails: "Invalid Bundle ID"

**Cause**: Bundle ID does not match Developer Portal registration.

**Solution**: Verify bundle ID in Developer Portal. Add wildcards if needed.

#### Upload Fails: "Code Signature Invalid"

**Cause**: Software is not properly signed or signature is broken.

**Solution**: Re-sign with valid Developer ID certificate.

#### Notarization Fails: "Malicious Software Detected"

**Cause**: Apple detected suspicious code patterns.

**Solution**: Review log file. Remove or refactor flagged code. Contact Apple Developer Support if you believe this is a false positive.

#### Notarization Fails: "Entitlements Issue"

**Cause**: Invalid or missing entitlements.

**Solution**: Review entitlements file. Ensure proper entitlements for your use case.

#### Staple Fails: "No Staple Record Found"

**Cause**: Notarization not completed or failed.

**Solution**: Complete notarization before stapling.

### Checking Logs

Download and review detailed notarization logs:

```bash
# Get log file URL from status response
# Download log
curl -o notarization_log.txt "https://example.com/log/url"

# View log
cat notarization_log.txt
```

### Re-Submitting After Failures

1. **Identify Issue**: Review error messages and log file
2. **Fix Problem**: Correct code or signing issues
3. **Re-Sign**: Sign software with corrected certificate
4. **Re-Upload**: Submit for notarization again
5. **Wait**: Allow processing time

## Automation

### Automated Notarization Script

Create a script to automate the complete process:

```bash
#!/bin/bash
# notarize.sh - Automated notarization script

set -e

# Configuration
APPLE_ID="your-apple-id@example.com"
APP_PASSWORD="your-app-specific-password"
BUNDLE_ID="com.iq.bullseye"
AU_PATH="$HOME/Library/Audio/Plug-Ins/Components/BULLsEYE.component"
VST3_PATH="$HOME/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3"

# Upload for notarization
echo "Submitting AU for notarization..."
AU_RESULT=$(xcrun altool --notarize-app \
    --primary-bundle-id "$BUNDLE_ID" \
    --username "$APPLE_ID" \
    --password "$APP_PASSWORD" \
    --file "$AU_PATH" \
    --output-format json)

AU_UUID=$(echo "$AU_RESULT" | jq -r '.notarization-upload.RequestUUID')
echo "AU Request UUID: $AU_UUID"

echo "Submitting VST3 for notarization..."
VST3_RESULT=$(xcrun altool --notarize-app \
    --primary-bundle-id "$BUNDLE_ID.vst3" \
    --username "$APPLE_ID" \
    --password "$APP_PASSWORD" \
    --file "$VST3_PATH" \
    --output-format json)

VST3_UUID=$(echo "$VST3_RESULT" | jq -r '.notarization-upload.RequestUUID')
echo "VST3 Request UUID: $VST3_UUID"

# Poll for completion (simplified - add proper polling loop)
echo "Waiting for notarization to complete..."
sleep 300  # 5 minutes

# Check status
echo "Checking AU status..."
xcrun altool --notarization-info "$AU_UUID" --username "$APPLE_ID" --password "$APP_PASSWORD"

echo "Checking VST3 status..."
xcrun altool --notarization-info "$VST3_UUID" --username "$APPLE_ID" --password "$APP_PASSWORD"

# Staple (only after approval)
echo "Stapling AU..."
xcrun stapler staple "$AU_PATH"

echo "Stapling VST3..."
xcrun stapler staple "$VST3_PATH"

echo "Notarization complete!"
```

### Environment Variables for CI/CD

Store credentials securely:

```bash
# Set environment variables (do not commit to git)
export APPLE_ID="your-apple-id@example.com"
export APP_PASSWORD="your-app-specific-password"
```

### GitHub Actions Example

```yaml
name: Notarize
on:
  release:
    types: [published]

jobs:
  notarize:
    runs-on: macos-latest
    steps:
      - uses: actions/checkout@v3
      
      - name: Setup Xcode
        run: xcode-select --install
      
      - name: Build Plugin
        run: ./build.sh
      
      - name: Sign Components
        run: |
          ./build/sign_plugin.sh "${{ secrets.CERTIFICATE_NAME }}"
      
      - name: Notarize AU
        run: |
          xcrun altool --notarize-app \
            --primary-bundle-id "com.iq.bullseye" \
            --username "${{ secrets.APPLE_ID }}" \
            --password "${{ secrets.APP_PASSWORD }}" \
            --file "$HOME/Library/Audio/Plug-Ins/Components/BULLsEYE.component"
      
      - name: Staple
        run: |
          xcrun stapler staple "$HOME/Library/Audio/Plug-Ins/Components/BULLsEYE.component"
```

## Verification

### Verify Notarization

```bash
# Check if software is notarized
spctl --assess --type install --verbose ~/Library/Audio/Plug-Components/BULLsEYE.component
spctl --assess --type install --verbose ~/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3
```

Expected output: `source=Notarized Developer ID`

### Verify Staple

```bash
# Check staple status
xcrun stapler validate ~/Library/Audio/Plug-Ins/Components/BULLsEYE.component

# Check signature with staple
codesign --display --verbose=4 ~/Library/Audio/Plug-Ins/Components/BULLsEYE.component
```

### Testing Gatekeeper

1. **Copy to new location**:
   ```bash
   cp -R ~/Library/Audio/Plug-Ins/Components/BULLsEYE.component /tmp/
   ```

2. **Launch from new location**:
   - Open in DAW from new location
   - Verify no Gatekeeper warnings

3. **Test distribution scenario**:
   - Upload to website or file sharing
   - Download to another Mac
   - Verify seamless launch

## Security Best Practices

### Protect Credentials

- **Never commit passwords** to version control
- **Use Keychain** or secure vault for storage
- **Create App-Specific Passwords** for each tool
- **Revoke compromised passwords** immediately
- **Rotate passwords** periodically

### Sign Before Notarize

- Sign with Developer ID certificate
- Verify signature before submitting
- Use `--timestamp` for future validation

### Review Notarization Logs

- Download logs after each notarization
- Address any warnings
- Investigate unknown errors

## Additional Resources

- [Apple Notarization Documentation](https://developer.apple.com/documentation/xcode/notarizing_your_software_before_distribution)
- [Altool Reference](https://help.apple.com/xcode/mac/current/#/devb57b30c8d)
- [Stapler Reference](https://help.apple.com/xcode/mac/current/#/dev8c73a1c5c)
- [Code Signing Guide](docs/CODE_SIGNING.md)
- [Apple System Status](https://developer.apple.com/system-status/)

## Quick Reference

```bash
# Prerequisites check
xcode-select --version
security find-identity -p codesigning -v
xcrun altool --version

# Upload for notarization
xcrun altool --notarize-app \
    --primary-bundle-id "com.iq.bullseye" \
    --username "apple-id@example.com" \
    --password "app-specific-password" \
    --file ~/Library/Audio/Plug-Ins/Components/BULLsEYE.component

# Check status
xcrun altool --notarization-info "UUID" \
    --username "apple-id@example.com" \
    --password "app-specific-password"

# Staple
xcrun stapler staple ~/Library/Audio/Plug-Ins/Components/BULLsEYE.component

# Verify
spctl --assess --type install --verbose ~/Library/Audio/Plug-Ins/Components/BULLsEYE.component
xcrun stapler validate ~/Library/Audio/Plug-Ins/Components/BULLsEYE.component
```
