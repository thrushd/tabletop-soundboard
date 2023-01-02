#pragma once

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <Audio.h>
#include <Wire.h>
#include <SerialFlash.h>
#include "toml.h"

const int chipSelect = BUILTIN_SDCARD;
// TODO: hangs if you specify track numbers over this limit
const int num_channels = 8;

// object to hold tracks
struct Track {
  char filename[50] {};
  char name[50] {};
  double gain {};
  bool loop {};
  bool play {};
};

// object to hold scenes
struct Scene {
  char name[50] {};
  double gain {};
  bool loop {};
  bool play {};
  struct Track track[num_channels];
};

void fatal(const char *msg, const char *msg1);

int size_of_toml_table(toml_table_t *table);

void print_track(Track input_track);

void print_scene(Scene input_scene);

Track extract_track_table(toml_table_t *input_table, double default_gain, bool default_loop, bool default_play);

Track extract_track_array(const char *input_filename, double default_gain, bool default_loop, bool default_play);

Scene *load_config(char filename[], int* n_scenes);