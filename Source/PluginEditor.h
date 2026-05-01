/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class ViolinLegatoSamplerAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    ViolinLegatoSamplerAudioProcessorEditor (ViolinLegatoSamplerAudioProcessor&);
    ~ViolinLegatoSamplerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ViolinLegatoSamplerAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ViolinLegatoSamplerAudioProcessorEditor)
    juce::MidiKeyboardState keyboardState;
    juce::MidiKeyboardComponent keyboardComponent;
};
