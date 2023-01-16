#include <Arduino.h>
#include "soundboard.h"

// testing configs
// char config_file_name[] = "config_no_defaults.toml";
// char config_file_name[] = "config_no_fixed.toml";
// char config_file_name[] = "config_no_scenes.toml";
// char config_file_name[] = "config_no_track_data.toml";
// char config_file_name[] = "config_no_tracks.toml";

String config_file_name = "config.toml";


void setup()
{
    Serial.begin(9600);
    Wire.begin();
    AudioMemory(15);
    // wait for serial to open
    while (!Serial) {}

    Serial.println();
    Serial.println("starting this bullshit program now");

    SoundBoard sb = SoundBoard(config_file_name);

    // Track tracks[] = {{"SDTEST2.WAV", "Spooky", 0.5, false, false},
    //                   {"SDTEST3.WAV", "Bang Time", 1.0, false, false},
    //                   {"SDTEST4.WAV", "Ouch", 0.5, false, false},
    //                   {"SDTEST4.WAV", "Chilling Outside", 1.2, false, false}};
    // Module mod_1(0, 33, tracks, &play_sd_wav_0, &left_mixer_0, &right_mixer_0, &Wire, &knobbo);
    // Module mod_2(1, 34, tracks, &play_sd_wav_1, &left_mixer_0, &right_mixer_0, &Wire, &knobbo);
    // Module mod_3(2, 35, tracks, &play_sd_wav_3, 0, &Wire);
    // Module mod_4(3, 36, tracks, &play_sd_wav_4, 0, &Wire);
    while(1){
        sb.update();
    }
}

void loop()
{
    
}
