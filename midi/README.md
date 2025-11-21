The Laser Seal uses an Arduino to control a [SparkFun MP3 Player Shield](https://www.sparkfun.com/sparkfun-mp3-player-shield.html).  
This board utilizes the VS1053 MP3 audio decoder IC to decode audio files.

The current implementation does *NOT* use the VS1053 to play MP3 files, but instead it is set to real-time MIDI by activating the "Real-Time MIDI mode":
- GPIO0 is LOW (grounded through 100K resistor on shield)
- GPIO1 is HIGH (driven by pin D4 of shield)
  
Once activated, the VS1053 can receive MIDI data through the UART (at a 31250 bps MIDI baud rate) or via the SPI bus, allowing it to function as a synthesizer without needing to load a MIDI file.

Support for the current v1.03.00 libraries is on [github.io](https://mpflaga.github.io/Arduino_Library-vs1053_for_SdFat/index.html) and explains how the board is set up.

Driver source code can be found [here](https://github.com/mpflaga/Arduino_Library-vs1053_for_SdFat)

This was migrated from the now closed v1.02.15 page on [github.com](https://mpflaga.github.io/Sparkfun-MP3-Player-Shield-Arduino-Library) along with the older [source code](https://github.com/mpflaga/Sparkfun-MP3-Player-Shield-Arduino-Library)

Documentation on the VS1053 chip can be found on the manufacturer's [website](https://www.vlsi.fi/en/products/vs1053.html)

TODO:
- Does the current Mega take into account that the Arduino Uno uses digital pins 11 (MOSI), 12 (MISO), and 13 (SCK) for hardware SPI, while the Arduino Mega uses 51 (MOSI), 50 (MISO), and 52 (SCK). 
- Both boards have a separate pin for slave select (SS), which is pin 10 on the Uno and pin 53 on the Mega.
