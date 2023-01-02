#pragma once

#include <Arduino.h>
#include <Bounce2.h>
#include <OneButton.h>
#include <Audio.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// #include "read_config.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
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
        Module(int module_number, int button_pin, Track *tracks, AudioPlaySdWav *play_sd, AudioMixer4 *mixer, TwoWire *twi = &Wire);
        void update(int new_scene_index);
        
    private:
        // pointer to array of track objects
        Track *tracks;
        // current scene index
        int scene_index = -1;
        // module number
        int module_number;
        // audio
        AudioPlaySdWav *play_sd;
        AudioMixer4 *mixer;
        int mixer_channel_number;
        bool player = false;
        // display
        Adafruit_SSD1306 display;
        TwoWire *twi;
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
        static void handle_single(void *ptr);
        static void handle_double(void *ptr);
        static void handle_hold(void *ptr);

        void process_single();
        void process_double();
        void process_hold();
};