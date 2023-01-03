#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <Audio.h>
#include <Bounce2.h>
#include <OneButton.h>
#include <Wire.h>

// #include "read_config.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C

#define TCA_ADDR 0x70 // address of i2c multiplexer chip

// object to hold tracks
struct Track {
    char filename[50] {};
    char name[50] {};
    double gain {};
    bool loop {};
    bool play {};
};

class Module {
public:
    Module(int module_number, int button_pin, Track* tracks, AudioPlaySdWav* play_sd, AudioMixer4* mixer, TwoWire* twi = &Wire);
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
    AudioMixer4* mixer;
    int mixer_channel_number;
    bool player = false;
    uint32_t track_duration;
    // display
    Adafruit_SSD1306 display;
    TwoWire* twi;
    // button
    int button_pin;
    // Bounce button;
    OneButton button;

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
};