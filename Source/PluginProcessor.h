#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include "SSOT/ModelSSOT.h"
#include "DSP/YourProcessor.h"

/**
 * Main Audio Processor
 * 
 * Handles audio processing, parameter management, and state.
 * TODO: Customize for your plugin
 */
class YourProcessor : public juce::AudioProcessor
{
public:
    // ========================================================================
    // CONSTRUCTOR / DESTRUCTOR
    // ========================================================================
    
    YourProcessor();
    ~YourProcessor() override;
    
    // ========================================================================
    // APVTS ACCESS
    // ========================================================================
    
    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }
    
    // ========================================================================
    // JUCE LIFECYCLE
    // ========================================================================
    
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    
#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif
    
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
    
    // ========================================================================
    // EDITOR
    // ========================================================================
    
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    
    // ========================================================================
    // PLUGIN INFO
    // ========================================================================
    
    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    
    // ========================================================================
    // PROGRAMS
    // ========================================================================
    
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;
    
    // ========================================================================
    // STATE
    // ========================================================================
    
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;
    
private:
    // ========================================================================
    // PRIVATE MEMBERS
    // ========================================================================
    
    juce::AudioProcessorValueTreeState apvts;
    YourProcessorCore dspCore;
    
    // ========================================================================
    // PARAMETER LAYOUT
    // ========================================================================
    
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    // ========================================================================
    // JUCE MACROS
    // ========================================================================
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(YourProcessor)
};
