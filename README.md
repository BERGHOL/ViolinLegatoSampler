# 🎻 Violin Legato Sampler

A sample-based violin instrument built with JUCE, featuring a custom real-time algorithmic legato system with smooth transitions between notes.

---

## 🔍 Overview

Violin Legato Sampler is an experimental audio plugin focused on reproducing natural legato behavior using a minimal set of samples.

Instead of relying on pre-recorded legato articulations, the plugin generates transitions in real time using:
- note detection
- time-based logic
- crossfading techniques

---

## ✨ Features

- 🎼 Sample-based playback (one WAV per note)
- 🎻 Monophonic voice (instrument-like behavior)
- 🔁 Real-time legato transitions
- 🎚 Crossfade between notes
- ⚡ Attack & release smoothing (anti-click)
- 🎹 MIDI support (virtual keyboard + external devices)
- 🧩 VST3 plugin + standalone application

---

## 🧠 Legato Engine

The legato system combines note state tracking and timing logic.

### Detection
- Overlapping notes (true legato)
- Fast note transitions (~50 ms window)

### Processing
When legato is triggered, the engine applies:
- Sample offset (skips initial transient)
- Crossfade between previous and new note
- Micro attack to ensure smooth amplitude transition

This approach allows fluid transitions without dedicated legato samples.

---

## 📁 Project Structure

Source/ ├── PluginProcessor.cpp ├── PluginEditor.cpp ├── ViolinSamplerVoice.h └── ViolinSampleSound.h

---

## 🎧 Sample Requirements

- Format: WAV
- Channels: Mono
- Sample Rate: 44.1 kHz or 48 kHz
- Content: clean sustain (minimal attack noise)

---

## 🛠️ Build

1. Open the .jucer file in Projucer
2. Export to Visual Studio 2022
3. Build using Debug x64

### Outputs
- VST3 plugin
- Standalone application

---

## ⚠️ Limitations

- Monophonic playback only
- Single velocity layer
- No articulation switching

---

## 🚀 Roadmap

- Improved legato realism
- Portamento (glide)
- Multi-velocity support
- UI improvements

---

## 📌 Notes

This project is intended as a technical exploration of real-time legato synthesis, rather than a fully featured commercial instrument.
