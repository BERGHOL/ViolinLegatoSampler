#pragma once

#include <vector>
#include <algorithm>

class LegatoManager
{
public:
    void noteOn(int note)
    {
        heldNotes.push_back(note);
        currentNote = note;
    }

    void noteOff(int note)
    {
        heldNotes.erase(
            std::remove(heldNotes.begin(), heldNotes.end(), note),
            heldNotes.end()
        );

        currentNote = heldNotes.empty() ? -1 : heldNotes.back();
    }

    bool isLegato() const
    {
        return heldNotes.size() > 1;
    }

    bool isAnyNoteHeld() const
    {
        return !heldNotes.empty();
    }

    int getCurrentNote() const
    {
        return currentNote;
    }

private:
    int currentNote = -1;
    std::vector<int> heldNotes;
};