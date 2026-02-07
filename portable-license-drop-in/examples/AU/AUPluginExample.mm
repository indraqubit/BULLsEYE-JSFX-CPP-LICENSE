/*
  ==============================================================================
  Example: Audio Unit Plugin with License Drop-In Integration
  
  This example shows how to integrate the Portable License Drop-In Kit
  with an Audio Unit plugin using the JUCE framework.
  
  Key principles demonstrated:
  1. Initialize license engine once in constructor
  2. Thread-safe license checks in audio rendering
  3. License status display in UI
  4. Proper handling of AU-specific requirements
  ==============================================================================
*/

#include "JucePluginMain.h"
#include "portable-license-drop-in/core/LICENSE_ENGINE.hpp"

class MyAUPlugin  : public juce::AudioProcessor
{
private:
    LicenseEngine::LicenseEngine licenseEngine;
    bool licenseChecked = false;
    
public:
    //==============================================================================
    MyAUPlugin()
        : AudioProcessor (BusesProperties()
                           #if ! JucePlugin_IsMidiEffect
                            #if ! JucePlugin_IsSynth
                             .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                            #endif
                             .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                           #endif
                           )
    {
        // Initialize the license engine with product information
        LicenseEngine::LicenseConfig config;
        config.companyName = "MyCompany";
        config.productName = "MyAudioPlugin";
        config.productVersion = "1.0.0";
        config.trialDays = 14;               // Minimum 14 days for pro audio
        config.subscriptionGracePeriodDays = 7;
        config.allowOfflineActivation = true;
        
        if (!licenseEngine.initialize(config))
        {
            juce::Logger::writeToLog("Failed to initialize license engine");
        }
    }

    ~MyAUPlugin() override {}

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override
    {
        // Check license status before processing begins
        refreshLicenseStatus();
    }

    void releaseResources() override {}

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override
    {
        return true;
    }
   #endif

    void processBlock (juce::AudioBuffer<float>& buffer, 
                       juce::MidiBuffer& midiMessages) override
    {
        juce::ScopedNoDenormals noDenormals;
        
        // License check - thread-safe and fast
        if (!licenseEngine.isLicensed())
        {
            // For unlicensed use, either silence output or apply trial limitations
            buffer.clear();
            
            // Only show license dialog once per session to avoid thread issues
            if (!licenseChecked)
            {
                licenseChecked = true;
                juce::MessageManager::callAsync([this]() {
                    showLicenseDialog();
                });
            }
            
            return;
        }
        
        // Normal audio processing for licensed users
        for (int channel = 0; channel < getTotalNumOutputChannels(); ++channel)
        {
            auto* channelData = buffer.getWritePointer (channel);
            
            // Example processing - replace with your audio processing
            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                channelData[sample] = channelData[sample] * 0.5f;
            }
        }
    }

    //==============================================================================
    const juce::String getName() const override { return "My Audio Plugin"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    //==============================================================================
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override
    {
        // Save plugin state
    }

    void setStateInformation (const void* data, int sizeInBytes) override
    {
        // Restore plugin state
    }
    
    //==============================================================================
    // AU-specific properties
    bool setBusesLayout (const BusesLayout& layouts) override
    {
        // AU specific bus layout handling
        return AudioProcessor::setBusesLayout (layouts);
    }
    
    bool canApplyBusesLayout (const BusesLayout& layouts) const override
    {
        // AU specific layout validation
        return AudioProcessor::canApplyBusesLayout (layouts);
    }
    
    //==============================================================================
    // License-related methods
    void refreshLicenseStatus()
    {
        // Perform license validation in background
        licenseEngine.refreshLicenseStatus();
    }
    
    void showLicenseDialog()
    {
        // Create and show license activation dialog
        auto* dialog = new LicenseDialog(licenseEngine);
        dialog->enterModalState(true, juce::ModalCallbackFunction::create(
            [this](int result) 
            {
                if (result == 1) // License activated successfully
                {
                    refreshLicenseStatus();
                }
            }
        ));
    }
    
    // Custom editor that displays license status
    juce::AudioProcessorEditor* createEditor() override
    {
        return new MyPluginEditor(*this, licenseEngine);
    }
    
    //==============================================================================
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MyAUPlugin)
};

//==============================================================================
// Custom editor with license status display
class MyPluginEditor : public juce::AudioProcessorEditor,
                       private juce::Timer
{
public:
    MyPluginEditor (MyAUPlugin& p, LicenseEngine::LicenseEngine& le)
        : AudioProcessorEditor (&p), plugin (p), licenseEngine (le)
    {
        // UI setup code
        addAndMakeVisible (licenseStatusLabel);
        addAndMakeVisible (activateButton);
        
        activateButton.onClick = [this]() { showLicenseActivation(); };
        
        // Update license status periodically
        startTimerHz (1); // 1 Hz update
        updateLicenseStatus();
        
        setSize (400, 300);
    }
    
    ~MyPluginEditor() override {}
    
    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colours::darkgrey);
    }
    
    void resized() override
    {
        auto area = getLocalBounds();
        
        licenseStatusLabel.setBounds (10, 10, area.getWidth() - 20, 30);
        activateButton.setBounds (10, 50, area.getWidth() - 20, 30);
        
        // Rest of UI layout
    }
    
    void timerCallback() override
    {
        updateLicenseStatus();
    }
    
private:
    MyAUPlugin& plugin;
    LicenseEngine::LicenseEngine& licenseEngine;
    
    juce::Label licenseStatusLabel {"License Status", "Checking..."};
    juce::TextButton activateButton {"Activate License"};
    
    void updateLicenseStatus()
    {
        juce::String statusText = licenseEngine.getLicenseStatusMessage();
        
        if (licenseEngine.isTrial())
        {
            statusText += " (" + juce::String(licenseEngine.daysRemaining()) + " days left)";
        }
        
        licenseStatusLabel.setText(statusText, juce::dontSendNotification);
        
        // Show/hide activate button based on license status
        activateButton.setVisible(!licenseEngine.isLicensed());
    }
    
    void showLicenseActivation()
    {
        auto* window = new juce::AlertWindow ("License Activation", 
                                             "Enter your license key:", 
                                             juce::AlertWindow::InfoIcon);
        
        window->addTextEditor ("licenseKey", "", "License Key:");
        window->addButton ("OK", 1, juce::KeyPress::returnKey);
        window->addButton ("Cancel", 0, juce::KeyPress::escapeKey);
        
        window->enterModalState(true, juce::ModalCallbackFunction::create(
            [this](int result) 
            {
                if (result == 1) // OK button pressed
                {
                    auto* textEditor = window->getTextEditor("licenseKey");
                    if (textEditor)
                    {
                        juce::String licenseKey = textEditor->getText();
                        if (licenseEngine.activateLicense(licenseKey.toStdString()))
                        {
                            updateLicenseStatus();
                        }
                        else
                        {
                            juce::AlertWindow::showMessageBoxAsync(
                                juce::AlertWindow::WarningIcon,
                                "Activation Failed",
                                "The license key is invalid or could not be validated."
                            );
                        }
                    }
                }
            }
        ));
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MyPluginEditor)
};

//==============================================================================
// Simple license dialog for showing activation status
class LicenseDialog : public juce::DialogWindow
{
public:
    LicenseDialog(LicenseEngine::LicenseEngine& le) 
        : DialogWindow("License Information", juce::Colours::lightgrey, true)
    {
        setContentOwned(new LicenseComponent(le), true);
        centreWithSize(400, 300);
        setResizable(false, false);
    }
    
private:
    class LicenseComponent : public juce::Component
    {
    public:
        LicenseComponent(LicenseEngine::LicenseEngine& le) : licenseEngine(le)
        {
            addAndMakeVisible(statusLabel);
            addAndMakeVisible(activateButton);
            
            updateStatus();
        }
        
        void resized() override
        {
            auto area = getLocalBounds();
            statusLabel.setBounds(area.reduced(10));
            activateButton.setBounds(area.removeFromBottom(40).reduced(10));
        }
        
    private:
        LicenseEngine::LicenseEngine& licenseEngine;
        juce::Label statusLabel;
        juce::TextButton activateButton {"Activate"};
        
        void updateStatus()
        {
            juce::String status = licenseEngine.getLicenseStatusMessage();
            
            if (licenseEngine.isLicensed())
            {
                status += "\n\nThis plugin is fully licensed and functional.";
            }
            else if (licenseEngine.isTrial())
            {
                int days = licenseEngine.daysRemaining();
                status += "\n\n" + juce::String(days) + " days remaining in trial.";
            }
            else
            {
                status += "\n\nPlease activate your license to continue using this plugin.";
            }
            
            statusLabel.setText(status, juce::dontSendNotification);
        }
    };
};

//==============================================================================
// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MyAUPlugin();
}