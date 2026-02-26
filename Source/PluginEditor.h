#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

class MT2PluginEditor : public juce::AudioProcessorEditor {
public:
    explicit MT2PluginEditor(MT2Plugin&);
    ~MT2PluginEditor() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    MT2Plugin& processorRef;
    juce::AudioProcessorValueTreeState& apvts;

    // Distortion section
    juce::Slider distSlider;
    juce::Slider levelSlider;
    juce::Slider diodeMorphSlider;
    juce::ToggleButton diodeLinkButton{"Link"};
    juce::Slider diodeMorph2Slider;

    // EQ section
    juce::Slider eqLowSlider;
    juce::Slider eqMidSlider;
    juce::Slider eqMidFreqSlider;
    juce::Slider eqMidQSlider;
    juce::Slider eqHighSlider;

    // Output section
    juce::Slider clipModeSlider;
    juce::Slider satPosSlider;
    juce::Slider outSatSlider;

    // Attachments (connect UI to parameters)
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> levelAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> diodeMorphAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> diodeLinkAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> diodeMorph2Attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> eqLowAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> eqMidAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> eqMidFreqAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> eqMidQAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> eqHighAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> clipModeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> satPosAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outSatAttachment;

    // Labels
    juce::Label distLabel{"Dist", "Dist"};
    juce::Label levelLabel{"Level", "Level"};
    juce::Label diodeMorphLabel{"Diode1", "Diode1"};
    juce::Label diodeMorph2Label{"Diode2", "Diode2"};
    juce::Label eqLowLabel{"Low", "Low"};
    juce::Label eqMidLabel{"Mid", "Mid"};
    juce::Label eqMidFreqLabel{"Freq", "Freq"};
    juce::Label eqMidQLabel{"Q", "Q"};
    juce::Label eqHighLabel{"High", "High"};
    juce::Label clipModeLabel{"Clip", "Clip Mode"};
    juce::Label satPosLabel{"Pos", "Sat Pos"};
    juce::Label outSatLabel{"Sat", "Sat"};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MT2PluginEditor)
};
