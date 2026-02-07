#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include "SSOT/ModelSSOT.h"
#include "DSP/BULLsEYEProcessor.h"

/**
 * BULLsEYE Audio Processor
 *
 * Main audio processor for LUFS-I metering with True Peak detection.
 * Handles audio processing, parameter management, and state.
 */
class BULLsEYEProcessor : public juce::AudioProcessor
{
public:
    // ========================================================================
    // CONSTRUCTOR / DESTRUCTOR
    // ========================================================================

    BULLsEYEProcessor();
    ~BULLsEYEProcessor() override;

    // ========================================================================
    // APVTS ACCESS
    // ========================================================================

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }
    const juce::AudioProcessorValueTreeState& getAPVTS() const { return apvts; }

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
    bool hasEditor() const override { return true; }

    // ========================================================================
    // PLUGIN INFO
    // ========================================================================

    const juce::String getName() const override { return "BULLsEYE"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    // ========================================================================
    // PROGRAMS
    // ========================================================================

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return "Default"; }
    void changeProgramName(int, const juce::String&) override {}

    // ========================================================================
    // STATE
    // ========================================================================

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // ========================================================================
    // METER ACCESS (for UI)
    // ========================================================================

    float getNormalizedLUFS() const { return dspCore.getNormalizedLUFS(); }
    float getNormalizedTruePeak() const { return dspCore.getNormalizedTruePeak(); }
    double getIntegratedLUFS() const { return dspCore.getIntegratedLUFS(); }
    double getTruePeakDB() const { return dspCore.getTruePeakDB(); }
    double getDeviationLU() const { return dspCore.getDeviationLU(); }
    ModelSSOT::ContentType getContentType() const { return dspCore.getContentType(); }

private:
    // ========================================================================
    // PRIVATE MEMBERS
    // ========================================================================

    juce::AudioProcessorValueTreeState apvts;
    BULLsEYEProcessorCore dspCore;

    // Transport state detection using DAW playhead
    bool wasPlaying{false};  // Track previous transport state for stop→play detection

    // ========================================================================
    // PARAMETER LAYOUT
    // ========================================================================

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // ========================================================================
    // PARAMETER CALLBACKS
    // ========================================================================

    void contentTypeChanged();

    // ========================================================================
    // JUCE MACROS
    // ========================================================================

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BULLsEYEProcessor)
};
