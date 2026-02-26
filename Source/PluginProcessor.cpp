#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

namespace {
    // Helper: Apply tanh saturation to double buffer
    void applySaturationDouble(juce::AudioBuffer<double>& buf, float amount)
    {
        if (amount < 0.01f) return;
        const int numChannels = buf.getNumChannels();
        const int numSamples = buf.getNumSamples();
        double drive = 1.0 + static_cast<double>(amount) * 3.0;
        double norm = 1.0 / std::tanh(drive);

        for (int ch = 0; ch < numChannels; ++ch) {
            auto* data = buf.getWritePointer(ch);
            for (int i = 0; i < numSamples; ++i) {
                data[i] = std::tanh(data[i] * drive) * norm;
            }
        }
    }

    // Helper: Apply tanh saturation to float buffer
    void applySaturationFloat(juce::AudioBuffer<float>& buf, float amount)
    {
        if (amount < 0.01f) return;
        const int numChannels = buf.getNumChannels();
        const int numSamples = buf.getNumSamples();
        double drive = 1.0 + static_cast<double>(amount) * 3.0;
        double norm = 1.0 / std::tanh(drive);

        for (int ch = 0; ch < numChannels; ++ch) {
            auto* data = buf.getWritePointer(ch);
            for (int i = 0; i < numSamples; ++i) {
                double driven = static_cast<double>(data[i]) * drive;
                data[i] = static_cast<float>(std::tanh(driven) * norm);
            }
        }
    }
}

MT2Plugin::MT2Plugin()
    : AudioProcessor(BusesProperties()
          .withInput("Input", juce::AudioChannelSet::stereo(), true)
          .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "PARAMETERS", MT2Params::createLayout())
{
    clipMode = apvts.getRawParameterValue("clip_mode");
    outSat = apvts.getRawParameterValue("out_sat");
    satPos = apvts.getRawParameterValue("sat_pos");
}

void MT2Plugin::prepareToPlay(double sampleRate, int maxSamplesPerBlock)
{
    // Prepare DSP modules (no oversampling for now due to auval issues)
    mGainStage.prepare(sampleRate);
    mToneStack.prepare(sampleRate);

    // Prepare smoothed values
    mSmoothedGain.reset(sampleRate, 0.01);
    mSmoothedLevel.reset(sampleRate, 0.01);

    // Prepare double buffer (always 2 channels for stereo)
    mBufferDouble.setSize(2, maxSamplesPerBlock);

    // Report latency (no oversampling = 0 latency)
    setLatencySamples(0);
}

void MT2Plugin::releaseResources()
{
}

void MT2Plugin::reset()
{
    mGainStage.reset();
    mToneStack.reset();
    mSmoothedGain.reset(0.0);
    mSmoothedLevel.reset(0.0);
}

void MT2Plugin::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    // Get parameter values (atomic read)
    auto* distParam = apvts.getRawParameterValue("dist");
    auto* levelParam = apvts.getRawParameterValue("level");
    auto* diodeMorphParam = apvts.getRawParameterValue("diode_morph");
    auto* diodeLinkParam = apvts.getRawParameterValue("diode_link");
    auto* diodeMorph2Param = apvts.getRawParameterValue("diode_morph_2");
    auto* eqLowParam = apvts.getRawParameterValue("eq_low");
    auto* eqMidParam = apvts.getRawParameterValue("eq_mid");
    auto* eqMidFreqParam = apvts.getRawParameterValue("eq_mid_freq");
    auto* eqMidQParam = apvts.getRawParameterValue("eq_mid_q");
    auto* eqHighParam = apvts.getRawParameterValue("eq_high");

    // Map dist parameter (0.0~1.0) to gain (5.6~200)
    float distValue = distParam ? distParam->load() : 0.5f;
    double gain = 5.6 * std::pow(200.0 / 5.6, distValue);

    // Map level parameter (0.0~1.0) to output level
    float levelValue = levelParam ? levelParam->load() : 0.5f;
    double outputLevel = levelValue * 2.0;

    // Saturator position and amount
    int satPosition = (satPos != nullptr) ? (int)std::round(satPos->load()) : 1;  // 0=Pre, 1=Post, 2=Off
    float satAmount = (outSat != nullptr) ? outSat->load() : 0.0f;

    // Update smoothed values
    mSmoothedGain.setTargetValue(gain);
    mSmoothedLevel.setTargetValue(outputLevel);

    // Update diode parameters
    float diodeMorph = diodeMorphParam ? diodeMorphParam->load() : 0.0f;
    bool diodeLink = diodeLinkParam ? diodeLinkParam->load() > 0.5f : true;
    float diodeMorph2 = diodeMorph2Param ? diodeMorph2Param->load() : 0.0f;

    auto stage1Params = mDiodeMorpher.getMorphedParams(diodeMorph);
    mGainStage.setStage1Diode(stage1Params.is, stage1Params.n, stage1Params.noClip);

    if (diodeLink) {
        mGainStage.setStage2Diode(stage1Params.is, stage1Params.n, stage1Params.noClip);
    } else {
        auto stage2Params = mDiodeMorpher.getMorphedParams(diodeMorph2);
        mGainStage.setStage2Diode(stage2Params.is, stage2Params.n, stage2Params.noClip);
    }

    // Update gain stage gain
    mGainStage.setGain(mSmoothedGain.getNextValue());

    // Set clip mode
    int mode = (clipMode != nullptr) ? (int)std::round(clipMode->load()) : 0;
    mGainStage.setClipMode(mode);

    // Update EQ coefficients (once per block)
    float eqLow = eqLowParam ? eqLowParam->load() : 0.5f;
    float eqMid = eqMidParam ? eqMidParam->load() : 0.5f;
    float eqMidFreq = eqMidFreqParam ? eqMidFreqParam->load() : 0.5f;
    float eqMidQ = eqMidQParam ? eqMidQParam->load() : 0.3f;
    float eqHigh = eqHighParam ? eqHighParam->load() : 0.5f;
    mToneStack.updateCoefficients(eqLow, eqMid, eqMidFreq, eqMidQ, eqHigh);

    // Get buffer info
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    // Always use 2 channels (stereo)
    mBufferDouble.setSize(2, numSamples, false, false, true);

    if (numChannels == 1) {
        // Mono input: copy to both channels
        for (int sample = 0; sample < numSamples; ++sample) {
            double val = static_cast<double>(buffer.getReadPointer(0)[sample]);
            mBufferDouble.getWritePointer(0)[sample] = val;
            mBufferDouble.getWritePointer(1)[sample] = val;
        }
    } else {
        // Stereo/multi-channel input
        for (int ch = 0; ch < juce::jmin(numChannels, 2); ++ch) {
            std::copy(buffer.getReadPointer(ch),
                      buffer.getReadPointer(ch) + numSamples,
                      mBufferDouble.getWritePointer(ch));
        }
        // If more than 2 channels, copy extra channels to channel 1
        for (int ch = 2; ch < numChannels; ++ch) {
            std::copy(buffer.getReadPointer(ch),
                      buffer.getReadPointer(ch) + numSamples,
                      mBufferDouble.getWritePointer(1));
        }
    }

    // --- Pre: Apply saturation BEFORE GainStage ---
    if (satPosition == 0 && satAmount > 0.01f) {
        applySaturationDouble(mBufferDouble, satAmount);
    }

    // Process DSP (no oversampling for now)
    for (int sample = 0; sample < numSamples; ++sample) {
        double level = mSmoothedLevel.getNextValue();

        for (int ch = 0; ch < 2; ++ch) {
            double x = mBufferDouble.getReadPointer(ch)[sample];

            // Gain Stage (distortion)
            double gsOut = mGainStage.processSample(x);

            // Tone Stack (EQ)
            double eqOut = mToneStack.processSample(gsOut);

            // Store back (will be converted to float)
            mBufferDouble.getWritePointer(ch)[sample] = eqOut;
        }
    }

    // Convert double back to float
    if (numChannels == 1) {
        // Mono output: use left channel only
        for (int sample = 0; sample < numSamples; ++sample) {
            float val = static_cast<float>(mBufferDouble.getReadPointer(0)[sample]);
            buffer.getWritePointer(0)[sample] = val;
        }
    } else {
        // Stereo/multi-channel output
        for (int ch = 0; ch < juce::jmin(numChannels, 2); ++ch) {
            for (int sample = 0; sample < numSamples; ++sample) {
                buffer.getWritePointer(ch)[sample] = static_cast<float>(mBufferDouble.getReadPointer(ch)[sample]);
            }
        }
        // If more than 2 channels, copy channel 1 to extra channels
        for (int ch = 2; ch < numChannels; ++ch) {
            for (int sample = 0; sample < numSamples; ++sample) {
                buffer.getWritePointer(ch)[sample] = buffer.getWritePointer(1)[sample];
            }
        }
    }

    // --- Post: Apply saturation AFTER ToneStack (to float buffer) ---
    if (satPosition == 1 && satAmount > 0.01f) {
        applySaturationFloat(buffer, satAmount);
    }
    // satPosition == 2 (Off): No saturation applied
}

juce::AudioProcessorEditor* MT2Plugin::createEditor()
{
    return new MT2PluginEditor(*this);
}

void MT2Plugin::getStateInformation(juce::MemoryBlock& destData)
{
    if (auto xml = apvts.copyState().createXml())
        copyXmlToBinary(*xml, destData);
}

void MT2Plugin::setStateInformation(const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary(data, sizeInBytes))
        if (xml->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MT2Plugin();
}
