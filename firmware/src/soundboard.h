#pragma once

#include "gif_player.h"
#include "module.h"
#include "toml.h"
#include "track.h"
#include "helper_functions.h"
#include <Adafruit_GFX.h> // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <Arduino.h>
#include <Audio.h>
#include <Encoder.h>
#include <OneButton.h>
#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>

#define MAIN_SCREEN_WIDTH 240 // display width, in pixels
#define MAIN_SCREEN_HEIGHT 240 // display height, in pixels

#define TFT_CS 30
#define TFT_RST 32 // Or set to -1 and connect to RESET pin
#define TFT_DC 31

#define ENCODER_BUTTON_PIN 10
#define ENCODER_A_PIN 35
#define ENCODER_B_PIN 36

#define N_MODULES 8

#define CHIP_SELECT BUILTIN_SDCARD

class SoundBoard {
public:
    SoundBoard(char* config_file_name);
    void update();

private:
    char config_file_name[MAX_CHAR];
    Adafruit_ST7789* main_display;
    OneButton button;
    Encoder* knob;
    int window_pos = 0; // position of the window in the array, defined as the minimum
    int cursor_pos = 0; // position of the cursor in the displayed scenes
    int active_scene = 0; // the current active scene

    char** scene_names;
    int n_scenes;
    Track** module_tracks;
    char startup_gif_filename[MAX_CHAR];

    // std::vector<String> scene_names;

    // GifPlayer* gif_player;

    void init();

    // onebutton update functions
    static void handle_single(void* ptr);
    static void handle_double(void* ptr);
    static void handle_hold(void* ptr);
    void process_single();
    void process_double();
    void process_hold();

    int size_of_toml_table(toml_table_t* table);
    void print_track(Track input_track);
    Track table_to_track(toml_table_t* input_table, double default_gain, bool default_loop, bool default_play);
    void load_config();
};

/*
void display_setup();

void display_text_test();

void display_encoder_test();
*/