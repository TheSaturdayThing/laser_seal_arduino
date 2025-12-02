#include "Knob.h"

// Create a ModulinoKnob object
static ModulinoKnob knob;

void initKnob() {
    Serial.begin(9600);
    Modulino.begin();
    Wire.begin();

    delay(1000);
    byte error, address;
    int nDevices = 0;

    Serial.println("Scanning for I2C in 1 second");
    delay(1000);

    for (address = 1; address < 127; address++) {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0) {
            Serial.print("I2C device found at address 0x");
            if (address < 16) Serial.print("0");
            Serial.print(address, HEX);
            Serial.println("  !");
            nDevices++;
        } else if (error == 4) {
            Serial.print("Unknown error at address 0x");
            if (address < 16) Serial.print("0");
            Serial.println(address, HEX);
        }
    }

    if (nDevices == 0)
        Serial.println("No I2C devices found\n");
    else
        Serial.println("done with setup, ready in 1 second\n");

    delay(1000);

    // Detect and connect to knob module
    knob.begin();
}

Pos readKnobPos() {
    int16_t read = knob.get();

    //Limit returned values to 0..127
    if (read > 127) {
        knob.set(0);   // reset to 0 if above range
        return 0;
    } else if (read < 0) {
        knob.set(127); // reset to 127 if below range
        return 127;
    }

    return read; // valid range, return as-is
}

bool knobClicked() {
    return knob.isPressed();
}

int8_t knobDirection() {
    return knob.getDirection();
}
