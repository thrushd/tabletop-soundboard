#pragma once

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <Audio.h>
#include <Wire.h>
#include <SerialFlash.h>
#include "toml.h"
#include "track.h"


void print_track(Track input_track);

void load_config(String config_filename, String * scene_names, Track ** module_tracks, String &gif_name);