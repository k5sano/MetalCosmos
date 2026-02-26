#include "PluginEditor.h"

MT2PluginEditor::MT2PluginEditor(MT2Plugin& p)
    : AudioProcessorEditor(p), processor(p), genericEditor(p)
{
    addAndMakeVisible(genericEditor);
    setSize(400, 300);
}

MT2PluginEditor::~MT2PluginEditor() {}

void MT2PluginEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void MT2PluginEditor::resized()
{
    genericEditor.setBounds(getLocalBounds());
}
