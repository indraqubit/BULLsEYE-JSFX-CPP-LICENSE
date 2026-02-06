#include "PluginProcessor.h"
#include "PluginEditor.h"

// ========================================================================
// PARAMETER LAYOUT
// ========================================================================

juce::AudioProcessorValueTreeState::ParameterLayout YourProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    // TODO: Add your parameters here
    
    // Example: Boolean parameter
    // params.push_back(std::make_unique<juce::AudioParameterBool>(
    //     ModelSSOT::ParameterIDs::YOUR_PARAM,
    //     ModelSSOT::ParameterNames::YOUR_PARAM,
    //     false  // default value
    // ));
    
    // Example: Choice parameter
    // params.push_back(std::make_unique<juce::AudioParameterChoice>(
    //     ModelSSOT::ParameterIDs::YOUR_MODE,
    //     ModelSSOT::ParameterNames::YOUR_MODE,
    //     juce::StringArray("Mode A", "Mode B", "Mode C"),
    //     0  // default index
    // ));
    
    // Example: Float parameter
    // params.push_back(std::make_unique<juce::AudioParameterFloat>(
    //     ModelSSOT::ParameterIDs::YOUR_GAIN,
    //     ModelSSOT::ParameterNames::YOUR_GAIN,
    //     juce::NormalisableRange<float>(-60.0f, 6.0f, 0.1f, 0.3f),  // min, max, step, skew
    //     0.0f  // default value
    // ));
    
    return { params.begin(), params.end() };
}

// ========================================================================
// CONSTRUCTOR
// ========================================================================

YourProcessor::YourProcessor()
    : apvts(*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

YourProcessor::~YourProcessor()
{
}

// ========================================================================
// PREPARE TO PLAY
// ========================================================================

void YourProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(sampleRate, samplesPerBlock);
    
    // Reset DSP state
    dspCore.reset();
    
    // TODO: Add any sample-rate dependent initialization
}

void YourProcessor::releaseResources()
{
    // TODO: Release any resources (open files, etc.)
}

// ========================================================================
// CHANNEL CONFIGURATION
// ========================================================================

#ifndef JucePlugin_PreferredChannelConfigurations
bool YourProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    // TODO: Customize for your plugin
    
    // Example: Support mono and stereo
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}
#endif

// ========================================================================
// PROCESS BLOCK
// ========================================================================

void YourProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);
    juce::ScopedNoDenormals noDenormals;
    
    // TODO: Implement your audio processing
    
    // Example: Read parameters
    // const bool yourParam = *apvts.getRawParameterValue(ModelSSOT::ParameterIDs::YOUR_PARAM) > 0.5f;
    // const int yourMode = static_cast<int>(*apvts.getRawParameterValue(ModelSSOT::ParameterIDs::YOUR_MODE));
    
    // Example: Update DSP state
    // dspCore.setMode(static_cast<ModelSSOT::YourMode>(yourMode));
    
    // Example: Process audio
    // for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    // {
    //     auto* data = buffer.getWritePointer(ch);
    //     for (int i = 0; i < buffer.getNumSamples(); ++i)
    //     {
    //         dspCore.process(data[i]);
    //     }
    // }
}

// ========================================================================
// EDITOR
// ========================================================================

juce::AudioProcessorEditor* YourProcessor::createEditor()
{
    return new YourEditor(*this);
}

bool YourProcessor::hasEditor() const
{
    return true;
}

// ========================================================================
// PLUGIN INFO
// ========================================================================

const juce::String YourProcessor::getName() const
{
    return JucePlugin_Name;
}

bool YourProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool YourProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool YourProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double YourProcessor::getTailLengthSeconds() const
{
    // TODO: Set appropriate tail length
    return 0.0;
}

// ========================================================================
// PROGRAMS
// ========================================================================

int YourProcessor::getNumPrograms()
{
    return 1;  // TODO: Set number of programs
}

int YourProcessor::getCurrentProgram()
{
    return 0;
}

void YourProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String YourProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void YourProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

// ========================================================================
// STATE
// ========================================================================

void YourProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // TODO: Implement state saving
    
    // Example: Save APVTS state
    // auto state = apvts.copyState();
    // std::unique_ptr<juce::XmlElement> xml(state.createXml());
    // copyXmlToBinary(*xml, destData);
}

void YourProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // TODO: Implement state loading
    
    // Example: Load APVTS state
    // std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    // if (xmlState.get() != nullptr && xmlState->hasTagName(apvts.state.getType()))
    // {
    //     apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
    // }
}

// ========================================================================
// FACTORY
// ========================================================================

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new YourProcessor();
}
