#pragma once
#include <Arduino.h>

void fatal(const char *msg, const char *msg1);
float map_float(float x, float in_min, float in_max, float out_min, float out_max);