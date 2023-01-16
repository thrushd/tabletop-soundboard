#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <Audio.h>
#include <Encoder.h>
#include <OneButton.h>
#include <Wire.h>
#include "track.h"

// #include "read_config.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C
#define GAIN_MIN 0
#define GAIN_MAX 1

#define TCA_ADDR 0x70 // address of i2c multiplexer chip


class Module {
public:
    Module(int module_number, int button_pin, Track* tracks_in, AudioPlaySdWav* play_sd, AudioMixer4* mixer_left, AudioMixer4* mixer_right, TwoWire* twi, Encoder* knob);
    void update(int new_scene_index);

private:
    // pointer to array of track objects
    Track* tracks;
    // current scene index
    int scene_index = -1;
    // module number
    int module_number;
    // audio
    AudioPlaySdWav* play_sd;
    AudioMixer4* mixer_left;
    AudioMixer4* mixer_right;
    int mixer_channel_number;
    bool play_flag = false;
    uint32_t track_duration;
    // display
    Adafruit_SSD1306 display;
    TwoWire* twi;
    // button
    int button_pin;
    // module button;
    OneButton button;
    // encoder knob for main display
    Encoder* knob;
    long old_position = 0;
    long new_position = 0;
    double gain_diff = 0;
    int enc_int_min = 0;
    int enc_int_max = 80; // 4 counts per detent showing on 0.05 scale 4*20

    // functions
    void init();
    // LCD stuff
    void set_mp_addr();
    void update_display();
    // audio stuff
    static void handle_single(void* ptr);
    static void handle_double(void* ptr);
    static void handle_hold(void* ptr);

    void process_single();
    void process_double();
    void process_hold();

    String millis_to_hhmmss(uint32_t time_millis);

    void rect_progress_bar(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t progress, uint32_t duration);
    void volume_display(uint16_t x, uint16_t y, uint16_t w, uint16_t h, double gain, double max);
    float map_float(float x, float in_min, float in_max, float out_min, float out_max);
};