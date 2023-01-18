#pragma once

#include "gif_player.h"
#include "module.h"
#include "read_config.h"
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

#define CHIP_SELECT BUILTIN_SDCARD

class SoundBoard {
public:
    SoundBoard(String config_file_name);
    void update();

private:
    String config_file_name;
    Adafruit_ST7789* main_display;
    OneButton button;
    Encoder* knob;
    int window_pos = 0; // position of the window in the array, defined as the minimum
    int cursor_pos = 0; // position of the cursor in the displayed scenes
    int active_scene = 0; // the current active scene

    String * scene_names;
    Track** module_tracks;
    String startup_gif_filename;

    // GifPlayer* gif_player;

    void init();

    // onebutton update functions
    static void handle_single(void* ptr);
    static void handle_double(void* ptr);
    static void handle_hold(void* ptr);
    void process_single();
    void process_double();
    void process_hold();

    void display_startup_splash();
};

/*
void display_setup();

void display_text_test();

void display_encoder_test();
*/