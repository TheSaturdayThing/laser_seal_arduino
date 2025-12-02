/*
MIDI Harp, with new MP3 Shield
======================

David Iwatsuki November 22, 2025
Revised for Arduino R4 Minima with simplified SparkFun MIDI Music Instrument Shield v13
Which is pre-configured on the board to operate in Real Time MIDI mode

Product: https://www.sparkfun.com/sparkfun-music-instrument-shield.html
GitHub: https://github.com/sparkfun/Music_Instrument_Shield

The Uno R4 has 14 digital pins and 6 analog pins

D0,D1 Serial Monitor RX/TX

D2 (SoftSerial RX) : floating on shield
D3 (SoftSerial TX) : VS1053 RX
D4 : VS1053 RESET
D13 : LED indicator

D5 - D12 availble (8 pins)
A0 - A3 available (4 pins) 

A4, A5 used for I2C peripheral comms

On the R4, the default mode for digital pins is 'input'

======================

Revised and Revamped, Jake Barnwell, July 3-4 2013:
Updated from legacy code (with iRig and iPhone) by Erin King, Ed Moriarty, Jake Barnwell.
See Jake Barnwell for a copy of legacy code for old hardware, and all comments thereof.
  
This is the code for using the new MP3 shield for the Arduino. Said shield precludes the need
for the iRig to iPhone hassle; instead the MP3 shield outputs straight to the speaker.

Useful link is still useful: www.phys.unsw.edu.au/jw/notes.html

Final Code (Only for laser harp versions 1 and 2), Jake Barnwell, July 9 2013:  
Completed edits to make the harp more or less vibration proof. I have instated a minimum
shade time requirement (10 milliseconds)--if the sensor is not shaded for at least 10 ms,
the note will not trigger. This prevents small vibrations from button-mashing the 
proverbial keyboard. Large vibrations (e.g. hammer smashes) will still trigger them.

*/

#include <SoftwareSerial.h>
#include "Knob.h"
#include "LCD.h"
#include "Midi_db.h"

/* Begin MIDI declarations */

SoftwareSerial mySerial(2, 3);  //RX pin defaults to input, TX pin set to output

const int resetMIDI = 4; //Chip reset be driven by Arduino
const int ledPin = 13;   //Drive LED on Arduino board as MIDI traffic inidicator

byte chanMsg_bankSelect = 0x00;
byte chanMsg_noteOff = 0x80;
byte chanMsg_noteOn = 0x90;
byte chanMsg_controlChange = 0xb0;
byte chanMsg_program = 0xc0; //This one is used to switch instruments
byte chanMsg_pitchWheel = 0xe0;

byte bank_default = 0;
byte bank_drums1 = 0x78;
byte bank_melodic = 0x79; //Default bank is usually GM1 (melodic)
byte bank_drums2 = 0x7f;

byte channelVolume = 0x07; //Use as parameter with chanMsg_controlChange
byte devControl_masterVolume = 0x01;

/* End MIDI declarations */
/* Begin music and pin declarations */

//Define Arduino R4's digital input pins for strings:
//Pins are {5,6,7,8,9,10,11,12,13}
const int STRING_PINS[] = {5,6,7,8,9,10,11,12}; // Pins D5-D12
const int NUM_STRINGS = sizeof(STRING_PINS) / sizeof(STRING_PINS[0]); // Calculate number of pins

//Define notes in arrays. The numbers are MIDI-specifiers for the given notes.
//e.g. 33 is A1 (A, 2nd octave)
const int MAJOR_NOTES[NUM_STRINGS] = {23, 24, 26, 28, 29, 31, 33, 35};
const int PENTA_NOTES[NUM_STRINGS] = {24, 26, 28, 31, 33, 36, 38, 40};

int stringVals[NUM_STRINGS]; //Holds the readings from the pins in the current round
boolean previouslyOn[NUM_STRINGS]; //True if the pin was on in the last round; false otherwise
boolean noteIsOn[NUM_STRINGS]; //True if the note is playing for the string
unsigned int signalOnTimes[NUM_STRINGS]; //Holds time at which each string was triggered, if on

/* End music declarations */
/* Begin options: */

const int minShadeTime = 10; //Minimum shade time for a note: 10 milliseconds.

int knob_read = 0;
int instrument = 12; // Piano: marimba. Make this user selectable
int channel = 0;     // First channel
int velocity = 60;
int volume = 120;

/* End options */

void setup() {
  //Begin Serial Monitor for debug: 
  Serial.begin(9600);
  Serial.println("STARTING");
 
  // Init knob on I2C
  initKnob();
  
  // Init LCD display
  initLCD();

  //Begin MIDI Serial for MIDI control: chip requires 31,250
  mySerial.begin(31250);
  
  //Terminate digital inputs if not connected
  pinMode(2, INPUT_PULLUP); //Enable pullup since RX pin from shield to Arduino is not used

  for (int i = 0; i < NUM_STRINGS; i++) {
    pinMode(STRING_PINS[i], INPUT_PULLUP); //Loop to enable pullup on sensor pins

    Serial.print("Pin ");
    Serial.print(STRING_PINS[i]);
    Serial.println(" set to INPUT_PULLUP");
  }
  //Resets the VS1053:
  pinMode(resetMIDI, OUTPUT); // declare Arduino pin as output
  digitalWrite(resetMIDI, LOW);
  delay(100);
  digitalWrite(resetMIDI, HIGH);
  delay(100);
  Serial.println("MIDI reset");
  
  //Set channel volume, max is 127
  talkMIDI(chanMsg_controlChange, channelVolume, volume);
}
/* End setup */

void loop() {
  //Read instrument value from knob and send to MIDI shield
  
  instrument = readKnobPos(); //FIX ME: instrument change during scan causes multiple voices
  Serial.println(instrument);
  talkMIDI(chanMsg_program, instrument, channel); //Set instrument number.
  
  //Display MIDI info on LCD
  displayMidi(instrument);

  //Take sensor pin readings in this round:
  for (int i = 0; i < NUM_STRINGS; i++) {
    stringVals[i] = digitalRead(STRING_PINS[i]);
  }
  
  //Process readings (play note when digital pin goes HIGH):
  for (int i = 0; i < NUM_STRINGS; i++) {
    if (stringVals[i] == HIGH && previouslyOn[i] == true) {
      if (millis() - signalOnTimes[i] >= minShadeTime) {
        if (noteIsOn[i] == false) {
          noteOn(channel, PENTA_NOTES[i], velocity); //Note plays when shaded (?check circuit design?)
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

// Functions are below
//
//Talks MIDI. Doesn't check to see that cmd is greater than 127, or that data values are less than 127
void talkMIDI(byte cmd, byte data1, byte data2) {
  digitalWrite(ledPin, HIGH);
  mySerial.write(cmd);
  mySerial.write(data1);

  //Some commands only have one data byte. All cmds less than 0xBn have 2 data bytes 
  //(sort of: http://253.ccarh.org/handout/midiprotocol/)
  if( (cmd & 0xF0) <= 0xB0)
    mySerial.write(data2);
  digitalWrite(ledPin, LOW);
}

//Send a MIDI note-on message.
void noteOn(byte chan, byte note, byte attack_velocity) {
  talkMIDI( (chanMsg_noteOn | chan), note, attack_velocity);
}

//Send a MIDI note-off message.
void noteOff(byte chan, byte note, byte release_velocity) {
  talkMIDI( (chanMsg_noteOff | chan), note, release_velocity);
}