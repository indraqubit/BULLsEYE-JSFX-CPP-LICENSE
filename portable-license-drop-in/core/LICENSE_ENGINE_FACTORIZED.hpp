/*
  ==============================================================================
  Factored License Engine
  
  Factored version that separates concerns and allows easier
  integration with existing codebases while maintaining drop-in simplicity.
  
  This version uses dependency injection and factory patterns to enable
  flexible implementation selection and easier testing.
  ==============================================================================
*/

#pragma once

#include "LICENSE_ENGINE.hpp"
#include <memory>
#include <string>
#include <functional>
#include <map>
#include <vector>
#include <chrono>
#include <mutex>
#include <atomic>

namespace LicenseEngine {
    
    // Additional types for factored implementation
    struct ValidationResult {
        bool isValid = false;
        std::string errorMessage;
    };
    
    enum class NetworkError {
        None,
        ConnectionFailed,
        Timeout,
        InvalidResponse,
        ServerError
    };
    
    // Forward declarations
    class LicenseStorage;
    class LicenseValidator;
    class LicenseNetworkClient;
    class LicenseUI;
    
    // Pure interfaces for dependency injection
    class ILicenseStorage {
    public:
        virtual ~ILicenseStorage() = default;
        virtual bool loadLicense(LicenseInfo& license) = 0;
        virtual bool saveLicense(const LicenseInfo& license) = 0;
        virtual bool deleteLicense() = 0;
        virtual bool licenseFileExists() = 0;
        virtual std::string getLicenseFilePath() const = 0;
    };
    
    class ILicenseValidator {
    public:
        virtual ~ILicenseValidator() = default;
        virtual bool validateKey(const std::string& key, LicenseInfo& license) = 0;
        virtual bool validateLicense(const LicenseInfo& license) = 0;
        virtual bool isKeyFormatValid(const std::string& key) = 0;
        virtual ValidationResult getValidationResult(const std::string& key) = 0;
    };
    
    class ILicenseNetworkClient {
    public:
        virtual ~ILicenseNetworkClient() = default;
        virtual bool activateOnline(const std::string& key, LicenseInfo& license) = 0;
        virtual bool refreshOnline(LicenseInfo& license) = 0;
        virtual bool deactivateOnline(const std::string& key) = 0;
        virtual bool isOnline() = 0;
        virtual NetworkError getLastError() = 0;
    };
    
    class ILicenseUI {
    public:
        virtual ~ILicenseUI() = default;
        virtual void showActivationDialog() = 0;
        virtual void showStatusMessage(const std::string& message) = 0;
        virtual void showError(const std::string& error) = 0;
        virtual void hideAllDialogs() = 0;
        virtual bool isDialogVisible() = 0;
    };
    
    // Factory for creating implementations
    class ImplementationFactory {
    private:
        std::map<std::string, std::function<std::unique_ptr<void>()>> creators_;
        
    public:
        template<typename T>
        void registerImplementation(const std::string& name) {
            creators_[name] = []() { return std::make_unique<T>(); };
        }
        
        template<typename T>
        std::unique_ptr<T> create(const std::string& name) {
            auto it = creators_.find(name);
            if (it != creators_.end()) {
                auto creator = static_cast<std::function<std::unique_ptr<T>()>>(it->second);
                return static_cast<std::unique_ptr<T>>(creator());
            }
            return nullptr;
        }
        
        std::vector<std::string> getAvailableImplementations() const {
            std::vector<std::string> names;
            for (const auto& pair : creators_) {
                names.push_back(pair.first);
            }
            return names;
        }
    };
    
    // Factored license engine using dependency injection
    class FactoredLicenseEngine {
    private:
        // Dependencies
        std::unique_ptr<ILicenseStorage> storage_;
        std::unique_ptr<ILicenseValidator> validator_;
        std::unique_ptr<ILicenseNetworkClient> network_;
        std::unique_ptr<ILicenseUI> ui_;
        
        // Configuration
        LicenseConfig config_;
        
        // State
        LicenseInfo currentLicense_;
        std::mutex licenseMutex_;
        std::atomic<bool> initialized_{false};
        
    public:
        FactoredLicenseEngine()
            : storage_(nullptr), validator_(nullptr), network_(nullptr), ui_(nullptr) {}
        
        ~FactoredLicenseEngine() = default;
        
        // Dependency injection methods
        void setStorage(std::unique_ptr<ILicenseStorage> storage) {
            storage_ = std::move(storage);
        }
        
        void setValidator(std::unique_ptr<ILicenseValidator> validator) {
            validator_ = std::move(validator);
        }
        
        void setNetworkClient(std::unique_ptr<ILicenseNetworkClient> network) {
            network_ = std::move(network);
        }
        
        void setUI(std::unique_ptr<ILicenseUI> ui) {
            ui_ = std::move(ui);
        }
        
        // Core engine methods
        bool initialize(const LicenseConfig& config) {
            std::lock_guard<std::mutex> lock(licenseMutex_);
            config_ = config;
            
            // Try to load existing license
            if (storage_ && storage_->licenseFileExists()) {
                if (storage_->loadLicense(currentLicense_)) {
                    initialized_ = true;
                    return true;
                }
            }
            
            // Initialize trial license
            currentLicense_.type = LicenseType::Trial;
            currentLicense_.status = LicenseStatus::Valid;
            currentLicense_.productId = config_.productName;
            currentLicense_.activationDate = std::chrono::system_clock::now();
            currentLicense_.expiryDate = std::chrono::system_clock::now() + 
                                   std::chrono::hours(24 * config_.trialDays);
            
            initialized_ = true;
            return true;
        }
        
        bool isLicensed() const {
            if (!initialized_) return false;
            return currentLicense_.isValid() && !currentLicense_.isExpired();
        }
        
        bool isTrial() const {
            if (!initialized_) return false;
            return currentLicense_.type == LicenseType::Trial;
        }
        
        int daysRemaining() const {
            if (!initialized_) return 0;
            
            auto now = std::chrono::system_clock::now();
            auto expiry = currentLicense_.expiryDate;
            
            if (expiry == std::chrono::system_clock::time_point{} || expiry <= now) {
                return 0;
            }
            
            auto remaining = std::chrono::duration_cast<std::chrono::hours>(expiry - now).count();
            return static_cast<int>(remaining / 24);
        }
        
        LicenseInfo getCurrentLicense() const {
            std::lock_guard<std::mutex> lock(licenseMutex_);
            return currentLicense_;
        }
        
        std::string getLicenseStatusMessage() const {
            if (!initialized_) return "License engine not initialized";
            
            if (currentLicense_.status == LicenseStatus::Valid) {
                if (currentLicense_.type == LicenseType::Trial) {
                    return "Trial license - " + std::to_string(daysRemaining()) + " days remaining";
                } else if (currentLicense_.type == LicenseType::Subscription) {
                    return "Subscription license - " + std::to_string(daysRemaining()) + " days remaining";
                } else {
                    return "Valid license";
                }
            } else if (currentLicense_.status == LicenseStatus::Expired) {
                return "License expired";
            } else if (currentLicense_.status == LicenseStatus::Invalid) {
                return "Invalid license";
            } else if (currentLicense_.status == LicenseStatus::Missing) {
                return "No license found - trial active";
            } else {
                return "Unknown license status";
            }
        }
        
        // License operations
        bool activateLicense(const std::string& licenseKey) {
            std::lock_guard<std::mutex> lock(licenseMutex_);
            
            // Try online activation first
            if (network_ && network_->isOnline()) {
                if (network_->activateOnline(licenseKey, currentLicense_)) {
                    if (storage_) {
                        storage_->saveLicense(currentLicense_);
                    }
                    return true;
                }
            }
            
            // Fallback to local validation
            if (validator_ && validator_->validateKey(licenseKey, currentLicense_)) {
                currentLicense_.type = LicenseType::Perpetual;
                currentLicense_.status = LicenseStatus::Valid;
                currentLicense_.productId = config_.productName;
                currentLicense_.activationDate = std::chrono::system_clock::now();
                
                if (storage_) {
                    storage_->saveLicense(currentLicense_);
                }
                
                return true;
            }
            
            return false;
        }
        
        bool deactivateLicense() {
            std::lock_guard<std::mutex> lock(licenseMutex_);
            
            // Try online deactivation
            if (network_) {
                // Extract key from current license if needed
                network_->deactivateOnline("");
            }
            
            // Clear local license
            currentLicense_ = LicenseInfo{};
            currentLicense_.status = LicenseStatus::Missing;
            
            if (storage_) {
                storage_->deleteLicense();
            }
            
            return true;
        }
        
        void refreshLicenseStatus() {
            std::lock_guard<std::mutex> lock(licenseMutex_);
            
            // Check if license is expired
            if (currentLicense_.expiryDate != std::chrono::system_clock::time_point{} && 
                currentLicense_.expiryDate < std::chrono::system_clock::now()) {
                currentLicense_.status = LicenseStatus::Expired;
            }
            
            // Try online refresh
            if (network_ && network_->isOnline()) {
                network_->refreshOnline(currentLicense_);
                if (storage_) {
                    storage_->saveLicense(currentLicense_);
                }
            }
        }
        
        // UI operations
        void showActivationDialog() {
            if (ui_) {
                ui_->showActivationDialog();
            }
        }
        
        void showLicenseStatus() {
            if (ui_) {
                ui_->showStatusMessage(getLicenseStatusMessage());
            }
        }
        
        void showLicenseError(const std::string& error) {
            if (ui_) {
                ui_->showError(error);
            }
        }
    };
    
    // Pre-configured implementations factory
    class PreconfiguredImplementations {
    public:
        static ImplementationFactory createAnalogMorphFactory() {
            ImplementationFactory factory;
            
            // Register AnalogMorphV333 implementations
            factory.registerImplementation<FileBasedStorage>("FileBasedStorage");
            factory.registerImplementation<NetworkValidator>("NetworkValidator");
            factory.registerImplementation<AnalogMorphNetworkClient>("AnalogMorphNetwork");
            factory.registerImplementation<AnalogMorphUI>("AnalogMorphUI");
            
            return factory;
        }
        
        static ImplementationFactory createMinimalFactory() {
            ImplementationFactory factory;
            
            // Register minimal implementations
            factory.registerImplementation<SimpleFileStorage>("SimpleFileStorage");
            factory.registerImplementation<BasicKeyValidator>("BasicKeyValidator");
            factory.registerImplementation<NoNetworkClient>("NoNetworkClient");
            factory.registerImplementation<BasicUI>("BasicUI");
            
            return factory;
        }
    };
    
    // Builder pattern for easy setup
    class LicenseEngineBuilder {
    private:
        ImplementationFactory* factory_ = nullptr;
        std::unique_ptr<FactoredLicenseEngine> engine_;
        std::string storageImpl_ = "SimpleFileStorage";
        std::string validatorImpl_ = "BasicKeyValidator";
        std::string networkImpl_ = "NoNetworkClient";
        std::string uiImpl_ = "BasicUI";
        
    public:
        LicenseEngineBuilder() : engine_(std::make_unique<FactoredLicenseEngine>()) {}
        
        LicenseEngineBuilder& withFactory(ImplementationFactory& factory) {
            factory_ = &factory;
            return *this;
        }
        
        LicenseEngineBuilder& withStorage(const std::string& implementationName) {
            storageImpl_ = implementationName;
            return *this;
        }
        
        LicenseEngineBuilder& withValidator(const std::string& implementationName) {
            validatorImpl_ = implementationName;
            return *this;
        }
        
        LicenseEngineBuilder& withNetworkClient(const std::string& implementationName) {
            networkImpl_ = implementationName;
            return *this;
        }
        
        LicenseEngineBuilder& withUI(const std::string& implementationName) {
            uiImpl_ = implementationName;
            return *this;
        }
        
        std::unique_ptr<FactoredLicenseEngine> build(const LicenseConfig& config) {
            if (!engine_) return nullptr;
            
            // Use factory if provided, otherwise use default
            ImplementationFactory factory = factory_ ? 
                *factory_ : 
                PreconfiguredImplementations::createMinimalFactory();
            
            // Create and inject implementations
            auto storage = factory.create<ILicenseStorage>(storageImpl_);
            auto validator = factory.create<ILicenseValidator>(validatorImpl_);
            auto network = factory.create<ILicenseNetworkClient>(networkImpl_);
            auto ui = factory.create<ILicenseUI>(uiImpl_);
            
            // Inject into engine
            engine_->setStorage(std::move(storage));
            engine_->setValidator(std::move(validator));
            engine_->setNetworkClient(std::move(network));
            engine_->setUI(std::move(ui));
            
            // Initialize engine
            engine_->initialize(config);
            
            return std::move(engine_);
        }
    };
}

/*
  Usage Examples:
  
  // 1. Simple setup with minimal implementations:
  auto engine = LicenseEngine::LicenseEngineBuilder()
                  .withStorage("SimpleFileStorage")
                  .withValidator("BasicKeyValidator")
                  .build(config);
  
  // 2. Using AnalogMorphV333 implementations:
  auto analogMorphFactory = LicenseEngine::PreconfiguredImplementations::createAnalogMorphFactory();
  auto engine = LicenseEngine::LicenseEngineBuilder()
                  .withFactory(analogMorphFactory)
                  .build(config);
  
  // 3. Creating custom implementation and registering it:
  ImplementationFactory factory;
  factory.registerImplementation<MyCustomStorage>("MyCustomStorage");
  auto engine = LicenseEngine::LicenseEngineBuilder()
                  .withStorage("MyCustomStorage")
                  .build(config);
  
  // 4. Direct dependency injection:
  auto engine = std::make_unique<LicenseEngine::FactoredLicenseEngine>();
  engine->setStorage(std::make_unique<MyCustomStorage>());
  engine->setValidator(std::make_unique<MyCustomValidator>());
  engine->initialize(config);
  
  This factored approach:
  1. Enables easy integration with existing codebases
  2. Allows swapping implementations without code changes
  3. Supports both drop-in simplicity and advanced customization
  4. Maintains all thread safety and lifecycle requirements
  5. Provides clear separation of concerns
*/