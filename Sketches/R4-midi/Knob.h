/*
 * Modulino Knob - Basic
 *
 * This example code is in the public domain.
 * Copyright (c) 2025 Arduino
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef KNOB_H
#define KNOB_H

#include <Arduino_Modulino.h>

// Define a type alias for knob position
typedef int16_t Pos;

// Function declarations
void initKnob();     // initialize Modulino + knob
Pos readKnobPos();   // get knob position
bool knobClicked();  // check if knob was pressed
int8_t knobDirection(); // get rotation direction

#endif // KNOB_H
