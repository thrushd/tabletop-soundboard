#include "hardware.h"

/*
hardware globals
*/

// main encoder knob
const int knob_scale = 4;
int old_position = -999;
Encoder knob(5, 6);
Bounce knob_button = Bounce(ENC_BUTTON_PIN, 5);