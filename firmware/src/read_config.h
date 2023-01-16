#pragma once

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <Audio.h>
#include <Wire.h>
#include <SerialFlash.h>
#include "toml.h"
#include "track.h"

const int chipSelect = BUILTIN_SDCARD;
// TODO: hangs if you specify track numbers over this limit
const int n_modules = 8;

int size_of_toml_table(toml_table_t *table);

void print_track(Track input_track);

// void print_scene(Scene input_scene);

Track extract_track_table(toml_table_t *input_table, double default_gain, bool default_loop, bool default_play);

Track extract_track_array(String input_filename, double default_gain, bool default_loop, bool default_play);

void load_config(String config_filename, String* scene_names, Track* module_names);