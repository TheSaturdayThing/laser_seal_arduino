/*
MIDI Harp, with new MP3 Shield

Revised and Revamped, Jake Barnwell, July 3-4 2013:
Updated from legacy code (with iRig and iPhone) by Erin King, Ed Moriarty, Jake Barnwell.
See Jake Barnwell for a copy of legacy code for old hardware, and all comments thereof.
  
This is the code for using the new MP3 shield for the Arduino. Said shield precludes the need
for the iRig to iPhone hassle; instead the MP3 shield outputs straight to the speaker.

Useful link is still useful: www.phys.unsw.edu.au/jw/notes.html

Final Code (Onlly for laser harp versions 1 and 2), Jake Barnwell, July 9 2013:  
Completed edits to make the harp more or less vibration proof. I have instated a minimum
shade time requirement (10 milliseconds)--if the sensor is not shaded for at least 10 ms,
the note will not trigger. This prevents small vibrations from button-mashing the 
proverbial keyboard. Large vibrations (e.g. hammer smashes) will still trigger them.


*/

#include <SoftwareSerial.h>

/* Begin MIDI declarations */

SoftwareSerial mySerial(2, 3);  //RX and TX pins

const int gpioPin = 4, resetMIDI = 8;

byte chanMsg_noteOff = 0x80, chanMsg_noteOn = 0x90, chanMsg_pitchWheel = 0xe0, chanMsg_controlChange = 0xb0, chanMsg_bankSelect = 0x00;
byte chanMsg_program = 0xc0; //This one is used to switch instruments
byte bank_default = 0, bank_drums1 = 0x78, bank_drums2 = 0x7f, bank_melodic = 0x79; //Default bank is usually GM1 (melodic)
byte channelVolume = 0x07; //Use as parameter with chanMsg_controlChange
byte devControl_masterVolume = 0x01;

/* End MIDI declarations */
/* Begin music and pin declarations */

const int NUM_STRINGS = 17;

//Define Arduino's digital input pins for strings:
//Pins are {22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38}
const int STRING_PINS[NUM_STRINGS] = {23,22,25,24,27,26,29,28,31,30,33,32,35,34,37,36,38}; //Note groups of two are flipped

//Define notes in arrays. The numbers are MIDI-specifiers for the given notes.
//e.g. 33 is A1 (A, 2nd octave)
const int MAJOR_NOTES[NUM_STRINGS] = {23, 24, 26, 28, 29, 31, 33, 35, 36, 38, 40, 41, 43, 45, 47, 48, 50};
const int PENTA_NOTES[NUM_STRINGS] = {24, 26, 28, 31, 33, 36, 38, 40, 43, 45, 48, 50, 52, 55, 57, 60, 62};

int stringVals[NUM_STRINGS]; //Holds the readings from the 17 pins in the current round
boolean previouslyOn[NUM_STRINGS]; //True if the pin was on in the last round; false otherwise
boolean noteIsOn[NUM_STRINGS]; //True if the note is playing for the string
unsigned int signalOnTimes[NUM_STRINGS]; //Holds time at which each string was triggered, if on

/* End music declarations */
/* Begin options: */

const int minShadeTime = 10; //Minimum shade time for a note: 10 milliseconds.
int instrument = 12, channel = 0, velocity = 60;

/* End options */

void setup() {
  //Begin Serial:
  Serial.begin(9600);
  
  //Begin MIDI serial for MIDI control:
  mySerial.begin(31250);

  //Sets chip mode to real-time MIDI:
  pinMode(gpioPin, OUTPUT);
  digitalWrite(gpioPin, HIGH);
  
  //Resets the VS1053:
  pinMode(resetMIDI, OUTPUT);
  digitalWrite(resetMIDI, LOW);
  delay(100);
  digitalWrite(resetMIDI, HIGH);
  delay(100);
  
  //Set channel volume, max is 127
  talkMIDI(chanMsg_controlChange, channelVolume, 120);

  for (int i = 0; i < NUM_STRINGS; i++) {
    pinMode(STRING_PINS[i], INPUT);
  }
  
  talkMIDI(chanMsg_program, instrument, channel); //Set instrument number.
}

void loop() {
  
  //Take readings in this round:
  for (int i = 0; i < NUM_STRINGS; i++) {
    stringVals[i] = digitalRead(STRING_PINS[i]);
  }
  
  //Process readings:
  for (int i = 0; i < NUM_STRINGS; i++) {
    if (stringVals[i] == HIGH && previouslyOn[i] == true) {
      if (millis() - signalOnTimes[i] >= minShadeTime) {
        if (noteIsOn[i] == false) {
          noteOn(channel, PENTA_NOTES[i], velocity);
          noteIsOn[i] = true;
        }
      }
    } else if (stringVals[i] == HIGH && previouslyOn[i] == false) {
      signalOnTimes[i] = millis();
    } else if (stringVals[i] == LOW && previouslyOn[i] == true) {
      noteOff(channel, PENTA_NOTES[i], velocity);
      noteIsOn[i] = false;
      signalOnTimes[i] = 0;
    } else {
      ;
    }
    
    //When all is done, assign current values to past values:
    previouslyOn[i] = stringVals[i]; //Booleans typecast, T = 1, F = 0
  }
  
}


//Send a MIDI note-on message.
void noteOn(byte chan, byte note, byte attack_velocity) {
  talkMIDI( (chanMsg_noteOn | chan), note, attack_velocity);
}

//Send a MIDI note-off message.
void noteOff(byte chan, byte note, byte release_velocity) {
  talkMIDI( (chanMsg_noteOff | chan), note, release_velocity);
}

//Talks MIDI. Doesn't check to see that cmd is greater than 127, or that data values are less than 127
void talkMIDI(byte cmd, byte data1, byte data2) {
  mySerial.write(cmd);
  mySerial.write(data1);

  //Some commands only have one data byte. All cmds less than 0xBn have 2 data bytes 
  //(sort of: http://253.ccarh.org/handout/midiprotocol/)
  if( (cmd & 0xF0) <= 0xB0)
    mySerial.write(data2);
}

