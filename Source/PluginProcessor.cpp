/*
  ==============================================================================
    JUCE Plugin Processor
  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ViolinSamplerVoice.h"
#include "ViolinSampleSound.h"
#include "LegatoManager.h"

//==============================================================================
ViolinLegatoSamplerAudioProcessor::ViolinLegatoSamplerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    )
#endif
{
    formatManager.registerBasicFormats();

    // Voices
    synth.clearVoices();
    synth.addVoice(new ViolinSamplerVoice());

    // 🔥 CHEMIN ABSOLU (IMPORTANT)
    juce::File samplesFolder("C:/SUNY_plugins/ViolinLegatoSampler/ViolinLegatoSampler/Source/Samples/");

    DBG("Samples folder exists: " + juce::String(samplesFolder.exists() ? 1 : 0));

    auto files = samplesFolder.findChildFiles(juce::File::findFiles, false, "*.wav");

    DBG("Files found: " + juce::String(files.size()));

    for (auto& file : files)
    {
        std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));

        if (reader != nullptr)
        {
            juce::AudioBuffer<float> buffer(reader->numChannels,
                (int)reader->lengthInSamples);

            reader->read(&buffer, 0,
                (int)reader->lengthInSamples,
                0, true, true);

            juce::String name = file.getFileNameWithoutExtension();

            int midiNote = -1;

            if (name == "C4")  midiNote = 72;
            else if (name == "C#4") midiNote = 73;
            else if (name == "D4")  midiNote = 74;
            else if (name == "D#4") midiNote = 75;
            else if (name == "E4")  midiNote = 76;
            else if (name == "F4")  midiNote = 77;
            else if (name == "F#4") midiNote = 78;
            else if (name == "G4")  midiNote = 79;
            else if (name == "G#4") midiNote = 80;
            else if (name == "A4")  midiNote = 81;
            else if (name == "A#4") midiNote = 82;
            else if (name == "B4")  midiNote = 83;

            if (midiNote < 0)
                continue;

            DBG("Loaded: " + file.getFileName());

            synth.addSound(new ViolinSampleSound(
                midiNote,
                buffer,
                reader->sampleRate));
        }
    }
}

ViolinLegatoSamplerAudioProcessor::~ViolinLegatoSamplerAudioProcessor() {}

//==============================================================================
const juce::String ViolinLegatoSamplerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ViolinLegatoSamplerAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool ViolinLegatoSamplerAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool ViolinLegatoSamplerAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double ViolinLegatoSamplerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

//==============================================================================
int ViolinLegatoSamplerAudioProcessor::getNumPrograms() { return 1; }
int ViolinLegatoSamplerAudioProcessor::getCurrentProgram() { return 0; }
void ViolinLegatoSamplerAudioProcessor::setCurrentProgram(int) {}
const juce::String ViolinLegatoSamplerAudioProcessor::getProgramName(int) { return {}; }
void ViolinLegatoSamplerAudioProcessor::changeProgramName(int, const juce::String&) {}

//==============================================================================
void ViolinLegatoSamplerAudioProcessor::prepareToPlay(double sampleRate, int)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);

    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto* v = dynamic_cast<ViolinSamplerVoice*>(synth.getVoice(i)))
            v->setSampleRateForCrossfade(sampleRate);
    }
}

void ViolinLegatoSamplerAudioProcessor::releaseResources() {}

//==============================================================================
#ifndef JucePlugin_PreferredChannelConfigurations
bool ViolinLegatoSamplerAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

//==============================================================================
void ViolinLegatoSamplerAudioProcessor::processBlock(
    juce::AudioBuffer<float>& buffer,
    juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    keyboardState.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);

    juce::MidiBuffer processedMidi;

    for (const auto metadata : midiMessages)
    {
        auto msg = metadata.getMessage();
        auto samplePos = metadata.samplePosition;

        if (msg.isNoteOn())
        {
            const int note = msg.getNoteNumber();

            bool legato = !heldMidiNotes.empty();

            if (std::find(heldMidiNotes.begin(), heldMidiNotes.end(), note) == heldMidiNotes.end())
                heldMidiNotes.push_back(note);

            isNoteActive = true;

            for (int i = 0; i < synth.getNumVoices(); ++i)
            {
                if (auto* v = dynamic_cast<ViolinSamplerVoice*>(synth.getVoice(i)))
                    v->setLegato(legato);
            }

            if (legato)
            {
                for (int i = 0; i < synth.getNumVoices(); ++i)
                {
                    if (auto* v = dynamic_cast<ViolinSamplerVoice*>(synth.getVoice(i)))
                    {
                        for (int s = 0; s < synth.getNumSounds(); ++s)
                        {
                            auto* sound = synth.getSound(s).get();

                            if (sound->appliesToNote(note))
                            {
                                v->legatoTo(sound, note, msg.getFloatVelocity());
                                break;
                            }
                        }
                    }
                }
            }
            else
            {
                processedMidi.addEvent(msg, samplePos);
            }
        }
        else if (msg.isNoteOff())
        {
            const int note = msg.getNoteNumber();

            heldMidiNotes.erase(
                std::remove(heldMidiNotes.begin(), heldMidiNotes.end(), note),
                heldMidiNotes.end()
            );

            if (heldMidiNotes.empty())
            {
                isNoteActive = false;
                processedMidi.addEvent(msg, samplePos);
            }
            else
            {
                // On garde la note actuelle vivante pendant le legato
            }
        }
        else
        {
            processedMidi.addEvent(msg, samplePos);
        }
    }

    synth.renderNextBlock(buffer, processedMidi, 0, buffer.getNumSamples());
}

//==============================================================================
bool ViolinLegatoSamplerAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* ViolinLegatoSamplerAudioProcessor::createEditor()
{
    return new ViolinLegatoSamplerAudioProcessorEditor(*this);
}

//==============================================================================
void ViolinLegatoSamplerAudioProcessor::getStateInformation(juce::MemoryBlock&) {}
void ViolinLegatoSamplerAudioProcessor::setStateInformation(const void*, int) {}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ViolinLegatoSamplerAudioProcessor();
}
