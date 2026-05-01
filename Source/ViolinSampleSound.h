#pragma once

#include <JuceHeader.h>

class ViolinSampleSound : public juce::SynthesiserSound
{
public:
    ViolinSampleSound(int midiNote,
        const juce::AudioBuffer<float>& data,
        double sampleRate)
        : rootMidiNote(midiNote),
        audioData(data),
        sourceSampleRate(sampleRate)
    {}

    bool appliesToNote(int midiNoteNumber) override
    {
        return midiNoteNumber == rootMidiNote;
    }

    bool appliesToChannel(int midiChannel) override
    {
        juce::ignoreUnused(midiChannel);
        return true;
    }

    int getRootNote() const { return rootMidiNote; }

    const juce::AudioBuffer<float>& getAudioData() const { return audioData; }

    double getSourceSampleRate() const { return sourceSampleRate; }

private:
    int rootMidiNote = 60;
    juce::AudioBuffer<float> audioData;
    double sourceSampleRate = 44100.0;
};