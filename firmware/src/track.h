#pragma once

#include <Arduino.h>

#define MAX_CHAR 41

// object to hold tracks
struct Track {
    char filename[MAX_CHAR];
    char name[MAX_CHAR];
    double gain;
    bool loop;
    bool play;
};