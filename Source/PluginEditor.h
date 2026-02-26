#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

class MT2PluginEditor : public juce::AudioProcessorEditor
{
public:
    explicit MT2PluginEditor(MT2Plugin& p);
    ~MT2PluginEditor() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    MT2Plugin& processor;
    juce::GenericAudioProcessorEditor genericEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MT2PluginEditor)
};
