#ifndef MIDI_DB_H
#define MIDI_DB_H

struct Midi {
  int number;
  const char* category;
  const char* instrument;
};

// Declare the list and function so other files can use them
extern Midi midiList[];
Midi getMidi(int number);

#endif

