#pragma once

#include <JuceHeader.h>
#include "ViolinSampleSound.h"

class ViolinSamplerVoice : public juce::SynthesiserVoice
{
public:
    void setLegato(bool shouldLegato)
    {
        isLegato = shouldLegato;
    }

    bool canPlaySound(juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<ViolinSampleSound*>(sound) != nullptr;
    }

    void startNote(int midiNoteNumber, float velocity,
        juce::SynthesiserSound* sound,
        int) override
    {
        if (auto* s = dynamic_cast<ViolinSampleSound*>(sound))
        {
            isReleasing = false;

            // Sauvegarde ancien sample pour crossfade
            if (isLegato && currentSample != nullptr)
            {
                previousSample = currentSample;
                previousPosition = position;
                previousPitchRatio = pitchRatio;
                crossfadeActive = true;
                crossfadePos = 0;
            }
            else
            {
                crossfadeActive = false;
            }

            currentSample = s;

            // OFFSET LEGATO
            if (isLegato)
                position = findZeroCrossing(s->getAudioData(), 0.15 * s->getSourceSampleRate());
            else
                position = 0.0;

            pitchRatio = std::pow(2.0,
                (midiNoteNumber - s->getRootNote()) / 12.0)
                * (s->getSourceSampleRate() / getSampleRate());

            level = velocity;

            attackPosition = 0;
            attackLength = (int)(0.008 * getSampleRate()); // 8 ms

            if (isLegato)
                DBG("LEGATO NOTE");
            else
                DBG("NORMAL NOTE");
        }
    }

    void stopNote(float, bool allowTailOff) override
    {
        if (allowTailOff)
        {
            isReleasing = true;
            releasePosition = 0;
            releaseLength = (int)(0.04 * getSampleRate()); // 40 ms
        }
        else
        {
            clearCurrentNote();
            currentSample = nullptr;
            previousSample = nullptr;
            isReleasing = false;
        }
    }

    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}

    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
        int startSample, int numSamples) override
    {
        if (currentSample == nullptr)
            return;

        const auto& currentData = currentSample->getAudioData();

        for (int i = 0; i < numSamples; ++i)
        {
            float outputSample = 0.0f;

            // New SAMPLE
            int pos = (int)position;

            if (pos < currentData.getNumSamples())
            {
                float newSample = currentData.getSample(0, pos);

                float newGain = 1.0f;

                if (crossfadeActive && crossfadePos < crossfadeLength)
                    newGain = (float)crossfadePos / (float)crossfadeLength;

                outputSample += newSample * newGain * 0.8f;
            }

            // crossfade
            if (crossfadeActive && previousSample != nullptr)
            {
                const auto& prevData = previousSample->getAudioData();
                int prevPos = (int)previousPosition;

                if (prevPos < prevData.getNumSamples())
                {
                    float oldSample = prevData.getSample(0, prevPos);

                    float oldGain = 1.0f - ((float)crossfadePos / (float)crossfadeLength);

                    outputSample += oldSample * oldGain * 0.8f;

                    previousPosition += previousPitchRatio;
                }
            }

            if (isReleasing)
            {
                float releaseGain = 1.0f - ((float)releasePosition / (float)releaseLength);
                outputSample *= juce::jlimit(0.0f, 1.0f, releaseGain);

                releasePosition++;

                if (releasePosition >= releaseLength)
                {
                    clearCurrentNote();
                    currentSample = nullptr;
                    previousSample = nullptr;
                    isReleasing = false;
                    break;
                }
            }

            float attackGain = 1.0f;

            if (attackPosition < attackLength)
            {
                attackGain = (float)attackPosition / (float)attackLength;
                attackPosition++;
            }

            outputSample *= attackGain;

            outputSample *= level;

            for (int ch = 0; ch < outputBuffer.getNumChannels(); ++ch)
                outputBuffer.addSample(ch, startSample + i, outputSample);

            position += pitchRatio;

            // progression crossfade
            if (crossfadeActive)
            {
                crossfadePos++;

                if (crossfadePos >= crossfadeLength)
                {
                    crossfadeActive = false;
                    previousSample = nullptr;
                }
            }
        }
    }

    void setSampleRateForCrossfade(double sampleRate)
    {
        crossfadeLength = (int)(0.18 * sampleRate); // 180 ms
    }

    void legatoTo(juce::SynthesiserSound* sound, int midiNoteNumber, float velocity)
    {
        setLegato(true);
        startNote(midiNoteNumber, velocity, sound, 0);
    }

private:
    ViolinSampleSound* currentSample = nullptr;
    ViolinSampleSound* previousSample = nullptr;

    double position = 0.0;
    double previousPosition = 0.0;

    double pitchRatio = 1.0;
    float level = 0.0f;

    bool isLegato = false;

    int attackPosition = 0;
    int attackLength = 1;

    // crossfade
    bool crossfadeActive = false;
    int crossfadePos = 0;
    int crossfadeLength = 0;

    bool isReleasing = false;
    int releasePosition = 0;
    int releaseLength = 1;

    double findZeroCrossing(const juce::AudioBuffer<float>& data, double startPosition)
    {
        int start = (int)startPosition;
        int maxSearch = juce::jmin(start + 2000, data.getNumSamples() - 1);

        for (int i = start; i < maxSearch; ++i)
        {
            float a = data.getSample(0, i);
            float b = data.getSample(0, i + 1);

            if ((a <= 0.0f && b >= 0.0f) || (a >= 0.0f && b <= 0.0f))
                return (double)i;
        }

        return startPosition;
    }

    double previousPitchRatio = 1.0;
};