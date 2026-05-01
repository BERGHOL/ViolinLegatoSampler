🎻 Violin Legato Sampler (JUCE VST)

A lightweight violin sampler plugin built with JUCE, featuring a custom algorithmic legato system with crossfade transitions.

🚀 Features
🎼 Sample-based instrument (WAV per note)
🎹 MIDI keyboard support (virtual + external)
🎻 Monophonic legato system
🔁 Real-time crossfade between notes
🎚 Dynamic attack & release smoothing
⚡ Built as VST3 + Standalone app
🧠 Legato System

This plugin implements a custom algorithmic legato engine:

Detects overlapping notes or fast re-triggering
Uses a time window (~50ms) to simulate natural legato playing
Applies:
sample offset (skip attack)
crossfade between previous and new note
micro attack to avoid clicks
📁 Project Structure
Source/
├── PluginProcessor.cpp
├── PluginEditor.cpp
├── ViolinSamplerVoice.h
├── ViolinSampleSound.h
🎧 Samples
One sample per note (WAV format)
Recommended:
mono
44.1kHz or 48kHz
clean
