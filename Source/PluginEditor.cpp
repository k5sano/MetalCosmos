#include "PluginEditor.h"

MT2PluginEditor::MT2PluginEditor(MT2Plugin& p)
    : AudioProcessorEditor(p),
      processorRef(p),
      apvts(p.apvts)
{
    // Setup clip mode combo box first
    clipModeCombo.addItem("Diode", 1);
    clipModeCombo.addItem("Tanh", 2);
    clipModeCombo.addItem("Atan", 3);
    clipModeCombo.addItem("Hard", 4);
    clipModeCombo.addItem("Asymmetric", 5);
    clipModeCombo.addItem("Foldback", 6);
    clipModeCombo.setSelectedId(1);
    addAndMakeVisible(clipModeCombo);

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

    // Toggle button
    addAndMakeVisible(diodeLinkButton);

    // Labels
    for (auto* label : std::vector<juce::Label*>{
         &distLabel, &levelLabel, &diodeMorphLabel, &diodeMorph2Label,
         &eqLowLabel, &eqMidLabel, &eqMidFreqLabel, &eqMidQLabel, &eqHighLabel,
         &clipModeLabel, &outSatLabel})
    {
        label->setJustificationType(juce::Justification::centred);
        label->setFont(juce::Font(12.0f));
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
    clipModeAttachment  = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, "clip_mode", clipModeCombo);
    outSatAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "out_sat", outSatSlider);

    setSize(500, 400);
}

void MT2PluginEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);

    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(16.0f, juce::Font::bold));
    g.drawText("MetalCosmos", 0, 10, getWidth(), 30, juce::Justification::centred);

    // Section dividers
    g.setColour(juce::Colours::lightgrey);
    g.drawHorizontalLine(185, 0, getWidth());
}

void MT2PluginEditor::resized()
{
    auto area = getLocalBounds();
    area.removeFromTop(40); // title space

    int knobWidth = 70;
    int knobHeight = 85;
    int gap = 10;
    int labelHeight = 20;

    // Distortion section (top)
    auto distArea = area.removeFromTop(145);
    auto distKnobs = distArea.reduced(10);

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

    diodeLinkButton.setBounds(xPos, yPos + 30, 60, 25);
    diodeMorph2Slider.setBounds(xPos + 70, yPos, knobWidth, knobHeight);
    diodeMorph2Label.setBounds(xPos + 70, yPos + knobHeight, knobWidth, labelHeight);

    // EQ section (middle)
    auto eqArea = area.removeFromTop(145);
    auto eqKnobs = eqArea.reduced(10);

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

    // Output section (bottom)
    auto outArea = area.removeFromTop(60);
    int comboY = outArea.getY() + 5;
    clipModeLabel.setBounds(100, comboY, 80, 20);
    clipModeCombo.setBounds(100, comboY + 20, 120, 25);
    outSatLabel.setBounds(300, comboY, 80, 20);
    outSatSlider.setBounds(300, comboY, knobWidth, knobHeight);
}
