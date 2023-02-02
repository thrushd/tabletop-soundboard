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

void load_config(char* config_filename, char** scene_names, int* n_scenes_ptr, Track** module_tracks, char* gif_name);