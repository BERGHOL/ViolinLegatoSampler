/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ViolinLegatoSamplerAudioProcessorEditor::ViolinLegatoSamplerAudioProcessorEditor(ViolinLegatoSamplerAudioProcessor& p)
    : AudioProcessorEditor(&p),
    audioProcessor(p),
    keyboardComponent(audioProcessor.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    addAndMakeVisible(keyboardComponent);
    setSize(600, 200);
}

ViolinLegatoSamplerAudioProcessorEditor::~ViolinLegatoSamplerAudioProcessorEditor()
{
}

//==============================================================================
void ViolinLegatoSamplerAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void ViolinLegatoSamplerAudioProcessorEditor::resized()
{
    keyboardComponent.setBounds(getLocalBounds());
}
