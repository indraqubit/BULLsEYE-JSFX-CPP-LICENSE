/*
  ==============================================================================
  Example: Plugin with AnalogMorphV333 Source Extraction
  
  This example shows how to extract and reuse the exact
  license implementation from the parent AnalogMorphV333 project.
  
  Key concepts demonstrated:
  1. Extract license components from parent source
  2. Wrap existing implementations to match interfaces
  3. Maintain all thread safety and lifecycle laws
  4. Zero-code reuse of proven license functionality
  ==============================================================================
*/

#include "JucePluginMain.h"
#include "portable-license-drop-in/core/LICENSE_ENGINE_EXTRACTOR.hpp"

// Include paths for parent project source
#ifdef _WIN32
#include "../../AnalogMorphV333/Source/LicenseStorage/LicenseStorage.h"
#include "../../AnalogMorphV333/Source/LicenseStorage/FileStorage.h"
#include "../../AnalogMorphV333/Source/LicenseSystem/LicenseValidation.h"
#include "../../AnalogMorphV333/Source/LicenseSystem/NetworkValidator.h"
#else
#include "../../Source/LicenseStorage/LicenseStorage.h"
#include "../../Source/LicenseStorage/FileStorage.h"
#include "../../Source/LicenseSystem/LicenseValidation.h"
#include "../../Source/LicenseSystem/NetworkValidator.h"
#endif

// Adapter classes to wrap existing implementations
namespace AnalogMorphV3 {
    
    // Adapter for existing storage implementation
    class StorageAdapter : public LicenseEngine::ILicenseStorage {
    private:
        std::unique_ptr<AnalogMorphV333::LicenseStorage> impl_;
        
    public:
        explicit StorageAdapter()
            : impl_(std::make_unique<AnalogMorphV333::FileStorage>()) {}
        
        bool loadLicense(LicenseEngine::LicenseInfo& license) override {
            // Convert to AnalogMorphV333 format
            AnalogMorphV333::LicenseData srcData;
            if (!convertToSourceFormat(license, srcData)) {
                return false;
            }
            
            // Use existing implementation
            bool result = impl_->loadLicense(srcData);
            
            // Convert result back
            convertFromSourceFormat(srcData, license);
            return result;
        }
        
        bool saveLicense(const LicenseEngine::LicenseInfo& license) override {
            // Convert to AnalogMorphV333 format
            AnalogMorphV333::LicenseData srcData;
            if (!convertToSourceFormat(license, srcData)) {
                return false;
            }
            
            // Use existing implementation
            bool result = impl_->saveLicense(srcData);
            
            // Convert result back
            convertFromSourceFormat(srcData, license);
            return result;
        }
        
        bool deleteLicense() override {
            return impl_->deleteLicense();
        }
        
        bool licenseFileExists() override {
            return impl_->licenseFileExists();
        }
        
        std::string getLicenseFilePath() const override {
            return impl_->getLicenseFilePath();
        }
        
    private:
        // Convert between Portable and AnalogMorphV333 license formats
        bool convertToSourceFormat(const LicenseEngine::LicenseInfo& portable, 
                                AnalogMorphV333::LicenseData& source) {
            // Implementation would convert fields appropriately
            source.productId = portable.productId;
            source.userId = portable.userId;
            source.machineId = portable.machineId;
            
            // Convert time points
            source.activationDate = convertToSourceTime(portable.activationDate);
            source.expiryDate = convertToSourceTime(portable.expiryDate);
            
            // Convert license type
            source.type = convertToSourceType(portable.type);
            source.status = convertToSourceStatus(portable.status);
            
            return true;
        }
        
        void convertFromSourceFormat(const AnalogMorphV333::LicenseData& source,
                                LicenseEngine::LicenseInfo& portable) {
            // Implementation would convert fields appropriately
            portable.productId = source.productId;
            portable.userId = source.userId;
            portable.machineId = source.machineId;
            
            // Convert time points
            portable.activationDate = convertFromSourceTime(source.activationDate);
            portable.expiryDate = convertFromSourceTime(source.expiryDate);
            
            // Convert license type
            portable.type = convertFromSourceType(source.type);
            portable.status = convertFromSourceStatus(source.status);
            
            // Convert enabled features
            portable.enabledFeatures = source.enabledFeatures;
        }
        
        // Time conversion helpers
        AnalogMorphV333::Timestamp convertToSourceTime(const std::chrono::system_clock::time_point& time) {
            // Convert to source time format
            auto epoch = std::chrono::system_clock::to_time_t(time);
            return AnalogMorphV333::Timestamp{static_cast<uint64_t>(epoch)};
        }
        
        std::chrono::system_clock::time_point convertFromSourceTime(const AnalogMorphV333::Timestamp& time) {
            // Convert from source time format
            return std::chrono::system_clock::from_time_t(
                static_cast<std::time_t>(time.epochSeconds));
        }
        
        // Type conversion helpers
        AnalogMorphV333::LicenseType convertToSourceType(const LicenseEngine::LicenseType& type) {
            switch (type) {
                case LicenseEngine::LicenseType::Trial:
                    return AnalogMorphV333::LicenseType::Trial;
                case LicenseEngine::LicenseType::Subscription:
                    return AnalogMorphV333::LicenseType::Subscription;
                case LicenseEngine::LicenseType::Perpetual:
                    return AnalogMorphV333::LicenseType::Perpetual;
                case LicenseEngine::LicenseType::FeatureLimited:
                    return AnalogMorphV333::LicenseType::FeatureLimited;
                default:
                    return AnalogMorphV333::LicenseType::Trial;
            }
        }
        
        LicenseEngine::LicenseType convertFromSourceType(const AnalogMorphV333::LicenseType& type) {
            switch (type) {
                case AnalogMorphV333::LicenseType::Trial:
                    return LicenseEngine::LicenseType::Trial;
                case AnalogMorphV333::LicenseType::Subscription:
                    return LicenseEngine::LicenseType::Subscription;
                case AnalogMorphV333::LicenseType::Perpetual:
                    return LicenseEngine::LicenseType::Perpetual;
                case AnalogMorphV333::LicenseType::FeatureLimited:
                    return LicenseEngine::LicenseType::FeatureLimited;
                default:
                    return LicenseEngine::LicenseType::Trial;
            }
        }
        
        // Status conversion helpers
        AnalogMorphV333::LicenseStatus convertToSourceStatus(const LicenseEngine::LicenseStatus& status) {
            switch (status) {
                case LicenseEngine::LicenseStatus::Valid:
                    return AnalogMorphV333::LicenseStatus::Valid;
                case LicenseEngine::LicenseStatus::Expired:
                    return AnalogMorphV333::LicenseStatus::Expired;
                case LicenseEngine::LicenseStatus::Invalid:
                    return AnalogMorphV333::LicenseStatus::Invalid;
                case LicenseEngine::LicenseStatus::Missing:
                    return AnalogMorphV333::LicenseStatus::Missing;
                case LicenseEngine::LicenseStatus::PendingActivation:
                    return AnalogMorphV333::LicenseStatus::PendingActivation;
                default:
                    return AnalogMorphV333::LicenseStatus::Missing;
            }
        }
        
        LicenseEngine::LicenseStatus convertFromSourceStatus(const AnalogMorphV333::LicenseStatus& status) {
            switch (status) {
                case AnalogMorphV333::LicenseStatus::Valid:
                    return LicenseEngine::LicenseStatus::Valid;
                case AnalogMorphV333::LicenseStatus::Expired:
                    return LicenseEngine::LicenseStatus::Expired;
                case AnalogMorphV333::LicenseStatus::Invalid:
                    return LicenseEngine::LicenseStatus::Invalid;
                case AnalogMorphV333::LicenseStatus::Missing:
                    return LicenseEngine::LicenseStatus::Missing;
                case AnalogMorphV333::LicenseStatus::PendingActivation:
                    return LicenseEngine::LicenseStatus::PendingActivation;
                default:
                    return LicenseEngine::LicenseStatus::Missing;
            }
        }
    };
    
    // Adapter for existing validation implementation
    class ValidatorAdapter : public LicenseEngine::ILicenseValidator {
    private:
        std::unique_ptr<AnalogMorphV333::NetworkValidator> impl_;
        
    public:
        explicit ValidatorAdapter()
            : impl_(std::make_unique<AnalogMorphV333::NetworkValidator>()) {}
        
        bool validateKey(const std::string& key, LicenseEngine::LicenseInfo& license) override {
            // Convert to AnalogMorphV333 format
            AnalogMorphV333::LicenseValidationRequest request;
            request.licenseKey = key;
            request.machineId = getMachineId(); // Get from parent implementation
            
            // Use existing validation
            AnalogMorphV333::LicenseValidationResult result = impl_->validateLicense(request);
            
            // Convert result back
            if (result.isValid) {
                license.type = LicenseEngine::LicenseType::Perpetual;
                license.status = LicenseEngine::LicenseStatus::Valid;
                license.productId = "AnalogMorphV3";
                license.activationDate = std::chrono::system_clock::now();
                license.expiryDate = std::chrono::system_clock::time_point{};
                
                // Convert features
                for (const auto& feature : result.enabledFeatures) {
                    license.enabledFeatures.push_back(feature);
                }
            }
            
            return result.isValid;
        }
        
        bool validateLicense(const LicenseEngine::LicenseInfo& license) override {
            // Convert to AnalogMorphV333 format for validation
            AnalogMorphV333::LicenseData srcData;
            if (!convertToSourceFormat(license, srcData)) {
                return false;
            }
            
            // Use existing validation
            auto result = impl_->validateLicense(srcData);
            
            return result.isValid;
        }
        
        bool isKeyFormatValid(const std::string& key) override {
            return impl_->isKeyFormatValid(key);
        }
        
        LicenseEngine::ValidationResult getValidationResult(const std::string& key) override {
            auto result = impl_->validateLicense(key);
            
            // Convert to common format
            LicenseEngine::ValidationResult commonResult;
            commonResult.isValid = result.isValid;
            commonResult.errorMessage = result.errorMessage;
            
            return commonResult;
        }
        
    private:
        std::string getMachineId() {
            // Reuse machine ID generation from parent
            return AnalogMorphV333::getMachineId();
        }
    };
}

// Plugin using extracted license components
class AnalogMorphV3Plugin : public juce::AudioProcessor
{
private:
    // License engine with source extraction
    LicenseEngine::LicenseEngineWithExtraction licenseEngine;
    
    // UI state for license status
    bool licenseDialogVisible = false;
    
public:
    AnalogMorphV3Plugin()
        : AudioProcessor (BusesProperties()
                       #if ! JucePlugin_IsMidiEffect
                        #if ! JucePlugin_IsSynth
                         .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                       #endif
                         .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                       #endif
                       )
    {
    // Configure extractor for AnalogMorphV333 source
        LicenseEngine::SourceExtractionConfig extractionConfig = 
            LicenseEngine::DefaultConfig::createAnalogMorphConfig();
        
        // Create license engine with extraction support
        licenseEngine = LicenseEngine::LicenseEngineWithExtraction(extractionConfig);
        
        // Initialize with product configuration
        LicenseEngine::LicenseConfig config;
        config.companyName = "AnalogMorphV3";
        config.productName = "AnalogMorphV3";
        config.productVersion = "1.0.0";
        config.trialDays = 14; // Following laws/06_TIME_LIFECYCLE.md
        config.subscriptionGracePeriodDays = 7;
        config.allowOfflineActivation = true;
        config.requireNetworkActivation = false;
        
        licenseEngine.initialize(config);
    }

    ~AnalogMorphV3Plugin() override {}

    void prepareToPlay (double sampleRate, int samplesPerBlock) override
    {
        // License status refresh before processing
        licenseEngine.refreshLicenseStatus();
        
        AudioProcessor::prepareToPlay (sampleRate, samplesPerBlock);
    }

    void releaseResources() override
    {
        AudioProcessor::releaseResources();
    }

    void processBlock (juce::AudioBuffer<float>& buffer, 
                       juce::MidiBuffer& midiMessages) override
    {
        juce::ScopedNoDenormals noDenormals;
        
        // Thread-safe license check following laws/07_AUDIO_THREAD_SAFETY.md
        const auto isLicensed = licenseEngine.isLicensed();
        
        // Law 3: Enforcement: deterministic branch only
        if (!isLicensed) {
            buffer.clear();
            return;
        }
        
        // Normal audio processing when licensed
        // ... your audio processing code here
    }

    const juce::String getName() const override { return "AnalogMorphV3"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    //==============================================================================
    // Audio Processor Editor
    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override
    {
        // Save license state if needed
        // ...
    }

    void setStateInformation (const void* data, int sizeInBytes) override
    {
        // Restore license state if needed
        // ...
    }
    
    // License-specific UI methods
    bool hasLicenseDialog() const override { return licenseDialogVisible; }
    
    void showLicenseActivationDialog()
    {
        licenseEngine.showActivationDialog();
        licenseDialogVisible = true;
    }
    
    void hideLicenseDialog()
    {
        licenseEngine.hideAllDialogs();
        licenseDialogVisible = false;
    }
    
    std::string getLicenseStatusMessage() const
    {
        return licenseEngine.getLicenseStatusMessage();
    }
    
    bool isPluginLicensed() const
    {
        return licenseEngine.isLicensed();
    }
    
    int getTrialDaysRemaining() const
    {
        return licenseEngine.daysRemaining();
    }
    
    LicenseEngine::LicenseInfo getCurrentLicense() const
    {
        return licenseEngine.getCurrentLicense();
    }
    
    bool activateLicense(const std::string& licenseKey)
    {
        return licenseEngine.activateLicense(licenseKey);
    }
    
    bool deactivateLicense()
    {
        return licenseEngine.deactivateLicense();
    }
};

// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AnalogMorphV3Plugin();
}