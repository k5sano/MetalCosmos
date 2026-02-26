#include "PluginEditor.h"

MT2PluginEditor::MT2PluginEditor(MT2Plugin& p)
    : AudioProcessorEditor(p), processor(p), genericEditor(p)
{
    addAndMakeVisible(genericEditor);
    setSize(400, 300);
}

void MT2PluginEditor::resized()
{
    genericEditor.setBounds(getLocalBounds());
}
