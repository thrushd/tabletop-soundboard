#include "helper_functions.h"

// print out an error message and halt execution
void fatal(const char* msg, const char* msg1)
{
    Serial.println();
    Serial.print("ERROR: ");
    Serial.print(msg);
    Serial.println(msg1);
    while (1)
        ;
}

// map a value to a float between bounds
float map_float(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}