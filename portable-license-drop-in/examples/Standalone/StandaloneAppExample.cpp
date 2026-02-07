/*
  ==============================================================================
  Example: Standalone Audio Application with License Drop-In Integration
  
  This example shows how to integrate the Portable License Drop-In Kit
  with a standalone audio application using the JUCE framework.
  
  Key principles demonstrated:
  1. Initialize license engine at application startup
  2. Handle license status in the main application window
  3. Provide convenient license activation dialogs
  4. Implement background license refresh
  ==============================================================================
*/

#include "JuceHeader.h"
#include "portable-license-drop-in/core/LICENSE_ENGINE.hpp"

//==============================================================================
// Main application class
class MyStandaloneApp : public juce::JUCEApplication
{
private:
    std::unique_ptr<LicenseEngine::LicenseEngine> licenseEngine;
    bool licenseValid = false;
    
public:
    //==============================================================================
    MyStandaloneApp() {}

    const juce::String getApplicationName() override       { return "My Audio App"; }
    const juce::String getApplicationVersion() override    { return "1.0.0"; }
    bool moreThanOneInstanceAllowed() override             { return true; }

    //==============================================================================
    void initialise (const juce::String& commandLine) override
    {
        // Initialize the license engine with product information
        licenseEngine = std::make_unique<LicenseEngine::LicenseEngine>();
        
        LicenseEngine::LicenseConfig config;
        config.companyName = "MyCompany";
        config.productName = "MyAudioApp";
        config.productVersion = "1.0.0";
        config.trialDays = 14;               // Minimum 14 days for pro audio
        config.subscriptionGracePeriodDays = 7;
        config.allowOfflineActivation = true;
        
        if (!licenseEngine->initialize(config))
        {
            juce::Logger::writeToLog("Failed to initialize license engine");
        }
        
        // Check license status
        refreshLicenseStatus();
        
        // Initialize the main window
        mainWindow.reset (new MainWindow ("My Audio App", licenseEngine.get()));
        
        // If license is not valid, show activation dialog
        if (!licenseEngine->isLicensed())
        {
            showLicenseActivationDialog();
        }
    }

    void shutdown() override
    {
        mainWindow = nullptr; // (deletes our window)
        licenseEngine = nullptr;
    }

    //==============================================================================
    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted (const juce::String& commandLine) override
    {
        // Handle command line arguments if needed
    }
    
    //==============================================================================
    // License-related methods
    void refreshLicenseStatus()
    {
        if (licenseEngine)
        {
            licenseEngine->refreshLicenseStatus();
            licenseValid = licenseEngine->isLicensed();
        }
    }
    
    void showLicenseActivationDialog()
    {
        if (mainWindow)
        {
            mainWindow->showLicenseDialog();
        }
    }

    //==============================================================================
    class MainWindow : public juce::DocumentWindow,
                       private juce::Timer
    {
    public:
        MainWindow (juce::String name, LicenseEngine::LicenseEngine* le)
            : DocumentWindow (name,
                              juce::Desktop::getInstance().getDefaultLookAndFeel()
                              .findColour (juce::ResizableWindow::backgroundColourId),
                              DocumentWindow::allButtons),
              licenseEngine(le)
        {
            setUsingNativeTitleBar (true);
            setContentOwned (new MainContentComponent (licenseEngine), true);

           #if JUCE_IOS || JUCE_ANDROID
            setFullScreen (true);
           #else
            setResizable (true, true);
            centreWithSize (getWidth(), getHeight());
           #endif

            setVisible (true);
            
            // Start timer for periodic license status checks
            startTimerHz (0.2f); // Check every 5 seconds
        }

        ~MainWindow() override {}

        void closeButtonPressed() override
        {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }
        
        void showLicenseDialog()
        {
            auto* dialog = new LicenseDialog(licenseEngine);
            dialog->enterModalState(true, juce::ModalCallbackFunction::create(
                [this](int result) 
                {
                    if (result == 1) // License activated successfully
                    {
                        refreshLicenseStatus();
                        getContentComponent()->repaint();
                    }
                }
            ));
        }
        
    private:
        LicenseEngine::LicenseEngine* licenseEngine;
        
        void timerCallback() override
        {
            // Periodically check license status in background
            refreshLicenseStatus();
        }
        
        void refreshLicenseStatus()
        {
            if (licenseEngine)
            {
                licenseEngine->refreshLicenseStatus();
            }
        }

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
};

//==============================================================================
// Main content component for the application
class MainContentComponent : public juce::Component
{
public:
    MainContentComponent (LicenseEngine::LicenseEngine* le)
        : licenseEngine(le)
    {
        // Create UI components
        addAndMakeVisible (headerLabel);
        addAndMakeVisible (licenseStatusLabel);
        addAndMakeVisible (activateButton);
        addAndMakeVisible (demoButton);
        
        // Configure components
        headerLabel.setText("My Audio Application", juce::dontSendNotification);
        headerLabel.setFont (juce::Font (24.0f, juce::Font::bold));
        headerLabel.setJustificationType (juce::Justification::centred);
        
        licenseStatusLabel.setText("Checking license...", juce::dontSendNotification);
        licenseStatusLabel.setJustificationType (juce::Justification::centred);
        
        activateButton.onClick = [this]() { showLicenseActivation(); };
        
        demoButton.onClick = [this]() { runDemo(); };
        
        updateLicenseStatus();
        
        setSize (600, 400);
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colours::lightgrey);
    }

    void resized() override
    {
        auto area = getLocalBounds();
        
        // Header
        headerLabel.setBounds (area.removeFromTop (60).reduced (20, 10));
        
        // License section
        auto licenseArea = area.removeFromTop (100).reduced (20, 10);
        licenseStatusLabel.setBounds (licenseArea.removeFromTop (40));
        activateButton.setBounds (licenseArea.removeFromTop (30));
        
        // Demo section
        demoButton.setBounds (area.removeFromTop (50).reduced (20, 10));
        
        // Main content area
        mainContentArea = area.reduced (20, 10);
    }
    
private:
    LicenseEngine::LicenseEngine* licenseEngine;
    
    juce::Label headerLabel;
    juce::Label licenseStatusLabel;
    juce::TextButton activateButton {"Activate License"};
    juce::TextButton demoButton {"Run Audio Demo"};
    juce::Rectangle<int> mainContentArea;
    
    void updateLicenseStatus()
    {
        if (!licenseEngine) return;
        
        juce::String statusText = licenseEngine->getLicenseStatusMessage();
        
        if (licenseEngine->isTrial())
        {
            statusText += " (" + juce::String(licenseEngine->daysRemaining()) + " days left)";
        }
        
        licenseStatusLabel.setText(statusText, juce::dontSendNotification);
        
        // Show/hide activate button based on license status
        activateButton.setVisible(!licenseEngine->isLicensed());
        
        // Enable/disable demo button based on license status
        demoButton.setEnabled(licenseEngine->isLicensed() || licenseEngine->isTrial());
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
                        if (licenseEngine->activateLicense(licenseKey.toStdString()))
                        {
                            updateLicenseStatus();
                            
                            juce::AlertWindow::showMessageBoxAsync(
                                juce::AlertWindow::InfoIcon,
                                "License Activated",
                                "Your license has been successfully activated. Thank you!"
                            );
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
    
    void runDemo()
    {
        if (!licenseEngine) return;
        
        if (!licenseEngine->isLicensed() && !licenseEngine->isTrial())
        {
            juce::AlertWindow::showMessageBoxAsync(
                juce::AlertWindow::WarningIcon,
                "Demo Not Available",
                "Please activate your license to use this feature."
            );
            return;
        }
        
        // Run audio demo - replace with actual audio processing code
        juce::AlertWindow::showMessageBoxAsync(
            juce::AlertWindow::InfoIcon,
            "Audio Demo",
            "Audio demo would run here. This demonstrates that the license is working correctly."
        );
    }
};

//==============================================================================
// Simple license dialog for showing activation status
class LicenseDialog : public juce::DialogWindow
{
public:
    LicenseDialog(LicenseEngine::LicenseEngine* le) 
        : DialogWindow("License Information", juce::Colours::lightgrey, true), licenseEngine(le)
    {
        setContentOwned(new LicenseComponent(licenseEngine), true);
        centreWithSize(400, 300);
        setResizable(false, false);
    }
    
private:
    LicenseEngine::LicenseEngine* licenseEngine;
    
    class LicenseComponent : public juce::Component
    {
    public:
        LicenseComponent(LicenseEngine::LicenseEngine* le) : licenseEngine(le)
        {
            addAndMakeVisible(statusLabel);
            addAndMakeVisible(activateButton);
            addAndMakeVisible(closeButton);
            
            updateStatus();
        }
        
        void resized() override
        {
            auto area = getLocalBounds();
            statusLabel.setBounds(area.removeFromTop(200).reduced(10));
            
            auto buttonArea = area.removeFromBottom(60).reduced(10);
            activateButton.setBounds(buttonArea.removeFromLeft(buttonArea.getWidth() / 2).reduced(5, 0));
            closeButton.setBounds(buttonArea.reduced(5, 0));
        }
        
    private:
        LicenseEngine::LicenseEngine* licenseEngine;
        juce::Label statusLabel;
        juce::TextButton activateButton {"Activate License"};
        juce::TextButton closeButton {"Close"};
        
        void updateStatus()
        {
            juce::String status = licenseEngine->getLicenseStatusMessage();
            
            if (licenseEngine->isLicensed())
            {
                status += "\n\nThis application is fully licensed and functional.";
                activateButton.setEnabled(false);
            }
            else if (licenseEngine->isTrial())
            {
                int days = licenseEngine->daysRemaining();
                status += "\n\n" + juce::String(days) + " days remaining in trial.";
                activateButton.setEnabled(true);
            }
            else
            {
                status += "\n\nPlease activate your license to continue using this application.";
                activateButton.setEnabled(true);
            }
            
            statusLabel.setText(status, juce::dontSendNotification);
        }
    };
};

//==============================================================================
static MyStandaloneApp app;