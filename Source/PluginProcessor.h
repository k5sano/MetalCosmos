#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "Parameters.h"
#include "DSP/MT2GainStage.h"
#include "DSP/MT2ToneStack.h"
#include "DSP/DiodeMorpher.h"

class MT2Plugin : public juce::AudioProcessor {
public:
    MT2Plugin();
    ~MT2Plugin() override = default;

    bool isBusesLayoutSupported(const juce::AudioProcessor::BusesLayout& layouts) const override
    {
        // Only support stereo input/output
        return layouts.getMainInputChannelSet() == juce::AudioChannelSet::stereo()
            && layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
    }

    void prepareToPlay(double sampleRate, int maxSamplesPerBlock) override;
    void releaseResources() override;
    void reset() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "MetalCosmos"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

    juce::AudioProcessorValueTreeState apvts;

    // Parameter pointers (for fast access)
    std::atomic<float>* clipMode = nullptr;
    std::atomic<float>* outSat = nullptr;

private:
    // DSP modules
    MT2GainStage mGainStage;
    MT2ToneStack mToneStack;
    DiodeMorpher mDiodeMorpher;

    // Parameter smoothing (to prevent clicks)
    juce::SmoothedValue<double, juce::ValueSmoothingTypes::Linear> mSmoothedGain;
    juce::SmoothedValue<double, juce::ValueSmoothingTypes::Linear> mSmoothedLevel;

    // Temp buffers for double precision processing
    juce::AudioBuffer<double> mBufferDouble;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MT2Plugin)
};
