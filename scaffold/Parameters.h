#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

namespace MT2Params {

    inline juce::AudioProcessorValueTreeState::ParameterLayout createLayout() {
        std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

        // All parameters per spec.md
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"dist", 1}, "Dist",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"level", 1}, "Level",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"diode_morph", 1}, "Diode Morph",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.0f));

        params.push_back(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID{"diode_link", 1}, "Diode Link", true));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"diode_morph_2", 1}, "Diode Morph 2",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.0f));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"eq_low", 1}, "Low",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"eq_mid", 1}, "Mid Level",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"eq_mid_freq", 1}, "Mid Freq",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"eq_mid_q", 1}, "Mid Q",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.3f));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"eq_high", 1}, "High",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));

        // Clip Mode: Stage1/Stage2 の歪み方式 (0=Diode, 1=Tanh, 2=Atan, 3=Hard, 4=Asymmetric, 5=Foldback)
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"clip_mode", 1},
            "Clip Mode",
            juce::NormalisableRange<float>(0.0f, 5.0f, 1.0f),
            0.0f,
            juce::AudioParameterFloatAttributes{}
                .withStringFromValueFunction([](float v, int) {
                    const char* names[] = {
                        "Diode", "Tanh", "Atan", "Hard", "Asymmetric", "Foldback"
                    };
                    return juce::String(names[std::clamp((int)v, 0, 5)]);
                })
        ));

        // Output Saturation: 最終段 tanh の効き（0=OFF, 1=フル）
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"out_sat", 1},
            "Saturation",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
            0.3f,
            juce::AudioParameterFloatAttributes{}
                .withStringFromValueFunction([](float v, int) {
                    if (v < 0.01f) return juce::String("OFF");
                    return juce::String((int)(v * 100)) + "%";
                })
        ));

        // Saturator Position: tanh サチュレーターの配置 (0=Pre, 1=Post, 2=Off)
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"sat_pos", 1},
            "Sat Position",
            juce::NormalisableRange<float>(0.0f, 2.0f, 1.0f),
            1.0f,
            juce::AudioParameterFloatAttributes{}
                .withStringFromValueFunction([](float v, int) {
                    const char* names[] = {"Pre", "Post", "Off"};
                    return juce::String(names[std::clamp((int)v, 0, 2)]);
                })
        ));

        return { params.begin(), params.end() };
    }

} // namespace MT2Params
