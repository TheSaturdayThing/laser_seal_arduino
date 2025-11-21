The Laser Seal uses an Arduino to control a [SparkFun MP3 Player Shield](https://www.sparkfun.com/sparkfun-mp3-player-shield.html).  
This board utilizes the VS1053 MP3 audio decoder IC to decode audio files.

The current implementation does *NOT* use the VS1053 to play MP3 files, but instead it is set to real-time MIDI by activating the "Real-Time MIDI mode":
- GPIO0 is LOW (grounded through 100K resistor on shield)
- GPIO1 is HIGH (driven by pin D4 of shield)
  
Once activated, the VS1053 can receive MIDI data through the UART (at a 31250 bps MIDI baud rate) or via the SPI bus, allowing it to function as a synthesizer without needing to load a MIDI file. 

Real-time MIDI data is transmitted as a series of messages, each starting with a status byte indicating the message type, followed by one or two data bytes containing specific information like note, velocity, or channel. Key real-time messages include System Real-Time messages for synchronization (e.g., Start, Stop, Timing Clock), which have status bytes but no data bytes. Other common real-time data includes standard messages like Note On/Off, which include a status byte for the message and channel, and data bytes for note number and velocity. 

The current laser seal uses a MIDI [instrument](https://fmslogo.sourceforge.io/manual/midi-instrument.html) code of '**12: Chromatic Percussion/ Marimba**' and follows a 17-note pentatonic scale:  
PENTA_NOTES[NUM_STRINGS] = {24, 26, 28, 31, 33, 36, 38, 40, 43, 45, 48, 50, 52, 55, 57, 60, 62};  
(see [here](https://inspiredacoustics.com/en/MIDI_note_numbers_and_center_frequencies))

| Sensor | MIDI | Piano Key |Note | Frequency (HZ) |
|--------|------|-----------|-----|----------------|
| 1      | 24   | 4         | C1  | 32.70          |
| 2      | 26   | 6         | D1  | 36.71          |
| 3      | 28   | 8         | E1  | 41.20          |
| 4      | 31   | 11        | G1  | 49.00          |
| 5      | 33   | 13        | A1  | 55.00          |
| 6      | 36   | 16        | C2  | 65.41          |
| 7      | 38   | 18        | D2  | 73.42          |
| 8      | 40   | 20        | E2  | 82.41          |
| 9      | 43   | 23        | G2  | 98.00          |
| 10     | 45   | 25        | A2  | 110.00         |
| 11     | 48   | 28        | C3  | 130.81         |
| 12     | 50   | 30        | D3  | 146.83         |
| 13     | 52   | 32        | E3  | 164.81         |
| 14     | 55   | 35        | G3  | 196.00         |
| 15     | 57   | 37        | A3  | 220.00         |
| 16     | 60   | 40        | C4  | 261.63         |
| 17     | 62   | 42        | D4  | 293.66         |



An explanation of MIDI messaging is [here](https://learn.sparkfun.com/tutorials/midi-tutorial/all)

Support for the current v1.03.00 libraries is on [github.io](https://mpflaga.github.io/Arduino_Library-vs1053_for_SdFat/index.html) and explains how the board is set up.

Driver source code can be found [here](https://github.com/mpflaga/Arduino_Library-vs1053_for_SdFat)

This was migrated from the now closed v1.02.15 page on [github.com](https://mpflaga.github.io/Sparkfun-MP3-Player-Shield-Arduino-Library) along with the older [source code](https://github.com/mpflaga/Sparkfun-MP3-Player-Shield-Arduino-Library)

Documentation on the VS1053 chip can be found on the manufacturer's [website](https://www.vlsi.fi/en/products/vs1053.html)

TODO:
- Does the current Mega take into account that the Arduino Uno uses digital pins 11 (MOSI), 12 (MISO), and 13 (SCK) for hardware SPI, while the Arduino Mega uses 51 (MOSI), 50 (MISO), and 52 (SCK). 
- Both boards have a separate pin for slave select (SS), which is pin 10 on the Uno and pin 53 on the Mega.
