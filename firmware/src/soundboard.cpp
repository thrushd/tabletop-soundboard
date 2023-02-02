#include "soundboard.h"

// GUItool: begin automatically generated code
AudioPlaySdWav play_sd_wav_1;  // xy=765.0000152587891,499.0000190734863
AudioPlaySdWav play_sd_wav_2;  // xy=765.0000152587891,539.0000228881836
AudioPlaySdWav play_sd_wav_3;  // xy=765.0000152587891,580.0000228881836
AudioPlaySdWav play_sd_wav_4;  // xy=765.0000190734863,624.0000190734863
AudioPlaySdWav play_sd_wav_5;  // xy=765.0000190734863,667.0000200271606
AudioPlaySdWav play_sd_wav_0;  // xy=766.0000152587891,455.000018119812
AudioPlaySdWav play_sd_wav_6;  // xy=765.0000190734863,708.000020980835
AudioPlaySdWav play_sd_wav_7;  // xy=766.0000190734863,748.0000228881836
AudioMixer4 right_mixer_0;     // xy=1033.0001907348633,660.0000190734863
AudioMixer4 left_mixer_1;      // xy=1034.0001907348633,539.0000553131104
AudioMixer4 right_mixer_1;     // xy=1034.0001907348633,732.0000228881836
AudioMixer4 left_mixer_0;      // xy=1036.0000305175781,469.000093460083
AudioMixer4 right_final_mixer; // xy=1293.0000381469727,646.0000190734863
AudioMixer4 left_final_mixer;  // xy=1295.0000381469727,563.0000953674316
AudioOutputPT8211 pt8211_1;    // xy=1524,598
AudioConnection patchCord1(play_sd_wav_1, 0, left_mixer_0, 1);
AudioConnection patchCord2(play_sd_wav_1, 1, right_mixer_0, 1);
AudioConnection patchCord3(play_sd_wav_2, 0, left_mixer_0, 2);
AudioConnection patchCord4(play_sd_wav_2, 1, right_mixer_0, 2);
AudioConnection patchCord5(play_sd_wav_3, 0, left_mixer_0, 3);
AudioConnection patchCord6(play_sd_wav_3, 1, right_mixer_0, 3);
AudioConnection patchCord7(play_sd_wav_4, 0, left_mixer_1, 0);
AudioConnection patchCord8(play_sd_wav_4, 1, right_mixer_1, 0);
AudioConnection patchCord9(play_sd_wav_5, 0, left_mixer_1, 1);
AudioConnection patchCord10(play_sd_wav_5, 1, right_mixer_1, 1);
AudioConnection patchCord11(play_sd_wav_0, 0, left_mixer_0, 0);
AudioConnection patchCord12(play_sd_wav_0, 1, right_mixer_0, 0);
AudioConnection patchCord13(play_sd_wav_6, 0, left_mixer_1, 2);
AudioConnection patchCord14(play_sd_wav_6, 1, right_mixer_1, 2);
AudioConnection patchCord15(play_sd_wav_7, 0, left_mixer_1, 3);
AudioConnection patchCord16(play_sd_wav_7, 1, right_mixer_1, 3);
AudioConnection patchCord17(right_mixer_0, 0, right_final_mixer, 0);
AudioConnection patchCord18(left_mixer_1, 0, left_final_mixer, 1);
AudioConnection patchCord19(right_mixer_1, 0, right_final_mixer, 1);
AudioConnection patchCord20(left_mixer_0, 0, left_final_mixer, 0);
AudioConnection patchCord21(right_final_mixer, 0, pt8211_1, 1);
AudioConnection patchCord22(left_final_mixer, 0, pt8211_1, 0);
// GUItool: end automatically generated code

SoundBoard::SoundBoard(char *config_file_name)
{
    memcpy(this->config_file_name, config_file_name, MAX_CHAR);
    init();
}

void SoundBoard::update()
{
}

void SoundBoard::init()
{
    // init encoder button
    button = OneButton(ENCODER_BUTTON_PIN, true, true);
    button.attachClick(handle_single, this);

    // init encoder
    // Encoder knob(ENCODER_A_PIN, ENCODER_B_PIN);

    // set up SD card
    Serial.print("initializing SD card... ");

    if (!SD.begin(CHIP_SELECT))
    {
        Serial.println();
        Serial.println("SD initialization failed");
        while (1)
        {
        }
    }
    Serial.println("done");

    // read config file
    load_config();

    Serial.printf("Number of Scenes: %d\n\n", n_scenes);

    for (int i = 0; i < n_scenes; i++)
    {
        Serial.printf("Scene: %s\n", scene_names[i]);
    }

    Serial.println("here");

    for (int i = 0; i < n_scenes; i++)
    {
        Serial.printf("Scene: %s\n", scene_names[i]);
        for (int j = 0; j < 8; j++)
        {
            Serial.printf("Track %d\n", j + 1);
            Serial.printf("Filename: %s\n", module_tracks[i][j].filename);
            Serial.printf("Name: %s\n", module_tracks[i][j].name);
            Serial.printf("Gain: %s\n", module_tracks[i][j].gain);
            Serial.printf("Loop: %s\n", module_tracks[i][j].loop);
            Serial.printf("Play: %s\n", module_tracks[i][j].play);
            Serial.println();
        }
    }

    // init display
    main_display = new Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
    main_display->init(MAIN_SCREEN_WIDTH, MAIN_SCREEN_HEIGHT);

    // init gifplayer and play the startup animation
    init_player(main_display);
    play_gif(startup_gif_filename);

    while (1)
    {
    }

    // Module mod_1(0, 33, tracks, &play_sd_wav_0, &left_mixer_0, &right_mixer_0, &Wire, &knobbo);
}

void SoundBoard::handle_single(void *ptr)
{
    SoundBoard *sb_ptr = (SoundBoard *)ptr;
    sb_ptr->process_single();
}

void SoundBoard::process_single()
{
    Serial.println("Encoder single click");
}

int SoundBoard::size_of_toml_table(toml_table_t *table)
{
    for (int i = 0;; i++)
    {
        const char *key = toml_key_in(table, i);
        if (!key)
        {
            return i;
        }
    }
    return 0;
}

// print out track for debugging
void SoundBoard::print_track(Track input_track)
{
    Serial.print("Filename: ");
    Serial.println(input_track.filename);
    Serial.print("Name: ");
    Serial.println(input_track.name);
    Serial.print("Gain: ");
    Serial.println(input_track.gain);
    Serial.print("Loop: ");
    Serial.println(input_track.loop);
    Serial.print("Play: ");
    Serial.println(input_track.play);
    Serial.println();
}

// extract all track data to a track object from a given toml track table, using defaults if no settings present
Track SoundBoard::table_to_track(toml_table_t *input_table, double default_gain, bool default_loop, bool default_play)
{
    Track track;
    // read in filename, this is the only required attribute, so hard fail if it's not here
    toml_datum_t fixed_track_filename = toml_string_in(input_table, "filename");
    if (!fixed_track_filename.ok)
    {
        fatal("Cannot find fixed track filename", "");
    }
    memcpy(track.filename, fixed_track_filename.u.s, MAX_CHAR);

    // read in name, use filename if not given
    toml_datum_t fixed_track_name = toml_string_in(input_table, "name");
    if (!fixed_track_name.ok)
    {
        // TODO: find a way to trim off the file extension for the name
        memcpy(track.name, fixed_track_filename.u.s, MAX_CHAR);
    }
    else
    {
        memcpy(track.name, fixed_track_name.u.s, MAX_CHAR);
        free(fixed_track_name.u.s);
    }
    free(fixed_track_filename.u.s);

    // read in gain, use default if not given
    toml_datum_t fixed_track_gain = toml_double_in(input_table, "gain");
    if (!fixed_track_gain.ok)
    {
        track.gain = default_gain;
    }
    else
    {
        track.gain = fixed_track_gain.u.d;
    }

    // read in loop, use default if not given
    toml_datum_t fixed_track_loop = toml_bool_in(input_table, "loop");
    if (!fixed_track_loop.ok)
    {
        track.loop = default_loop;
    }
    else
    {
        track.loop = fixed_track_loop.u.b;
    }

    // read in play, use default if not given
    toml_datum_t fixed_track_play = toml_bool_in(input_table, "play");
    if (!fixed_track_play.ok)
    {
        track.play = default_play;
    }
    else
    {
        track.play = fixed_track_play.u.b;
    }
    return track;
}

void SoundBoard::load_config()
{
    // first check if the file even exists
    if (SD.exists(config_file_name))
    {
        Serial.print("config found, ");
    }
    else
    {
        fatal("config not found", "");
    }

    // create file to hold contents
    File config_file;

    // read toml file
    config_file = SD.open(config_file_name, FILE_READ);
    if (!config_file)
    {
        fatal("cannot open config ", "");
    }
    Serial.print("opened, ");

    // read file into a char buffer so it can be fed into the parser
    // A bit hacky right now, if a user inputs a massive config file this might cause issues
    // TODO: Make toml read buffer less hacky, maybe using malloc?
    int config_size = config_file.size();
    char tomlbuffer[config_size];
    char errbuf[200];
    int index = 0;
    while (config_file.available())
    {
        char c = config_file.read();
        tomlbuffer[index] = c;
        if (c == 0)
            break; // end of string
        index = index + 1;
        if (index == config_size)
            break; // buffer full
    }
    tomlbuffer[index] = 0;
    // print out the raw file for debugging
    // Serial.println("raw file contents: ");
    // Serial.println("");
    // Serial.println(tomlbuffer);

    // parse the file
    toml_table_t *conf = toml_parse(tomlbuffer, errbuf, sizeof(errbuf));
    Serial.print("read, ");
    config_file.close();
    Serial.print("closed, ");

    if (!conf)
    {
        fatal("cannot parse - ", errbuf);
    }
    Serial.println("parsed.");

    // find the default table, can't proceed without it
    toml_table_t *default_table = toml_table_in(conf, "default");
    if (!default_table)
        fatal("missing default settings", "");

    // extract default values, hard quit if none are present
    toml_datum_t default_loop_datum = toml_bool_in(default_table, "loop");
    if (!default_loop_datum.ok)
        fatal("missing default loop", "");
    bool default_loop = default_loop_datum.u.b;

    toml_datum_t default_play_datum = toml_bool_in(default_table, "play");
    if (!default_play_datum.ok)
        fatal("missing default play", "");
    bool default_play = default_play_datum.u.b;

    toml_datum_t default_gain_datum = toml_double_in(default_table, "gain");
    if (!default_gain_datum.ok)
        fatal("missing default gain", "");
    double default_gain = default_gain_datum.u.d;

    // extract the gif name
    toml_table_t *gif_table = toml_table_in(conf, "gifs");
    if (gif_table)
    {
        toml_datum_t startup_gif_toml = toml_string_in(gif_table, "startup");
        if (startup_gif_toml.ok)
        {
            memcpy(startup_gif_filename, startup_gif_toml.u.s, MAX_CHAR);
            free(startup_gif_toml.u.s);
        }
    }

    // traverse and extract any fixed tracks
    // arrays need to be outside and fixed size because dumb
    Track fixed_tracks[N_MODULES];
    toml_table_t *fixed_table = toml_table_in(conf, "fixed");
    if (fixed_table)
    {
        // fixed tracks present, get em
        int n_fixed_tracks = size_of_toml_table(fixed_table);
        Serial.print("found ");
        Serial.print(n_fixed_tracks);
        Serial.println(" fixed tracks");

        // populate array
        for (int i = 0;; i++)
        {
            const char *key = toml_key_in(fixed_table, i);
            if (!key)
            {
                break;
            }
            // error check the track number that was entered
            if (atoi(key) > N_MODULES)
            {
                fatal("fixed track number exeeds number of modules", "");
            }
            // extract table and get track
            toml_table_t *fixed_subtable = toml_table_in(fixed_table, key);
            fixed_tracks[atoi(key) - 1] = table_to_track(fixed_subtable, default_gain, default_loop, default_play);
        }
    }
    else
    {
        Serial.println("no fixed tracks found");
    }

    // find the scene table
    toml_table_t *scenes_table = toml_table_in(conf, "scenes");
    if (!scenes_table)
    {
        fatal("no scenes found", "");
    }

    // yo how many scenes we got
    n_scenes = size_of_toml_table(scenes_table);

    // allocate for scene names
    scene_names = (char **)calloc(n_scenes, sizeof(char *));
    for (int i = 0; i < n_scenes; i++)
    {
        scene_names[i] = (char *)calloc(1, MAX_CHAR);
    }

    // allocate for track array
    module_tracks = (Track **)calloc(N_MODULES, sizeof(Track *));
    for (int i = 0; i < N_MODULES; i++)
    {
        module_tracks[i] = (Track *)calloc(n_scenes, sizeof(Track));
    }

    Serial.printf("found %d scenes\n", n_scenes);

    // how to write to one of the tracks
    // module_tracks[0][0] = Track{String("filename.wav"), String("name"), 1.0, false, false};

    // loop through all scenes to load them and their tracks into the array
    for (int i = 0;; i++)
    {
        // for each scene
        const char *scene_key = toml_key_in(scenes_table, i); // this will be the toml name of the scene ("tavern", "camp", etc)
        if (!scene_key)
            break;

        toml_table_t *scene_subtable = toml_table_in(scenes_table, scene_key); // this will be used to extract all the scene specific settings

        // get scene names, if not provided use the key
        toml_datum_t scene_name = toml_string_in(scene_subtable, "name");
        if (!scene_name.ok)
        {
            memcpy(scene_names[i], scene_key, MAX_CHAR);
        }
        else
        {
            memcpy(scene_names[i], scene_name.u.s, MAX_CHAR);
            free(scene_name.u.s);
        }

        Serial.printf("Scene: %s\n", scene_names[i]);

        // get scene defaults
        double scene_gain = 0;
        bool scene_loop = false;
        bool scene_play = false;

        toml_datum_t scene_gain_toml = toml_double_in(scene_subtable, "gain");
        if (!scene_gain_toml.ok)
        {
            scene_gain = default_gain;
        }
        else
        {
            scene_gain = scene_gain_toml.u.d;
        }
        toml_datum_t scene_loop_toml = toml_bool_in(scene_subtable, "loop");
        if (!scene_loop_toml.ok)
        {
            scene_loop = default_loop;
        }
        else
        {
            scene_loop = scene_loop_toml.u.b;
        }
        toml_datum_t scene_play_toml = toml_bool_in(scene_subtable, "play");
        if (!scene_play_toml.ok)
        {
            scene_play = default_play;
        }
        else
        {
            scene_play = scene_play_toml.u.b;
        }

        // get array called 'tracks' in the scene subtable
        toml_array_t *tracks_array = toml_array_in(scene_subtable, "tracks");
        if (!tracks_array)
        {
            // minimal track array not found, loop through track keys to get values
            toml_table_t *track_table = toml_table_in(conf, scene_key);
            // get tracks
            for (int j = 0;; j++)
            {
                // TODO: if no tracks are defined in the config program will just hang here, should have a way to error handle this
                const char *track_key = toml_key_in(track_table, j); // this will be the toml track number ("1", "2", "3", etc)
                // exit if we run out
                if (!track_key)
                    break;
                // get a table with the track settings
                toml_table_t *track_subtable = toml_table_in(track_table, track_key); // table with the track settings
                if (!track_subtable)
                    fatal("No tracks found for scene ", scene_key); // hard exit if no tracks are given for scene
                // load the track object into the array
                module_tracks[j][i] = table_to_track(track_subtable, scene_gain, scene_loop, scene_play);
            }
        }
        else
        {
            // track array is present, so use that to load in all the tracks
            for (int j = 0;; j++)
            {
                toml_datum_t track_array = toml_string_at(tracks_array, j);
                if (!track_array.ok)
                    break;
                memcpy(module_tracks[j][i].filename, track_array.u.s, MAX_CHAR);
                memcpy(module_tracks[j][i].name, track_array.u.s, MAX_CHAR);
                module_tracks[j][i].gain = scene_gain;
                module_tracks[j][i].loop = scene_loop;
                module_tracks[j][i].play = scene_play;
                free(track_array.u.s);
            }
        }

        // if there is no fixed table we can exit now
        if (!fixed_table)
        {
            break;
        }
        // otherwise we need to load the fixed tracks into the array
        for (int k = 0; k < N_MODULES; k++)
        {
            if (sizeof(fixed_tracks[k].filename) > 0)
            {
                module_tracks[k][i] = fixed_tracks[k];
            }
        }
    }
    return;
}

/*

void display_setup() {
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!main_display.begin(SSD1306_SWITCHCAPVCC, MAIN_SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    fatal("failed to set up display", "");
  }
  main_display.clearDisplay();
}


void display_text_test() {
  main_display.clearDisplay();

  main_display.setTextSize(2); // scale of 2 gives 4x10 characters on the 128x64 display, 12x16 pixels
  main_display.setTextColor(SSD1306_WHITE); // Draw white text
  main_display.setCursor(1, 1);     // Start at top-left corner
  main_display.print("Mountains");
  main_display.setCursor(1, 16+1);     // Start at top-left corner
  main_display.print("Tavern");
  main_display.setCursor(1, 32+1);     // Start at top-left corner
  main_display.print("Camp");
  main_display.setCursor(1, 48+1);     // Start at top-left corner
  main_display.print("Castle");
  main_display.fillRect(0, 16*3, MAIN_SCREEN_WIDTH, 16, SSD1306_INVERSE);

  main_display.display();
  while(1);
}

// updates main display scenes
void update_main_display(String scene_names[], int selection, int current_scene){
  // clear and set text parameters
  main_display.clearDisplay();
  main_display.setTextSize(2); // scale of 2 gives 4x10 characters on the 128x64 display, 12x16 pixels
  main_display.setTextColor(SSD1306_WHITE); // Draw white text

  // if selection is larger than the current window bounds, move the window down
  if (selection > window_pos + 2){
    window_pos++;
    cursor_pos = 2;
  }
  // if selection is smaller than the current window bounds, move the window up
  else if (selection < window_pos) {
    window_pos--;
    cursor_pos = 0;
  }

  // set the cursor position within the window
  cursor_pos = selection - window_pos;

  // draw all the text
  main_display.setCursor(1, 1);     // Start at top-left corner
  main_display.print(scene_names[window_pos]);
  main_display.setCursor(1, 16+1);     // Start at top-left corner
  main_display.print(scene_names[window_pos + 1]);
  main_display.setCursor(1, 32+1);     // Start at top-left corner
  main_display.print(scene_names[window_pos + 2]);
  // draw the cursor
  main_display.fillRect(0, 16*cursor_pos, MAIN_SCREEN_WIDTH, 16, SSD1306_INVERSE);
  // write
  main_display.display();
}

void display_encoder_test() {

  int curr_active_scene = 0; // the active scene
  int last_active_scene = -1; // the one before that

  String scenerenos[8] = {
    "Moutains",
    "Lake",
    "Dungeon",
    "Temple",
    "Tavern",
    "Camp",
    "Fight",
    "Spooky",
  };

  while(1){
    long new_position = knob.read() / knob_scale;
    knob_button.update();

    // constrain the position between bounds
    if (new_position > 7){
      new_position = 7;
      knob.write(7 * knob_scale);
    }
    else if (new_position < 0) {
      new_position = 0;
      knob.write(0);
    }

    if (knob_button.fallingEdge()) {
      curr_active_scene = new_position;
      if (curr_active_scene != last_active_scene){
        // add indication of current active scene
        scenerenos[curr_active_scene] = ">" + scenerenos[curr_active_scene];
        // remove the last one
        scenerenos[last_active_scene].remove(0, 1);
        last_active_scene = curr_active_scene;
      }


    }

    // if position has changed or the knob has been pressed update the display
    if ((new_position != old_position) || knob_button.fallingEdge()){
      old_position = new_position;
      update_main_display(scenerenos, new_position, curr_active_scene);
    }
  }
}

*/