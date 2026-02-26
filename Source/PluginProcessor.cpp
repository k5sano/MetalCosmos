#include "PluginProcessor.h"
#include "PluginEditor.h"

MT2Plugin::MT2Plugin()
    : AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo())
        .withOutput("Output", juce::AudioChannelSet::stereo())),
      apvts(*this, nullptr, "Parameters", MT2Params::createLayout())
{
}

MT2Plugin::~MT2Plugin() {}

void MT2Plugin::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(sampleRate, samplesPerBlock);
}

void MT2Plugin::releaseResources() {}

void MT2Plugin::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    // Phase 1: Pass-through - input goes directly to output
    juce::ignoreUnused(buffer);
}

juce::AudioProcessorEditor* MT2Plugin::createEditor()
{
    return new MT2PluginEditor(*this);
}

bool MT2Plugin::hasEditor() const
{
    return true;
}

const juce::String MT2Plugin::getName() const
{
    return "MetalCosmos";
}

bool MT2Plugin::acceptsMidi() const
{
    return false;
}

bool MT2Plugin::producesMidi() const
{
    return false;
}

bool MT2Plugin::isMidiEffect() const
{
    return false;
}

double MT2Plugin::getTailLengthSeconds() const
{
    return 0.0;
}

int MT2Plugin::getNumPrograms()
{
    return 1;
}

int MT2Plugin::getCurrentProgram()
{
    return 0;
}

void MT2Plugin::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String MT2Plugin::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void MT2Plugin::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

void MT2Plugin::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void MT2Plugin::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MT2Plugin();
}
