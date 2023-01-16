#pragma once

#include <Arduino.h>

// object to hold tracks
struct Track {
    String filename {};
    String name {};
    double gain {};
    bool loop {};
    bool play {};
};