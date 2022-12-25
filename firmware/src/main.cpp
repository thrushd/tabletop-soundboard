#include "read_config.h"
#include "display.h"
// #include "helper_functions.h"

char config_file_name[] = "config.toml";
// testing configs
// char config_file_name[] = "config_no_defaults.toml";
// char config_file_name[] = "config_no_fixed.toml";
// char config_file_name[] = "config_no_scenes.toml";
// char config_file_name[] = "config_no_track_data.toml";
// char config_file_name[] = "config_no_tracks.toml";

void setup() {
    Serial.begin(115200);

    // wait for serial to open
    while (!Serial) {}

    // set up SD card
    Serial.println();
    Serial.print("initializing SD card... ");
  
    if (!SD.begin(chipSelect)) {
      Serial.println();
      fatal("SD initialization failed", "");
    }
    Serial.println("done");
  
    // get array of scenes from the config
    Scene *scenes; // array of scenes
    int n_scenes = 0; // length of array of scenes
    scenes = load_config(config_file_name, &n_scenes);

    // print them
    // for (int i = 0; i < n_scenes; i++) {
    //   print_scene(scenes[i]);
    // }

    Serial.println("scenes loaded");

    display_setup();
    Serial.println("display init");
    display_startup_splash();

    // stop, hammer time
    Serial.println();
    Serial.println("#######");
    Serial.println("# end #");
    Serial.println("#######");
    while (1);
  
}

void loop() {}
