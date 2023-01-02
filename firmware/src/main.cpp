// #include "read_config.h"
// #include "display.h"
// #include "helper_functions.h"
#include "module.h"

#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

char config_file_name[] = "config.toml";
// testing configs
// char config_file_name[] = "config_no_defaults.toml";
// char config_file_name[] = "config_no_fixed.toml";
// char config_file_name[] = "config_no_scenes.toml";
// char config_file_name[] = "config_no_track_data.toml";
// char config_file_name[] = "config_no_tracks.toml";


// GUItool: begin automatically generated code
AudioPlaySdWav           play_sd_wav_1;     //xy=765.0000152587891,499.0000190734863
AudioPlaySdWav           play_sd_wav_2;     //xy=765.0000152587891,539.0000228881836
AudioPlaySdWav           play_sd_wav_3;     //xy=765.0000152587891,580.0000228881836
AudioPlaySdWav           play_sd_wav_4;     //xy=765.0000190734863,624.0000190734863
AudioPlaySdWav           play_sd_wav_5;     //xy=765.0000190734863,667.0000200271606
AudioPlaySdWav           play_sd_wav_0;     //xy=766.0000152587891,455.000018119812
AudioPlaySdWav           play_sd_wav_6;     //xy=765.0000190734863,708.000020980835
AudioPlaySdWav           play_sd_wav_7;     //xy=766.0000190734863,748.0000228881836
AudioMixer4              right_mixer_0;         //xy=1033.0001907348633,660.0000190734863
AudioMixer4              left_mixer_1;         //xy=1034.0001907348633,539.0000553131104
AudioMixer4              right_mixer_1;         //xy=1034.0001907348633,732.0000228881836
AudioMixer4              left_mixer_0;         //xy=1036.0000305175781,469.000093460083
AudioMixer4              right_final_mixer;         //xy=1293.0000381469727,646.0000190734863
AudioMixer4              left_final_mixer;         //xy=1295.0000381469727,563.0000953674316
AudioOutputPT8211        pt8211_1;       //xy=1524,598
AudioConnection          patchCord1(play_sd_wav_1, 0, left_mixer_0, 1);
AudioConnection          patchCord2(play_sd_wav_1, 1, right_mixer_0, 1);
AudioConnection          patchCord3(play_sd_wav_2, 0, left_mixer_0, 2);
AudioConnection          patchCord4(play_sd_wav_2, 1, right_mixer_0, 2);
AudioConnection          patchCord5(play_sd_wav_3, 0, left_mixer_0, 3);
AudioConnection          patchCord6(play_sd_wav_3, 1, right_mixer_0, 3);
AudioConnection          patchCord7(play_sd_wav_4, 0, left_mixer_1, 0);
AudioConnection          patchCord8(play_sd_wav_4, 1, right_mixer_1, 0);
AudioConnection          patchCord9(play_sd_wav_5, 0, left_mixer_1, 1);
AudioConnection          patchCord10(play_sd_wav_5, 1, right_mixer_1, 1);
AudioConnection          patchCord11(play_sd_wav_0, 0, left_mixer_0, 0);
AudioConnection          patchCord12(play_sd_wav_0, 1, right_mixer_0, 0);
AudioConnection          patchCord13(play_sd_wav_6, 0, left_mixer_1, 2);
AudioConnection          patchCord14(play_sd_wav_6, 1, right_mixer_1, 2);
AudioConnection          patchCord15(play_sd_wav_7, 0, left_mixer_1, 3);
AudioConnection          patchCord16(play_sd_wav_7, 1, right_mixer_1, 3);
AudioConnection          patchCord17(right_mixer_0, 0, right_final_mixer, 0);
AudioConnection          patchCord18(left_mixer_1, 0, left_final_mixer, 1);
AudioConnection          patchCord19(right_mixer_1, 0, right_final_mixer, 1);
AudioConnection          patchCord20(left_mixer_0, 0, left_final_mixer, 0);
AudioConnection          patchCord21(right_final_mixer, 0, pt8211_1, 1);
AudioConnection          patchCord22(left_final_mixer, 0, pt8211_1, 0);
// GUItool: end automatically generated code

#define CHIP_SELECT    BUILTIN_SDCARD


void setup() {

    // display testing
    // pinMode(ENC_BUTTON_PIN, INPUT_PULLUP);

    Serial.begin(9600);
    Wire.begin();
    AudioMemory(15);
    // wait for serial to open
    while (!Serial) {}

    // set up SD card
    Serial.println();
    Serial.print("initializing SD card... ");
  
    if (!SD.begin(CHIP_SELECT)) {
      Serial.println();
      Serial.println("SD initialization failed");
      while(1);
    }
    Serial.println("done");
  
    // get array of scenes from the config
    // Scene *scenes; // array of scenes
    // int n_scenes = 0; // length of array of scenes
    // scenes = load_config(config_file_name, &n_scenes);
    // print them
    // for (int i = 0; i < n_scenes; i++) {
    //   print_scene(scenes[i]);
    // }
    // Serial.println("scenes loaded");

    // display_setup();
    // display_startup_splash();
    // display_text_test();
    // display_encoder_test();

    Serial.println("starting");

    Track tracks[] = {{"SDTEST5.WAV", "Test 1", 1.5, false, false},
                      {"SDTEST6.WAV", "Test 2", 1.5, true, false},
                      {"SDTEST3.WAV", "Test 3", 1.5, false, true},
                      {"SDTEST4.WAV", "Test 4", 1.5, true, true}};

    Module mod_1(0, 33, tracks, &play_sd_wav_1, 0, &Wire);
    
    while(1) {
      mod_1.update(1);
    }

    // // stop, hammer time
    // Serial.println();
    // Serial.println("#######");
    // Serial.println("# end #");
    // Serial.println("#######");
    // while (1);
  
}

void loop() {}
