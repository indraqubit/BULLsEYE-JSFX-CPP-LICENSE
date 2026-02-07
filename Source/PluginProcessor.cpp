#include "PluginProcessor.h"
#include "PluginEditor.h"

// ========================================================================
// CONSTRUCTOR / DESTRUCTOR
// ========================================================================

BULLsEYEProcessor::BULLsEYEProcessor()
    : juce::AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true))
    , apvts(*this, nullptr, "Parameters", createParameterLayout())
{
}

BULLsEYEProcessor::~BULLsEYEProcessor() = default;

// ========================================================================
// PARAMETER LAYOUT
// ========================================================================

juce::AudioProcessorValueTreeState::ParameterLayout BULLsEYEProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // Content Type (Choice parameter)
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        ModelSSOT::ParameterIDs::CONTENT_TYPE,
        ModelSSOT::ParameterNames::CONTENT_TYPE,
        juce::StringArray({
            "Music Non-drums (-11 LUFS)",
            "Music Drums (-8 LUFS)",
            "Cinema/Trailer (-14 LUFS)"
        }),
        1  // Default: Music Drums
    ));

    return layout;
}

// ========================================================================
// PARAMETER CALLBACKS
// ========================================================================

void BULLsEYEProcessor::contentTypeChanged()
{
    int choiceIndex = apvts.getRawParameterValue(ModelSSOT::ParameterIDs::CONTENT_TYPE)->load();
    ModelSSOT::ContentType type = ModelSSOT::Helpers::intToContentType(choiceIndex);
    dspCore.setContentType(type);
}

// ========================================================================
// JUCE LIFECYCLE
// ========================================================================

void BULLsEYEProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Initialize DSP core
    dspCore.setSampleRate(sampleRate);
    dspCore.reset();

    // Reset transport state tracking
    wasPlaying = false;

    // Notify parameter changes
    contentTypeChanged();
}

void BULLsEYEProcessor::releaseResources()
{
    // Nothing specific to release
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BULLsEYEProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    // Only support stereo
    if (layouts.getMainInputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return true;
}
#endif

// ========================================================================
// PROCESSING
// ========================================================================

void BULLsEYEProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    // Transport state detection using DAW playhead (reliable across all DAWs)
    // Matches JSFX behavior: @init called on transport stop→play
    bool isCurrentlyPlaying = false;
    
    if (auto* playHead = getPlayHead())
    {
        if (auto position = playHead->getPosition())
        {
            isCurrentlyPlaying = position->getIsPlaying();
            
            // Detect stopped→playing transition
            if (isCurrentlyPlaying && !wasPlaying)
            {
                // Transport just started: reset all measurements
                dspCore.reset();
            }
            
            wasPlaying = isCurrentlyPlaying;
        }
    }
    
    // Poll content type parameter each block (equivalent to JSFX @slider)
    contentTypeChanged();

    // Get input and output pointers
    float* leftIn = buffer.getWritePointer(0);
    float* rightIn = buffer.getWritePointer(1);

    // Process each sample
    for (int i = 0; i < buffer.getNumSamples(); i++)
    {
        dspCore.process(leftIn[i], rightIn[i]);
    }
}

// ========================================================================
// STATE
// ========================================================================

void BULLsEYEProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // Save APVTS state
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void BULLsEYEProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // Restore APVTS state
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));

    if (xml && xml->hasTagName(apvts.state.getType()))
    {
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
        contentTypeChanged();
    }
}

// ========================================================================
// FACTORY
// ========================================================================

// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BULLsEYEProcessor();
}

// ========================================================================
// EDITOR
// ========================================================================

juce::AudioProcessorEditor* BULLsEYEProcessor::createEditor()
{
    return new BULLsEYEEditor(*this);
}
