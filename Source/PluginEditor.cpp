#include "PluginEditor.h"

MT2PluginEditor::MT2PluginEditor(MT2Plugin& p)
    : AudioProcessorEditor(p),
      processorRef(p),
      apvts(p.apvts)
{
    // Setup all sliders
    for (auto* slider : std::vector<juce::Slider*>{
         &distSlider, &levelSlider, &diodeMorphSlider, &diodeMorph2Slider,
         &eqLowSlider, &eqMidSlider, &eqMidFreqSlider, &eqMidQSlider, &eqHighSlider,
         &outSatSlider})
    {
        slider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
        addAndMakeVisible(slider);
    }

    // Clip Mode and Sat Position - discrete sliders with value display
    for (auto* slider : std::vector<juce::Slider*>{&clipModeSlider, &satPosSlider})
    {
        slider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
        addAndMakeVisible(slider);
    }

    // Set up value display for discrete sliders
    clipModeSlider.textFromValueFunction = [](double value) {
        int v = (int)std::round(value);
        const char* names[] = {"Diode", "Tanh", "Atan", "Hard", "Asym", "Fold"};
        if (v >= 0 && v < 6) return juce::String(names[v]);
        return juce::String((int)value);
    };
    clipModeSlider.setValue(0.0);

    satPosSlider.textFromValueFunction = [](double value) {
        int v = (int)std::round(value);
        const char* names[] = {"Pre", "Post", "Off"};
        if (v >= 0 && v < 3) return juce::String(names[v]);
        return juce::String((int)value);
    };
    satPosSlider.setValue(1.0);

    outSatSlider.textFromValueFunction = [](double value) {
        if (value < 0.01) return juce::String("OFF");
        return juce::String((int)(value * 100)) + "%";
    };

    // Diode morph display
    auto diodeTextFromValue = [](double value) {
        if (value < 0.125) return juce::String("Si");
        if (value < 0.375) return juce::String("Ge");
        if (value < 0.625) return juce::String("LED");
        if (value < 0.875) return juce::String("Sch");
        return juce::String("Off");
    };
    diodeMorphSlider.textFromValueFunction = diodeTextFromValue;
    diodeMorph2Slider.textFromValueFunction = diodeTextFromValue;

    // Start timer for diode link functionality
    startTimerHz(30);  // Check 30 times per second

    // Toggle button
    diodeLinkButton.setButtonText("Link");
    addAndMakeVisible(diodeLinkButton);

    // Labels
    for (auto* label : std::vector<juce::Label*>{
         &distLabel, &levelLabel, &diodeMorphLabel, &diodeMorph2Label,
         &eqLowLabel, &eqMidLabel, &eqMidFreqLabel, &eqMidQLabel, &eqHighLabel,
         &clipModeLabel, &satPosLabel, &outSatLabel})
    {
        label->setJustificationType(juce::Justification::centred);
        label->setFont(juce::Font(11.0f));
        addAndMakeVisible(label);
    }

    // Attach parameters
    distAttachment      = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "dist", distSlider);
    levelAttachment     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "level", levelSlider);
    diodeMorphAttachment= std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "diode_morph", diodeMorphSlider);
    diodeLinkAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, "diode_link", diodeLinkButton);
    diodeMorph2Attachment=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "diode_morph_2", diodeMorph2Slider);
    eqLowAttachment     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "eq_low", eqLowSlider);
    eqMidAttachment     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "eq_mid", eqMidSlider);
    eqMidFreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "eq_mid_freq", eqMidFreqSlider);
    eqMidQAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "eq_mid_q", eqMidQSlider);
    eqHighAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "eq_high", eqHighSlider);
    clipModeAttachment  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "clip_mode", clipModeSlider);
    satPosAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "sat_pos", satPosSlider);
    outSatAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "out_sat", outSatSlider);

    setSize(540, 460);
}

MT2PluginEditor::~MT2PluginEditor()
{
    stopTimer();
}

void MT2PluginEditor::timerCallback()
{
    // Check if link is enabled
    auto* diodeLinkParam = apvts.getParameter("diode_link");
    if (!diodeLinkParam) return;

    bool isLinked = diodeLinkParam->getValue() > 0.5f;

    if (isLinked) {
        // Sync diode2 to diode1
        auto* diodeMorphParam = apvts.getParameter("diode_morph");
        if (diodeMorphParam) {
            float value = diodeMorphParam->getValue();
            diodeMorph2Slider.setValue(value, juce::dontSendNotification);
        }
    }

    // Enable/disable diode2 based on link state
    diodeMorph2Slider.setEnabled(!isLinked);
    diodeMorph2Label.setEnabled(!isLinked);
}

void MT2PluginEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);

    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(16.0f, juce::Font::bold));
    g.drawText("MetalCosmos", 0, 8, getWidth(), 25, juce::Justification::centred);

    // Section dividers
    g.setColour(juce::Colours::lightgrey);
    g.drawHorizontalLine(165, 0, getWidth());  // After Dist section
    g.drawHorizontalLine(295, 0, getWidth());  // After EQ section
}

void MT2PluginEditor::resized()
{
    auto area = getLocalBounds();
    area.removeFromTop(35); // title space

    int knobWidth = 70;
    int knobHeight = 85;
    int gap = 8;
    int labelHeight = 18;

    // Distortion section (top) - 5 items: 3 knobs + button + 1 knob
    auto distArea = area.removeFromTop(130);
    auto distKnobs = distArea.reduced(8);

    int xPos = distKnobs.getX();
    int yPos = distKnobs.getY();

    distSlider.setBounds(xPos, yPos, knobWidth, knobHeight);
    distLabel.setBounds(xPos, yPos + knobHeight, knobWidth, labelHeight);
    xPos += knobWidth + gap;

    levelSlider.setBounds(xPos, yPos, knobWidth, knobHeight);
    levelLabel.setBounds(xPos, yPos + knobHeight, knobWidth, labelHeight);
    xPos += knobWidth + gap;

    diodeMorphSlider.setBounds(xPos, yPos, knobWidth, knobHeight);
    diodeMorphLabel.setBounds(xPos, yPos + knobHeight, knobWidth, labelHeight);
    xPos += knobWidth + gap;

    // Link button + Diode2
    diodeLinkButton.setBounds(xPos, yPos + 32, 50, 22);
    diodeMorph2Slider.setBounds(xPos + 55, yPos, knobWidth, knobHeight);
    diodeMorph2Label.setBounds(xPos + 55, yPos + knobHeight, knobWidth, labelHeight);

    // EQ section (middle) - 5 knobs
    auto eqArea = area.removeFromTop(130);
    auto eqKnobs = eqArea.reduced(8);

    xPos = eqKnobs.getX();
    yPos = eqKnobs.getY();

    eqLowSlider.setBounds(xPos, yPos, knobWidth, knobHeight);
    eqLowLabel.setBounds(xPos, yPos + knobHeight, knobWidth, labelHeight);
    xPos += knobWidth + gap;

    eqMidFreqSlider.setBounds(xPos, yPos, knobWidth, knobHeight);
    eqMidFreqLabel.setBounds(xPos, yPos + knobHeight, knobWidth, labelHeight);
    xPos += knobWidth + gap;

    eqMidSlider.setBounds(xPos, yPos, knobWidth, knobHeight);
    eqMidLabel.setBounds(xPos, yPos + knobHeight, knobWidth, labelHeight);
    xPos += knobWidth + gap;

    eqMidQSlider.setBounds(xPos, yPos, knobWidth, knobHeight);
    eqMidQLabel.setBounds(xPos, yPos + knobHeight, knobWidth, labelHeight);
    xPos += knobWidth + gap;

    eqHighSlider.setBounds(xPos, yPos, knobWidth, knobHeight);
    eqHighLabel.setBounds(xPos, yPos + knobHeight, knobWidth, labelHeight);

    // Output section (bottom) - 3 knobs
    auto outArea = area.removeFromTop(130);
    auto outKnobs = outArea.reduced(8);

    xPos = outKnobs.getX() + 35;
    yPos = outKnobs.getY() + 5;

    clipModeSlider.setBounds(xPos, yPos, knobWidth, knobHeight);
    clipModeLabel.setBounds(xPos, yPos + knobHeight, knobWidth, labelHeight);
    xPos += knobWidth + gap;

    satPosSlider.setBounds(xPos, yPos, knobWidth, knobHeight);
    satPosLabel.setBounds(xPos, yPos + knobHeight, knobWidth, labelHeight);
    xPos += knobWidth + gap;

    outSatSlider.setBounds(xPos, yPos, knobWidth, knobHeight);
    outSatLabel.setBounds(xPos, yPos + knobHeight, knobWidth, labelHeight);
}
