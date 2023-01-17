#pragma once

#include <Adafruit_GFX.h> // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <Arduino.h>
#include <Audio.h>
#include <Encoder.h>
#include <OneButton.h>
#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>
#include "gif_player.h"
#include "module.h"

#define MAIN_SCREEN_WIDTH 240 // display width, in pixels
#define MAIN_SCREEN_HEIGHT 240 // display height, in pixels

#define TFT_CS 30
#define TFT_RST 32 // Or set to -1 and connect to RESET pin
#define TFT_DC 31

#define ENCODER_BUTTON_PIN 10
#define ENCODER_A_PIN 35
#define ENCODER_B_PIN 36

#define CHIP_SELECT BUILTIN_SDCARD

// GUItool: begin automatically generated code
// AudioPlaySdWav           play_sd_wav_1;     //xy=765.0000152587891,499.0000190734863
// AudioPlaySdWav           play_sd_wav_2;     //xy=765.0000152587891,539.0000228881836
// AudioPlaySdWav           play_sd_wav_3;     //xy=765.0000152587891,580.0000228881836
// AudioPlaySdWav           play_sd_wav_4;     //xy=765.0000190734863,624.0000190734863
// AudioPlaySdWav           play_sd_wav_5;     //xy=765.0000190734863,667.0000200271606
// AudioPlaySdWav           play_sd_wav_0;     //xy=766.0000152587891,455.000018119812
// AudioPlaySdWav           play_sd_wav_6;     //xy=765.0000190734863,708.000020980835
// AudioPlaySdWav           play_sd_wav_7;     //xy=766.0000190734863,748.0000228881836
// AudioMixer4              right_mixer_0;         //xy=1033.0001907348633,660.0000190734863
// AudioMixer4              left_mixer_1;         //xy=1034.0001907348633,539.0000553131104
// AudioMixer4              right_mixer_1;         //xy=1034.0001907348633,732.0000228881836
// AudioMixer4              left_mixer_0;         //xy=1036.0000305175781,469.000093460083
// AudioMixer4              right_final_mixer;         //xy=1293.0000381469727,646.0000190734863
// AudioMixer4              left_final_mixer;         //xy=1295.0000381469727,563.0000953674316
// AudioOutputPT8211        pt8211_1;       //xy=1524,598
// AudioConnection          patchCord1(play_sd_wav_1, 0, left_mixer_0, 1);
// AudioConnection          patchCord2(play_sd_wav_1, 1, right_mixer_0, 1);
// AudioConnection          patchCord3(play_sd_wav_2, 0, left_mixer_0, 2);
// AudioConnection          patchCord4(play_sd_wav_2, 1, right_mixer_0, 2);
// AudioConnection          patchCord5(play_sd_wav_3, 0, left_mixer_0, 3);
// AudioConnection          patchCord6(play_sd_wav_3, 1, right_mixer_0, 3);
// AudioConnection          patchCord7(play_sd_wav_4, 0, left_mixer_1, 0);
// AudioConnection          patchCord8(play_sd_wav_4, 1, right_mixer_1, 0);
// AudioConnection          patchCord9(play_sd_wav_5, 0, left_mixer_1, 1);
// AudioConnection          patchCord10(play_sd_wav_5, 1, right_mixer_1, 1);
// AudioConnection          patchCord11(play_sd_wav_0, 0, left_mixer_0, 0);
// AudioConnection          patchCord12(play_sd_wav_0, 1, right_mixer_0, 0);
// AudioConnection          patchCord13(play_sd_wav_6, 0, left_mixer_1, 2);
// AudioConnection          patchCord14(play_sd_wav_6, 1, right_mixer_1, 2);
// AudioConnection          patchCord15(play_sd_wav_7, 0, left_mixer_1, 3);
// AudioConnection          patchCord16(play_sd_wav_7, 1, right_mixer_1, 3);
// AudioConnection          patchCord17(right_mixer_0, 0, right_final_mixer, 0);
// AudioConnection          patchCord18(left_mixer_1, 0, left_final_mixer, 1);
// AudioConnection          patchCord19(right_mixer_1, 0, right_final_mixer, 1);
// AudioConnection          patchCord20(left_mixer_0, 0, left_final_mixer, 0);
// AudioConnection          patchCord21(right_final_mixer, 0, pt8211_1, 1);
// AudioConnection          patchCord22(left_final_mixer, 0, pt8211_1, 0);
// GUItool: end automatically generated code

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

    String* scene_names;
    Track** module_tracks;

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