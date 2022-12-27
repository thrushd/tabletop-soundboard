#pragma once

#include <Arduino.h>
#include <Encoder.h>
#include <Bounce.h>

// main encoder definitions
extern const int knob_scale;
extern int old_position;
#define ENC_BUTTON_PIN 33
extern Encoder knob;
extern Bounce knob_button;