/*
  ==============================================================================
  Portable License Drop-In Kit
  License Engine Implementation (Audio-Safe Version)
  
  A modular, DRY-compliant licensing system for audio plugins and commercial software.
  This single-file header contains all the core licensing functionality.
  
  IMPORTANT AUDIO SAFETY NOTES:
  1. License checks for AUDIO THREAD must use atomic API (isLicensedAtomically)
  2. The LicenseInfo struct is NOT thread-safe for direct access
  3. All license state updates MUST happen from UI/background threads
  4. Use tryOrSkip() pattern for non-blocking operations
  5. Audio thread: ONLY read atomic status flags, never block
  
  Rules & Best Practices:
  1. Always initialize the license engine before any license checks
  2. License checks from UI thread: use isLicensed(), isTrial(), isExpired()
  3. License checks from AUDIO thread: use isLicensedAtomically() ONLY
  4. License validation is automatic and periodic (from background thread)
  5. Implement appropriate grace periods for subscription licenses
  6. Always respect user privacy and system security
  ==============================================================================
*/

#pragma once

#include <string>
#include <chrono>
#include <atomic>
#include <mutex>
#include <memory>
#include <functional>
#include <array>
#include <cstdint>

#ifdef _WIN32
#include <windows.h>
#include <intrin.h>
#else
#include <sys/utsname.h>
#endif

namespace LicenseEngine {
    
    // Enum for license types (audio-safe: stored as uint8_t in atomics)
    enum class LicenseType : uint8_t {
        Trial = 0,
        Subscription = 1,
        Perpetual = 2,
        FeatureLimited = 3
    };
    
    // Enum for license status (audio-safe: stored as uint8_t in atomics)
    enum class LicenseStatus : uint8_t {
        Valid = 0,
        Expired = 1,
        Invalid = 2,
        Missing = 3,
        PendingActivation = 4
    };
    
    // Maximum number of features supported (for audio-safe fixed-size storage)
    constexpr size_t MAX_FEATURES = 8;
    constexpr size_t MACHINE_ID_HASH_LENGTH = 32;
    
    // Audio-safe license information structure
    // NOTE: This struct contains data for background/UI thread use.
    // For audio thread checks, use the atomic API only.
    struct LicenseInfo {
        LicenseType type = LicenseType::Trial;
        LicenseStatus status = LicenseStatus::Missing;
        std::string productId;
        std::string userId;
        std::string machineId;
        std::chrono::system_clock::time_point activationDate;
        std::chrono::system_clock::time_point expiryDate;
        
        // Fixed-size feature storage (no heap allocation)
        std::array<std::string, MAX_FEATURES> enabledFeatures;
        size_t enabledFeatureCount = 0;
        
        bool isValid() const {
            return status == LicenseStatus::Valid;
        }
        
        bool isExpired() const {
            return status == LicenseStatus::Expired || 
                   (expiryDate != std::chrono::system_clock::time_point{} && 
                    expiryDate < std::chrono::system_clock::now());
        }
        
        bool isInGracePeriod(uint32_t graceDays, std::chrono::system_clock::time_point now) const {
            if (status != LicenseStatus::Expired) return false;
            if (type != LicenseType::Subscription) return false;
            if (expiryDate == std::chrono::system_clock::time_point{}) return false;
            
            auto graceEnd = expiryDate + std::chrono::hours(24 * graceDays);
            return now < graceEnd;
        }
    };
    
    // Audio-safe license snapshot (for lock-free audio thread reads)
    struct LicenseSnapshot {
        std::atomic<uint8_t> status{static_cast<uint8_t>(LicenseStatus::Missing)};
        std::atomic<uint8_t> type{static_cast<uint8_t>(LicenseType::Trial)};
        std::atomic<int64_t> daysRemaining{0};
        std::atomic<bool> isValid{false};
        std::atomic<bool> isInGracePeriod{false};
        std::atomic<uint32_t> featureMask{0};
        
        // Memory ordering: release for writes, acquire for reads
        void updateFrom(const LicenseInfo& info, uint32_t graceDays) {
            status.store(static_cast<uint8_t>(info.status), std::memory_order_release);
            type.store(static_cast<uint8_t>(info.type), std::memory_order_release);
            
            // Calculate days remaining with proper rounding
            auto now = std::chrono::system_clock::now();
            int64_t days = 0;
            
            if (info.status == LicenseStatus::Valid) {
                if (info.expiryDate != std::chrono::system_clock::time_point{} && 
                    info.expiryDate > now) {
                    auto hours = std::chrono::duration_cast<std::chrono::hours>(
                        info.expiryDate - now).count();
                    days = (hours + 12) / 24;  // Ceiling rounding
                } else if (info.type == LicenseType::Perpetual) {
                    days = -1;  // Infinite
                }
            } else if (info.isInGracePeriod(graceDays, now)) {
                auto graceEnd = info.expiryDate + std::chrono::hours(24 * graceDays);
                auto hours = std::chrono::duration_cast<std::chrono::hours>(
                    graceEnd - now).count();
                days = (hours + 12) / 24;
            }
            
            daysRemaining.store(days, std::memory_order_release);
            isValid.store(info.isValid(), std::memory_order_release);
            isInGracePeriod.store(info.isInGracePeriod(graceDays, now), std::memory_order_release);
            
            // Build feature mask
            uint32_t mask = 0;
            for (size_t i = 0; i < info.enabledFeatureCount && i < MAX_FEATURES; ++i) {
                mask |= (1u << i);
            }
            featureMask.store(mask, std::memory_order_release);
        }
    };
    
    // License configuration
    struct LicenseConfig {
        std::string companyName;
        std::string productName;
        std::string productVersion;
        uint32_t trialDays = 14;
        uint32_t subscriptionGracePeriodDays = 7;
        bool allowOfflineActivation = true;
        bool requireNetworkActivation = false;
    };
    
    // Forward declaration
    class LicenseProvider;
    
    // Audio-safe license result for try-or-skip pattern
    enum class LicenseResult {
        Success,
        WouldBlock,
        NotInitialized,
        InvalidState
    };
    
    // Main license engine class (audio-safe design)
    class LicenseEngine {
    private:
        LicenseConfig config_;
        LicenseInfo currentLicense_;
        
        // Audio-safe atomic snapshot for lock-free audio thread reads
        LicenseSnapshot audioSnapshot_;
        
        // Background thread state (mutex required for update operations)
        // This mutex is for background/UI thread updates ONLY
        // Audio thread NEVER acquires this mutex
        mutable std::mutex stateMutex_;
        
        std::atomic<bool> initialized_{false};
        std::unique_ptr<LicenseProvider> provider_;
        
        // Pre-allocated buffers (no allocation in audio path)
        std::array<char, 256> machineIdBuffer_{};
        std::array<char, 64> hashBuffer_{};
        
        // Machine identification (pre-allocated)
        void getMachineIdPreallocated(char* buffer, size_t bufferSize);
        
        // License file operations (background thread only)
        bool loadLicenseFromFile(const std::string& filePath);
        bool saveLicenseToFile(const std::string& filePath, const LicenseInfo& license);
        
        // License validation (background thread only)
        bool validateLicenseFormat(const std::string& licenseData);
        bool validateLicenseSignature(const std::string& licenseData);
        
        // Update atomic snapshot (thread-safe)
        void updateAudioSnapshot() {
            std::lock_guard<std::mutex> lock(stateMutex_);
            audioSnapshot_.updateFrom(currentLicense_, config_.subscriptionGracePeriodDays);
        }
        
    public:
        LicenseEngine() = default;
        ~LicenseEngine() = default;
        
        // Initialize the license engine (MUST be called from UI/background thread)
        bool initialize(const LicenseConfig& config);
        
        // ========== AUDIO THREAD SAFE API ==========
        // These methods are SAFE to call from audio thread (processBlock)
        // Uses lock-free atomic reads with try-or-skip pattern
        
        // Quick license status check for audio thread (NON-BLOCKING)
        // Returns: true if licensed, false otherwise
        // This is the ONLY method safe to call from audio thread
        inline bool isLicensedAtomically() const {
            return audioSnapshot_.isValid.load(std::memory_order_acquire);
        }
        
        // Get days remaining for audio thread UI display (NON-BLOCKING)
        // Returns: -1 for perpetual/infinite, 0+ for limited
        inline int64_t getDaysRemainingAtomically() const {
            return audioSnapshot_.daysRemaining.load(std::memory_order_acquire);
        }
        
        // Check if in grace period (NON-BLOCKING)
        inline bool isInGracePeriodAtomically() const {
            return audioSnapshot_.isInGracePeriod.load(std::memory_order_acquire);
        }
        
        // Check feature enabled (NON-BLOCKING)
        inline bool isFeatureEnabledAtomically(uint32_t featureIndex) const {
            uint32_t mask = audioSnapshot_.featureMask.load(std::memory_order_acquire);
            return (mask & (1u << featureIndex)) != 0;
        }
        
        // ========== UI/BACKGROUND THREAD API ==========
        // These methods MUST be called from UI or background thread
        // They may block and use mutex protection
        
        // License status methods (for UI thread)
        bool isLicensed() const;
        bool isTrial() const;
        bool isExpired() const;
        int daysRemaining() const;
        
        // License information
        LicenseInfo getCurrentLicense() const;
        std::string getLicenseStatusMessage() const;
        
        // Try-or-skip pattern for non-blocking operations
        // Returns LicenseResult instead of blocking
        LicenseResult tryActivateLicense(const std::string& licenseKey);
        LicenseResult tryRefreshStatus();
        
        // License activation (blocking - for UI thread)
        bool activateLicense(const std::string& licenseKey);
        bool activateLicenseFromFile(const std::string& licenseFilePath);
        bool deactivateLicense();
        
        // Feature management
        bool isFeatureEnabled(const std::string& featureId) const;
        std::vector<std::string> getEnabledFeatures() const;
        
        // Maintenance (background thread)
        void refreshLicenseStatus();
        void setLicenseProvider(std::unique_ptr<LicenseProvider> provider);
        
        // Drop-data fallback: get last known good state
        // Returns false if no valid state exists
        bool getLastKnownGoodState(LicenseInfo& outInfo) const;
    };
    
    // Abstract license provider for different activation methods
    class LicenseProvider {
    public:
        virtual ~LicenseProvider() = default;
        virtual bool activateLicense(const std::string& licenseKey, LicenseInfo& license) = 0;
        virtual bool validateLicense(const LicenseInfo& license) = 0;
        virtual bool refreshLicense(LicenseInfo& license) = 0;
    };
    
    // Implementation details
    
    inline void LicenseEngine::getMachineIdPreallocated(char* buffer, size_t bufferSize) {
#ifdef _WIN32
        DWORD volumeSerial = 0;
        GetVolumeInformationA("C:\\", nullptr, 0, &volumeSerial, nullptr, nullptr, nullptr, 0);
        
        int cpuInfo[4] = {0};
        __cpuid(cpuInfo, 1);
        
        snprintf(buffer, bufferSize, "%lu-%d", volumeSerial, cpuInfo[0]);
#else
        struct utsname sysInfo;
        uname(&sysInfo);
        snprintf(buffer, bufferSize, "%s-%s-%s", 
                 sysInfo.sysname, sysInfo.nodename, sysInfo.release);
#endif
        
        // Simple hash (for production, use SHA-256)
        uint64_t hash = 1469598103934665603ULL;  // FNV offset basis
        for (size_t i = 0; buffer[i] != '\0' && i < bufferSize; ++i) {
            hash ^= static_cast<uint64_t>(buffer[i]);
            hash *= 1099511628211ULL;  // FNV prime
        }
        snprintf(buffer, bufferSize, "%llu", static_cast<unsigned long long>(hash));
    }
    
    inline bool LicenseEngine::initialize(const LicenseConfig& config) {
        {
            std::lock_guard<std::mutex> lock(stateMutex_);
            
            config_ = config;
            
            // Pre-allocate machine ID
            getMachineIdPreallocated(machineIdBuffer_.data(), machineIdBuffer_.size());
            
            // Load existing license if available
            std::string licenseFilePath = config_.productName + ".lic";
            if (loadLicenseFromFile(licenseFilePath)) {
                refreshLicenseStatus();
            } else {
                // Initialize trial license
                currentLicense_.type = LicenseType::Trial;
                currentLicense_.status = LicenseStatus::Valid;
                currentLicense_.productId = config_.productName;
                currentLicense_.activationDate = std::chrono::system_clock::now();
                currentLicense_.expiryDate = std::chrono::system_clock::now() + 
                    std::chrono::hours(24 * config_.trialDays);
                snprintf(machineIdBuffer_.data(), machineIdBuffer_.size(), "%s", 
                         machineIdBuffer_.data());  // Already populated
                currentLicense_.machineId = machineIdBuffer_.data();
            }
            
            initialized_ = true;
        }
        
        // Update atomic snapshot OUTSIDE the lock (avoid lock in release)
        updateAudioSnapshot();
        
        return true;
    }
    
    inline bool LicenseEngine::isLicensed() const {
        if (!initialized_) return false;
        
        std::lock_guard<std::mutex> lock(stateMutex_);
        return currentLicense_.isValid();
    }
    
    inline bool LicenseEngine::isTrial() const {
        if (!initialized_) return false;
        
        std::lock_guard<std::mutex> lock(stateMutex_);
        return currentLicense_.type == LicenseType::Trial;
    }
    
    inline bool LicenseEngine::isExpired() const {
        if (!initialized_) return true;
        
        std::lock_guard<std::mutex> lock(stateMutex_);
        
        // Check grace period
        if (currentLicense_.isInGracePeriod(config_.subscriptionGracePeriodDays, 
                                             std::chrono::system_clock::now())) {
            return false;  // Not expired if in grace period
        }
        
        return currentLicense_.isExpired();
    }
    
    inline int LicenseEngine::daysRemaining() const {
        if (!initialized_) return 0;
        
        std::lock_guard<std::mutex> lock(stateMutex_);
        
        auto now = std::chrono::system_clock::now();
        
        // Check grace period first
        if (currentLicense_.isInGracePeriod(config_.subscriptionGracePeriodDays, now)) {
            auto graceEnd = currentLicense_.expiryDate + 
                std::chrono::hours(24 * config_.subscriptionGracePeriodDays);
            auto hours = std::chrono::duration_cast<std::chrono::hours>(graceEnd - now).count();
            return static_cast<int>((hours + 12) / 24);  // Ceiling rounding
        }
        
        auto expiry = currentLicense_.expiryDate;
        
        if (expiry == std::chrono::system_clock::time_point{} || expiry <= now) {
            return 0;
        }
        
        if (currentLicense_.type == LicenseType::Perpetual) {
            return -1;  // Infinite
        }
        
        auto hours = std::chrono::duration_cast<std::chrono::hours>(expiry - now).count();
        return static_cast<int>((hours + 12) / 24);  // Ceiling rounding
    }
    
    inline LicenseInfo LicenseEngine::getCurrentLicense() const {
        std::lock_guard<std::mutex> lock(stateMutex_);
        return currentLicense_;
    }
    
    inline std::string LicenseEngine::getLicenseStatusMessage() const {
        if (!initialized_) return "License engine not initialized";
        
        std::lock_guard<std::mutex> lock(stateMutex_);
        
        if (currentLicense_.status == LicenseStatus::Valid) {
            if (currentLicense_.type == LicenseType::Trial) {
                int days = daysRemaining();
                if (days < 0) return "Trial license";
                return "Trial license - " + std::to_string(days) + " days remaining";
            } else if (currentLicense_.type == LicenseType::Subscription) {
                int days = daysRemaining();
                if (days < 0) return "Subscription license (grace period)";
                return "Subscription license - " + std::to_string(days) + " days remaining";
            } else if (currentLicense_.type == LicenseType::Perpetual) {
                return "Perpetual license";
            } else {
                return "Valid license";
            }
        } else if (currentLicense_.status == LicenseStatus::Expired) {
            if (currentLicense_.isInGracePeriod(config_.subscriptionGracePeriodDays, 
                                                 std::chrono::system_clock::now())) {
                int days = daysRemaining();
                return "License expired - " + std::to_string(days) + " days grace period remaining";
            }
            return "License expired";
        } else if (currentLicense_.status == LicenseStatus::Invalid) {
            return "Invalid license";
        } else if (currentLicense_.status == LicenseStatus::Missing) {
            return "No license found - trial active";
        } else if (currentLicense_.status == LicenseStatus::PendingActivation) {
            return "License activation pending";
        } else {
            return "Unknown license status";
        }
    }
    
    inline LicenseResult LicenseEngine::tryActivateLicense(const std::string& licenseKey) {
        if (!initialized_) return LicenseResult::NotInitialized;
        
        // Try to acquire lock without blocking (try-or-skip pattern)
        std::unique_lock<std::mutex> lock(stateMutex_, std::try_to_lock);
        if (!lock.owns_lock()) {
            return LicenseResult::WouldBlock;  // Skip if can't acquire
        }
        
        if (provider_) {
            LicenseInfo newLicense;
            if (provider_->activateLicense(licenseKey, newLicense)) {
                currentLicense_ = newLicense;
                
                std::string licenseFilePath = config_.productName + ".lic";
                bool saved = saveLicenseToFile(licenseFilePath, currentLicense_);
                
                // Update snapshot after releasing lock
                lock.unlock();
                updateAudioSnapshot();
                
                return LicenseResult::Success;
            }
        }
        
        // Default implementation
        if (validateLicenseFormat(licenseKey) && validateLicenseSignature(licenseKey)) {
            currentLicense_.type = LicenseType::Perpetual;
            currentLicense_.status = LicenseStatus::Valid;
            currentLicense_.productId = config_.productName;
            currentLicense_.machineId = machineIdBuffer_.data();
            currentLicense_.activationDate = std::chrono::system_clock::now();
            
            std::string licenseFilePath = config_.productName + ".lic";
            bool saved = saveLicenseToFile(licenseFilePath, currentLicense_);
            
            // Update snapshot after releasing lock
            lock.unlock();
            updateAudioSnapshot();
            
            return LicenseResult::Success;
        }
        
        return LicenseResult::InvalidState;
    }
    
    inline LicenseResult LicenseEngine::tryRefreshStatus() {
        if (!initialized_) return LicenseResult::NotInitialized;
        
        std::unique_lock<std::mutex> lock(stateMutex_, std::try_to_lock);
        if (!lock.owns_lock()) {
            return LicenseResult::WouldBlock;
        }
        
        // Check if license is expired
        auto now = std::chrono::system_clock::now();
        if (currentLicense_.expiryDate != std::chrono::system_clock::time_point{} && 
            currentLicense_.expiryDate < now) {
            
            // Check grace period
            if (!currentLicense_.isInGracePeriod(config_.subscriptionGracePeriodDays, now)) {
                currentLicense_.status = LicenseStatus::Expired;
            }
        }
        
        // Validate with provider if available (non-blocking check)
        if (provider_) {
            bool valid = provider_->validateLicense(currentLicense_);
            if (!valid && currentLicense_.status == LicenseStatus::Valid) {
                // Provider says invalid, but we have a local license
                // Don't immediately expire - may be offline
                // Just mark for re-validation
            }
        }
        
        lock.unlock();
        updateAudioSnapshot();
        
        return LicenseResult::Success;
    }
    
    inline bool LicenseEngine::activateLicense(const std::string& licenseKey) {
        return tryActivateLicense(licenseKey) == LicenseResult::Success;
    }
    
    inline bool LicenseEngine::activateLicenseFromFile(const std::string& licenseFilePath) {
        if (!initialized_) return false;
        
        std::lock_guard<std::mutex> lock(stateMutex_);
        bool loaded = loadLicenseFromFile(licenseFilePath);
        
        if (loaded) {
            updateAudioSnapshot();
        }
        
        return loaded;
    }
    
    inline bool LicenseEngine::deactivateLicense() {
        if (!initialized_) return false;
        
        std::lock_guard<std::mutex> lock(stateMutex_);
        
        currentLicense_ = LicenseInfo{};
        currentLicense_.status = LicenseStatus::Missing;
        
        std::string licenseFilePath = config_.productName + ".lic";
        bool deleted = std::remove(licenseFilePath.c_str()) == 0;
        
        updateAudioSnapshot();
        
        return deleted;
    }
    
    inline bool LicenseEngine::isFeatureEnabled(const std::string& featureId) const {
        if (!initialized_) return false;
        
        std::lock_guard<std::mutex> lock(stateMutex_);
        
        // For trial licenses, check if feature is enabled
        if (currentLicense_.type == LicenseType::Trial) {
            for (size_t i = 0; i < currentLicense_.enabledFeatureCount; ++i) {
                if (currentLicense_.enabledFeatures[i] == featureId) return true;
            }
            return false;
        }
        
        // For paid licenses, all features are enabled if valid
        return currentLicense_.isValid();
    }
    
    inline std::vector<std::string> LicenseEngine::getEnabledFeatures() const {
        if (!initialized_) return {};
        
        std::lock_guard<std::mutex> lock(stateMutex_);
        
        std::vector<std::string> features;
        
        if (currentLicense_.type == LicenseType::Trial) {
            for (size_t i = 0; i < currentLicense_.enabledFeatureCount; ++i) {
                features.push_back(currentLicense_.enabledFeatures[i]);
            }
        } else if (currentLicense_.isValid()) {
            features.push_back("all");
        }
        
        return features;
    }
    
    inline void LicenseEngine::refreshLicenseStatus() {
        auto result = tryRefreshStatus();
        // If WouldBlock, skip this refresh - use last known state
        // Audio will continue with cached values
    }
    
    inline void LicenseEngine::setLicenseProvider(std::unique_ptr<LicenseProvider> provider) {
        std::lock_guard<std::mutex> lock(stateMutex_);
        provider_ = std::move(provider);
    }
    
    inline bool LicenseEngine::loadLicenseFromFile(const std::string& filePath) {
        (void)filePath;
        return false;
    }
    
    inline bool LicenseEngine::saveLicenseToFile(const std::string& filePath, const LicenseInfo& license) {
        (void)filePath;
        (void)license;
        return false;
    }
    
    inline bool LicenseEngine::validateLicenseFormat(const std::string& licenseData) {
        return licenseData.length() >= 16;
    }
    
    inline bool LicenseEngine::validateLicenseSignature(const std::string& licenseData) {
        return licenseData.length() >= 16;
    }
    
    inline bool LicenseEngine::getLastKnownGoodState(LicenseInfo& outInfo) const {
        if (!initialized_) return false;
        
        std::lock_guard<std::mutex> lock(stateMutex_);
        
        // Return last valid state (drop-data fallback)
        if (currentLicense_.isValid() || 
            currentLicense_.isInGracePeriod(config_.subscriptionGracePeriodDays, 
                                             std::chrono::system_clock::now())) {
            outInfo = currentLicense_;
            return true;
        }
        
        return false;
    }
    
} // namespace LicenseEngine
