# Portable License Drop-In Integration Guide

## Overview

This guide provides detailed instructions for integrating the Portable License Drop-In Kit with audio plugins, with special focus on extracting and reusing implementations from the parent AnalogMorphV333 project.

## Integration Approaches

### 1. Simple Drop-In (Recommended for New Projects)

For new audio plugins that don't need to reuse existing license implementations:

#### Code Example

```cpp
#include "portable-license-drop-in/core/LICENSE_ENGINE.hpp"

class MyPlugin : public juce::AudioProcessor {
private:
    LicenseEngine::LicenseEngine licenseEngine;
    
public:
    MyPlugin() {
        // Configure license engine
        LicenseEngine::LicenseConfig config;
        config.companyName = "MyCompany";
        config.productName = "MyPlugin";
        config.productVersion = "1.0.0";
        config.trialDays = 14;               // Minimum 14 days for pro audio
        config.subscriptionGracePeriodDays = 7;
        config.allowOfflineActivation = true;
        
        // Initialize license engine
        licenseEngine.initialize(config);
    }
    
    void processBlock(juce::AudioBuffer<float>& buffer, 
                       juce::MidiBuffer& midiMessages) override {
        // Thread-safe license check following laws/07_AUDIO_THREAD_SAFETY.md
        const auto isLicensed = licenseEngine.isLicensed();
        
        if (!isLicensed) {
            buffer.clear(); // Or apply trial restrictions
            return;
        }
        
        // Normal audio processing continues...
        // Your audio processing code here
    }
    
    // UI integration would be in the editor class
    juce::AudioProcessorEditor* createEditor() override {
        return new MyPluginEditor(licenseEngine);
    }
};
```

### 2. Source Extraction (For Integration with AnalogMorphV333)

To reuse the exact license implementations from the parent AnalogMorphV333 project:

#### Code Example

```cpp
#include "portable-license-drop-in/core/LICENSE_ENGINE_EXTRACTOR.hpp"
#include "portable-license-drop-in/examples/AnalogMorphV3Plugin/AnalogMorphV3PluginExample.cpp"

class MyPlugin : public juce::AudioProcessor {
private:
    // License engine with source extraction
    std::unique_ptr<LicenseEngine::LicenseEngineWithExtraction> licenseEngine;
    
public:
    MyPlugin() {
        // Configure extractor for AnalogMorphV333 source
        auto extractionConfig = LicenseEngine::DefaultConfig::createAnalogMorphConfig();
        
        // Create license engine with extraction support
        licenseEngine = std::make_unique<LicenseEngine::LicenseEngineWithExtraction>(extractionConfig);
        
        // Initialize with your product configuration
        LicenseEngine::LicenseConfig config;
        config.companyName = "MyCompany";
        config.productName = "MyPlugin";
        config.productVersion = "1.0.0";
        config.trialDays = 14;
        
        licenseEngine->initialize(config);
    }
    
    void processBlock(juce::AudioBuffer<float>& buffer, 
                       juce::MidiBuffer& midiMessages) override {
        // Thread-safe license check
        if (!licenseEngine->isLicensed()) {
            buffer.clear();
            return;
        }
        
        // Normal audio processing continues...
    }
    
    juce::AudioProcessorEditor* createEditor() override {
        return new MyPluginEditor(*licenseEngine);
    }
};
```

### 3. Factored Implementation (For Maximum Flexibility)

For maximum flexibility and dependency injection:

#### Code Example

```cpp
#include "portable-license-drop-in/core/LICENSE_ENGINE_FACTORIZED.hpp"

class MyPlugin : public juce::AudioProcessor {
private:
    // Factored license engine with dependency injection
    std::unique_ptr<LicenseEngine::FactoredLicenseEngine> licenseEngine;
    
public:
    MyPlugin() {
        // Create factory with AnalogMorphV333 implementations
        auto analogMorphFactory = LicenseEngine::PreconfiguredImplementations::createAnalogMorphFactory();
        
        // Configure engine builder
        auto engine = LicenseEngine::LicenseEngineBuilder()
                .withFactory(analogMorphFactory)
                .withStorage("FileBasedStorage")
                .withValidator("NetworkValidator")
                .withNetworkClient("AnalogMorphNetwork")
                .withUI("AnalogMorphUI");
        
        // Initialize with your product configuration
        LicenseEngine::LicenseConfig config;
        config.companyName = "MyCompany";
        config.productName = "MyPlugin";
        config.productVersion = "1.0.0";
        config.trialDays = 14;
        
        licenseEngine = engine.build(config);
    }
    
    void processBlock(juce::AudioBuffer<float>& buffer, 
                       juce::MidiBuffer& midiMessages) override {
        if (!licenseEngine->isLicensed()) {
            buffer.clear();
            return;
        }
        
        // Normal audio processing continues...
    }
    
    juce::AudioProcessorEditor* createEditor() override {
        return new MyPluginEditor(*licenseEngine);
    }
};
```

## UI Integration

### License Status Display

```cpp
class MyPluginEditor : public juce::AudioProcessorEditor {
private:
    // Reference to license engine
    LicenseEngine::FactoredLicenseEngine& licenseEngine;
    
    // UI components
    juce::Label licenseStatusLabel;
    juce::TextButton activateButton {"Activate License"};
    juce::Timer statusUpdateTimer;
    
public:
    MyPluginEditor(LicenseEngine::FactoredLicenseEngine& engine)
        : AudioProcessorEditor(), licenseEngine(engine) {
        // UI setup
        addAndMakeVisible(licenseStatusLabel);
        addAndMakeVisible(activateButton);
        
        // Start periodic status updates
        statusUpdateTimer.startTimerHz(1);
    }
    
    void paint(juce::Graphics& g) override {
        g.fillAll(juce::Colours::lightgrey);
    }
    
    void resized() override {
        auto area = getLocalBounds();
        
        licenseStatusLabel.setBounds(10, 10, area.getWidth() - 20, 30);
        activateButton.setBounds(10, 50, area.getWidth() - 20, 30);
    }
    
    void timerCallback() override {
        updateLicenseStatus();
    }
    
    void updateLicenseStatus() {
        // Get license status from engine
        juce::String statusText = licenseEngine.getLicenseStatusMessage();
        
        // Update UI
        licenseStatusLabel.setText(statusText, juce::dontSendNotification);
        
        // Show/hide activate button based on license status
        activateButton.setVisible(!licenseEngine.isLicensed());
    }
    
private:
    void handleActivationButton() {
        // Show activation dialog
        licenseEngine.showActivationDialog();
    }
};
```

### License Activation Dialog

```cpp
class LicenseActivationDialog : public juce::DialogWindow {
private:
    LicenseEngine::FactoredLicenseEngine& licenseEngine;
    juce::TextEditor licenseKeyEditor;
    juce::TextButton activateButton {"Activate"};
    juce::TextButton cancelButton {"Cancel"};
    
public:
    LicenseActivationDialog(LicenseEngine::FactoredLicenseEngine& engine)
        : DialogWindow("License Activation", juce::Colours::lightgrey, true)
        , licenseEngine(engine) {
        // Dialog setup
        setContentOwned(new LicenseActivationComponent(*this), true);
        centreWithSize(400, 200);
        setResizable(false);
        setUsingNativeTitleBar(true);
    }
    
private:
    class LicenseActivationComponent : public juce::Component {
    private:
        LicenseActivationDialog& owner;
        
    public:
        LicenseActivationComponent(LicenseActivationDialog& dialog) : owner(dialog) {
            addAndMakeVisible(licenseKeyEditor);
            addAndMakeVisible(activateButton);
            addAndMakeVisible(cancelButton);
        }
        
        void resized() override {
            auto area = getLocalBounds().reduced(10);
            
            licenseKeyEditor.setBounds(area.removeFromTop(30));
            
            auto buttonArea = area.removeFromTop(40);
            activateButton.setBounds(buttonArea.removeFromLeft(buttonArea.getWidth() / 2 - 5));
            cancelButton.setBounds(buttonArea);
        }
    };
};
```

## Migration from Existing License Systems

### Interface Compatibility

```cpp
// Adapter for existing license system
class ExistingLicenseAdapter : public LicenseEngine::ILicenseStorage {
private:
    ExistingLicenseSystem* legacySystem;
    
public:
    explicit ExistingLicenseAdapter(ExistingLicenseSystem* legacy) 
        : legacySystem(legacy) {}
    
    bool loadLicense(LicenseEngine::LicenseInfo& license) override {
        // Convert between data structures
        LegacyLicenseData legacyData;
        convertToLegacyFormat(license, legacyData);
        
        // Call existing implementation
        bool result = legacySystem->loadLicense(legacyData);
        
        // Convert result back
        convertFromLegacyFormat(legacyData, license);
        return result;
    }
};
```

### Data Migration

```cpp
// Migrate existing license data
bool migrateLegacyLicense(const std::string& legacyPath) {
    // Load existing license
    LegacyLicenseData legacyData;
    if (!loadLegacyLicense(legacyPath, legacyData)) {
        return false;
    }
    
    // Convert to new format
    LicenseEngine::LicenseInfo newLicense;
    if (!convertLicenseFormat(legacyData, newLicense)) {
        return false;
    }
    
    // Save in new format
    LicenseEngine::LicenseEngine engine;
    if (!engine.activateLicense(newLicense.getLicenseKey())) {
        return false;
    }
    
    return true;
}
```

## Testing Strategy

### Unit Testing

```cpp
// Test license engine with mocked dependencies
class MockLicenseStorage : public LicenseEngine::ILicenseStorage {
public:
    bool loadLicense(LicenseEngine::LicenseInfo& license) override { return true; }
    bool saveLicense(const LicenseEngine::LicenseInfo& license) override { return true; }
};

TEST_CASE("License Engine Initialization", "[license]") {
    auto mockStorage = std::make_unique<MockLicenseStorage>();
    auto engine = LicenseEngine::LicenseEngineBuilder()
                .withStorage(mockStorage)
                .build(validConfig);
    
    REQUIRE(engine.initialize(validConfig));
    REQUIRE(engine.isLicensed() == false); // Should start in trial
}
```

### Integration Testing

```cpp
// Test license integration in actual plugin context
void testLicenseIntegration() {
    // Create plugin instance
    TestPlugin plugin;
    
    // Initialize with different license states
    testWithTrialLicense(plugin);
    testWithValidLicense(plugin);
    testWithExpiredLicense(plugin);
    
    // Verify audio thread safety
    testThreadSafety(plugin);
}
```

## Platform-Specific Considerations

### Windows

```cpp
// Windows-specific license storage
class WindowsLicenseStorage : public LicenseEngine::ILicenseStorage {
private:
    std::string getRegistryPath() const {
        return "SOFTWARE\\MyCompany\\MyPlugin\\License";
    }
    
public:
    bool saveLicense(const LicenseEngine::LicenseInfo& license) override {
        // Use Windows Registry API
        HKEY key;
        if (RegOpenKeyExA(HKEY_CURRENT_USER, getRegistryPath().c_str(), 
                         0, KEY_WRITE, &key) != ERROR_SUCCESS) {
            return false;
        }
        
        // Save license data
        RegSetValueExA(key, "LicenseKey", REG_SZ, 
                        license.licenseKey.c_str(), license.licenseKey.length() + 1);
        RegSetValueExA(key, "ExpiryDate", REG_SZ, 
                        license.expiryDate.c_str(), license.expiryDate.length() + 1);
        
        RegCloseKey(key);
        return true;
    }
};
```

### macOS

```cpp
// macOS-specific license storage using Keychain
class MacOSLicenseStorage : public LicenseEngine::ILicenseStorage {
public:
    bool saveLicense(const LicenseEngine::LicenseInfo& license) override {
        // Convert license data to string
        std::string licenseData = serializeLicense(license);
        
        // Save to Keychain
        OSStatus status = SecKeychainAddGenericPassword(
            nullptr, // keychain
            "MyPlugin", // service
            "MyCompany", // account
            licenseData.length(), // data length
            licenseData.c_str(), // data
            nullptr // item reference
        );
        
        return status == errSecSuccess;
    }
};
```

### Linux

```cpp
// Linux-specific license storage
class LinuxLicenseStorage : public LicenseEngine::ILicenseStorage {
private:
    std::string getConfigPath() const {
        // Get config directory
        const char* home = std::getenv("HOME");
        if (!home) return "~/.config/MyPlugin";
        return std::string(home) + "/.config/MyPlugin";
    }
    
public:
    bool saveLicense(const LicenseEngine::LicenseInfo& license) override {
        // Save to config file
        std::ofstream licenseFile(getConfigPath() + "/license.dat");
        licenseFile << serializeLicense(license);
        return licenseFile.good();
    }
};
```

## Troubleshooting

### Common Issues

1. **Compilation Errors**:
   - Verify include paths are correct
   - Check for missing dependencies
   - Ensure namespace usage is consistent

2. **Runtime Issues**:
   - Verify license file paths are accessible
   - Check for permission issues with storage
   - Ensure thread safety of license checks

3. **Integration Issues**:
   - Verify initialization sequence is correct
   - Check for memory leaks in license code
   - Ensure UI updates happen on correct thread

### Debug Mode

```cpp
// Enable debug mode for license engine
#ifdef DEBUG_LICENSE_ENGINE
#define LICENSE_DEBUG_LOG(msg) juce::Logger::writeToLog("License: " + msg)
#else
#define LICENSE_DEBUG_LOG(msg)
#endif

// Usage in license engine
LICENSE_DEBUG_LOG("Initializing license engine");
```

## Best Practices

1. **Follow Parent Project Laws**:
   - Audio Thread Safety: laws/07_AUDIO_THREAD_SAFETY.md
   - Time & Lifecycle: laws/06_TIME_LIFECYCLE.md
   - License Drop-In: laws/04_LICENSE_DROP_IN.md

2. **Maintain Compatibility**:
   - Keep interfaces stable
   - Provide migration paths
   - Document breaking changes

3. **Test Thoroughly**:
   - Test all license states
   - Test with different platforms
   - Verify thread safety

4. **Document Decisions**:
   - Record integration approach choices
   - Document any modifications to license engine
   - Keep integration guide updated

## Deployment Checklist

- [ ] Choose integration approach (simple, extraction, or factored)
- [ ] Configure include paths correctly
- [ ] Initialize license engine in plugin constructor
- [ ] Add license checks to audio processing thread
- [ ] Integrate license UI in plugin editor
- [ ] Test with different license states
- [ ] Verify license persistence across restarts
- [ ] Test on all target platforms
- [ ] Follow all laws from parent project

For complete examples, see the `examples/` directory.