#include "gif_player.h"
#include "helper_functions.h"
#include "module.h"
#include "toml.h"
#include "track.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <Arduino.h>
#include <Audio.h>
#include <Encoder.h>
#include <OneButton.h>
#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioPlaySdWav play_sd_wav_1; // xy=765.0000152587891,499.0000190734863
AudioPlaySdWav play_sd_wav_2; // xy=765.0000152587891,539.0000228881836
AudioPlaySdWav play_sd_wav_3; // xy=765.0000152587891,580.0000228881836
AudioPlaySdWav play_sd_wav_4; // xy=765.0000190734863,624.0000190734863
AudioPlaySdWav play_sd_wav_5; // xy=765.0000190734863,667.0000200271606
AudioPlaySdWav play_sd_wav_0; // xy=766.0000152587891,455.000018119812
AudioPlaySdWav play_sd_wav_6; // xy=765.0000190734863,708.000020980835
AudioPlaySdWav play_sd_wav_7; // xy=766.0000190734863,748.0000228881836
AudioMixer4 right_mixer_0; // xy=1033.0001907348633,660.0000190734863
AudioMixer4 left_mixer_1; // xy=1034.0001907348633,539.0000553131104
AudioMixer4 right_mixer_1; // xy=1034.0001907348633,732.0000228881836
AudioMixer4 left_mixer_0; // xy=1036.0000305175781,469.000093460083
AudioMixer4 right_final_mixer; // xy=1293.0000381469727,646.0000190734863
AudioMixer4 left_final_mixer; // xy=1295.0000381469727,563.0000953674316
AudioOutputPT8211 pt8211_1; // xy=1524,598
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

/* hardware definition

Main display connections
VCC -> VIN
GND -> GND
DIN -> 11 (MOSI0)
CLK -> 13 (SCK0)
CS  -> 30
DC  -> 31
RST -> 32
BL  -> NC

I2C
SCL0: 19
SDA0: 18
*/

// screen
#define TFT_CS 30
#define TFT_RST 32 // Or set to -1 and connect to RESET pin
#define TFT_DC 31

// encoder knob
#define ENCODER_BUTTON_PIN 35
#define ENCODER_A_PIN 33
#define ENCODER_B_PIN 34
#define KNOB_SCALE 4

#define MAIN_BUTTON_PIN 36

// module buttons
const int button_pins[] = { 21, 20, 17, 16, 15, 14, 39, 29 };

char config_filename[] = { "config.toml" };
char startup_gif_filename[] = { "startup.gif" };

#define CHIP_SELECT BUILTIN_SDCARD

// variables you shouldn't change
#define N_MODULES 8
#define MAIN_SCREEN_WIDTH 240 // display width, in pixels
#define MAIN_SCREEN_HEIGHT 240 // display height, in pixels

int window_pos = 0; // position of the window in the array, defined as the minimum
int cursor_pos = 0; // position of the cursor in the window
int active_scene = -1; // the current active scene
int last_active_scene = -2; // the one before that
long new_position = 0; // position read from encoder
long old_position = -1; // the one before that
int knob_diff = 0; // difference between new position and old position since last read
int selection = 0; // index of the highlighted scene

char** scene_names;
int n_scenes;
Track** module_tracks;

double universal_gain = 0.5;
double gain_diff = 0;

Adafruit_ST7789 main_display = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

GFXcanvas16 canvas(MAIN_SCREEN_WIDTH, MAIN_SCREEN_HEIGHT);

OneButton knob_button = OneButton(ENCODER_BUTTON_PIN, true, true);
OneButton main_button = OneButton(MAIN_BUTTON_PIN, true, true);

Encoder knob(ENCODER_A_PIN, ENCODER_B_PIN);

Module modules[] = {
    { 0, button_pins[0], &play_sd_wav_0, &left_mixer_0, &right_mixer_0, &Wire, &knob },
    { 1, button_pins[1], &play_sd_wav_1, &left_mixer_0, &right_mixer_0, &Wire, &knob },
    { 2, button_pins[2], &play_sd_wav_2, &left_mixer_0, &right_mixer_0, &Wire, &knob },
    { 3, button_pins[3], &play_sd_wav_3, &left_mixer_0, &right_mixer_0, &Wire, &knob },
    { 4, button_pins[4], &play_sd_wav_4, &left_mixer_1, &right_mixer_1, &Wire, &knob },
    { 5, button_pins[5], &play_sd_wav_5, &left_mixer_1, &right_mixer_1, &Wire, &knob },
    { 6, button_pins[6], &play_sd_wav_6, &left_mixer_1, &right_mixer_1, &Wire, &knob },
    { 7, button_pins[7], &play_sd_wav_7, &left_mixer_1, &right_mixer_1, &Wire, &knob }
};

// number of entries in a given table
int size_of_toml_table(toml_table_t* table)
{
    for (int i = 0;; i++) {
        const char* key = toml_key_in(table, i);
        if (!key) {
            return i;
        }
    }
    return 0;
}

// print out track for debugging
void print_track(Track input_track)
{
    Serial.printf("Filename: %s\n", input_track.filename);
    Serial.printf("Name: %s\n", input_track.name);
    Serial.printf("Gain: %.2f\n", input_track.gain);
    Serial.printf("Loop: %d\n", input_track.loop);
    Serial.printf("Play: %d\n", input_track.play);
}

// extract all track data to a track object from a given toml track table, using defaults if no settings present
Track table_to_track(toml_table_t* input_table, double default_gain, bool default_loop, bool default_play)
{
    Track track;
    // read in filename, this is the only required attribute, so hard fail if it's not here
    toml_datum_t fixed_track_filename = toml_string_in(input_table, "filename");
    if (!fixed_track_filename.ok) {
        fatal("Cannot find fixed track filename", "");
    }
    strcpy(track.filename, fixed_track_filename.u.s);

    // read in name, use filename if not given
    toml_datum_t fixed_track_name = toml_string_in(input_table, "name");
    if (!fixed_track_name.ok) {
        // TODO: find a way to trim off the file extension for the name
        strcpy(track.name, fixed_track_filename.u.s);
    } else {
        strcpy(track.name, fixed_track_name.u.s);
        free(fixed_track_name.u.s);
    }
    free(fixed_track_filename.u.s);

    // read in gain, use default if not given
    toml_datum_t fixed_track_gain = toml_double_in(input_table, "gain");
    if (!fixed_track_gain.ok) {
        track.gain = default_gain;
    } else {
        track.gain = fixed_track_gain.u.d;
    }

    // read in loop, use default if not given
    toml_datum_t fixed_track_loop = toml_bool_in(input_table, "loop");
    if (!fixed_track_loop.ok) {
        track.loop = default_loop;
    } else {
        track.loop = fixed_track_loop.u.b;
    }

    // read in play, use default if not given
    toml_datum_t fixed_track_play = toml_bool_in(input_table, "play");
    if (!fixed_track_play.ok) {
        track.play = default_play;
    } else {
        track.play = fixed_track_play.u.b;
    }
    return track;
}

// load config file into track array
void load_config()
{
    // first check if the file even exists
    if (SD.exists(config_filename)) {
        Serial.print("config found, ");
    } else {
        fatal("config not found", "");
    }

    // create file to hold contents
    File config_file;

    // read toml file
    config_file = SD.open(config_filename, FILE_READ);
    if (!config_file) {
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
    while (config_file.available()) {
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
    toml_table_t* conf = toml_parse(tomlbuffer, errbuf, sizeof(errbuf));
    Serial.print("read, ");
    config_file.close();
    Serial.print("closed, ");

    if (!conf) {
        fatal("cannot parse - ", errbuf);
    }
    Serial.println("parsed.");

    // find the default table, can't proceed without it
    toml_table_t* default_table = toml_table_in(conf, "default");
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
    toml_table_t* gif_table = toml_table_in(conf, "gifs");
    if (gif_table) {
        toml_datum_t startup_gif_toml = toml_string_in(gif_table, "startup");
        if (startup_gif_toml.ok) {
            strcpy(startup_gif_filename, startup_gif_toml.u.s);
            free(startup_gif_toml.u.s);
        }
    }

    // traverse and extract any fixed tracks
    // arrays need to be outside and fixed size because dumb
    Track* fixed_tracks = (Track*)calloc(N_MODULES, sizeof(Track));

    toml_table_t* fixed_table = toml_table_in(conf, "fixed");
    if (fixed_table) {
        // fixed tracks present, get em
        int n_fixed_tracks = size_of_toml_table(fixed_table);
        Serial.print("found ");
        Serial.print(n_fixed_tracks);
        Serial.println(" fixed tracks");

        // populate array
        for (int i = 0;; i++) {
            const char* key = toml_key_in(fixed_table, i);
            if (!key) {
                break;
            }
            // error check the track number that was entered
            if (atoi(key) > N_MODULES) {
                fatal("fixed track number exeeds number of modules", "");
            }
            // extract table and get track
            toml_table_t* fixed_subtable = toml_table_in(fixed_table, key);
            fixed_tracks[atoi(key) - 1] = table_to_track(fixed_subtable, default_gain, default_loop, default_play);
        }
    } else {
        Serial.println("no fixed tracks found");
    }

    // find the scene table
    toml_table_t* scenes_table = toml_table_in(conf, "scenes");
    if (!scenes_table) {
        fatal("no scenes found", "");
    }

    // yo how many scenes we got
    n_scenes = size_of_toml_table(scenes_table);

    // allocate for scene names
    scene_names = (char**)calloc(n_scenes, sizeof(char*));
    for (int i = 0; i < n_scenes; i++) {
        scene_names[i] = (char*)calloc(1, MAX_CHAR);
    }

    // allocate for track array
    module_tracks = (Track**)calloc(n_scenes, sizeof(Track*));
    for (int i = 0; i < n_scenes; i++) {
        module_tracks[i] = (Track*)calloc(N_MODULES, sizeof(Track));
    }

    Serial.printf("found %d scenes\n", n_scenes);

    // loop through all scenes to load them and their tracks into the array
    for (int i = 0;; i++) {
        // for each scene
        const char* scene_key = toml_key_in(scenes_table, i); // this will be the toml name of the scene ("tavern", "camp", etc)
        if (!scene_key)
            break;

        toml_table_t* scene_subtable = toml_table_in(scenes_table, scene_key); // this will be used to extract all the scene specific settings

        // get scene names, if not provided use the key
        toml_datum_t scene_name = toml_string_in(scene_subtable, "name");
        if (!scene_name.ok) {
            strcpy(scene_names[i], scene_key);
        } else {
            strcpy(scene_names[i], scene_name.u.s);
            free(scene_name.u.s);
        }

        // Serial.printf("Scene: %s\n", scene_names[i]);

        // get scene defaults
        double scene_gain = 0;
        bool scene_loop = false;
        bool scene_play = false;

        toml_datum_t scene_gain_toml = toml_double_in(scene_subtable, "gain");
        if (!scene_gain_toml.ok) {
            scene_gain = default_gain;
        } else {
            scene_gain = scene_gain_toml.u.d;
        }
        toml_datum_t scene_loop_toml = toml_bool_in(scene_subtable, "loop");
        if (!scene_loop_toml.ok) {
            scene_loop = default_loop;
        } else {
            scene_loop = scene_loop_toml.u.b;
        }
        toml_datum_t scene_play_toml = toml_bool_in(scene_subtable, "play");
        if (!scene_play_toml.ok) {
            scene_play = default_play;
        } else {
            scene_play = scene_play_toml.u.b;
        }

        // get array called 'tracks' in the scene subtable
        toml_array_t* tracks_array = toml_array_in(scene_subtable, "tracks");
        if (!tracks_array) {
            // minimal track array not found, loop through track keys to get values
            toml_table_t* track_table = toml_table_in(conf, scene_key);
            // get tracks
            for (int j = 0;; j++) {
                // TODO: if no tracks are defined in the config program will just hang here, should have a way to error handle this
                const char* track_key = toml_key_in(track_table, j); // this will be the toml track number ("1", "2", "3", etc)
                // exit if we run out
                if (!track_key)
                    break;
                // get a table with the track settings
                toml_table_t* track_subtable = toml_table_in(track_table, track_key); // table with the track settings
                if (!track_subtable)
                    fatal("No tracks found for scene ", scene_key); // hard exit if no tracks are given for scene
                // load the track object into the array
                module_tracks[i][j] = table_to_track(track_subtable, scene_gain, scene_loop, scene_play); // The SAME?!?!?!?
            }
        } else {
            // track array is present, so use that to load in all the tracks
            for (int j = 0;; j++) {
                toml_datum_t track_array = toml_string_at(tracks_array, j);
                if (!track_array.ok)
                    break;
                // copy contents to the array
                strcpy(module_tracks[i][j].filename, track_array.u.s);
                strcpy(module_tracks[i][j].name, track_array.u.s);
                module_tracks[i][j].gain = scene_gain;
                module_tracks[i][j].loop = scene_loop;
                module_tracks[i][j].play = scene_play;
                free(track_array.u.s);
            }
        }

        // if there is no fixed table we can exit now
        if (!fixed_table) {
            break;
        }
        // otherwise we need to load the fixed tracks into the array
        for (int j = 0; j < N_MODULES; j++) {
            if (strlen(fixed_tracks[j].filename) > 0) {
                module_tracks[i][j] = fixed_tracks[j];
            }
        }
    }
    return;
}

// // updates main display scenes
void update_main_display()
{
    // character sizes in pixels, for later calculations
    // size [n] character [width/height]
    // int size_1_c_w = 6;
    // int size_1_c_h = 8;
    int size_2_c_w = 12;
    int size_2_c_h = 16;
    // int size_3_c_w = 18;
    int size_3_c_h = 24;

    int side_offset = 24; // offset of side bar showing the selection indicator, allows for 12 size 3 characters
    int bottom_offset = 24; // offset of bottom bar showing universal gain setting

    // int max_chars_2 = (MAIN_SCREEN_WIDTH - side_offset) / size_2_c_w; // maximum number of size 3 characters in a row
    // int max_chars_3 = (MAIN_SCREEN_WIDTH - side_offset) / size_3_c_w; // maximum number of size 3 characters in a row
    int max_rows = (MAIN_SCREEN_HEIGHT - bottom_offset) / size_3_c_h; // maximum number of rows of text that can be shown

    // clear and set text parameters
    canvas.fillScreen(ST77XX_BLACK);
    canvas.setTextSize(1); // scale of 3 makes the characters 18x24
    canvas.setTextColor(ST77XX_WHITE); // Draw white text
    canvas.setTextWrap(false); // if it's too long, oh well

    // if selection is larger than the current window bounds, move the window down
    if (selection > window_pos + (max_rows - 1)) {
        window_pos++;
        cursor_pos = max_rows - 1;
    }
    // if selection is smaller than the current window bounds, move the window up
    else if (selection < window_pos) {
        window_pos--;
        cursor_pos = 0;
    }
    // set the cursor position within the window
    cursor_pos = selection - window_pos;

    // draw volume slider
    int v_x = 4 * size_2_c_w + 2;
    int v_y = MAIN_SCREEN_HEIGHT - bottom_offset;
    int v_w = MAIN_SCREEN_WIDTH - v_x - 2;
    int v_h = bottom_offset;
    int v_r = v_h / 3; // radius of position marker
    int v_pos = (v_w - 2 * v_r) * (universal_gain / GAIN_MAX); // position of the marker on the line
    canvas.drawLine(v_x, v_y + v_h / 2, v_x + v_w, v_y + v_h / 2, ST77XX_WHITE); // horizontal line
    canvas.drawLine(v_x, v_y, v_x, v_y + v_h, ST77XX_WHITE); // left vertical
    canvas.drawLine(v_x + v_w, v_y, v_x + v_w, v_y + v_h, ST77XX_WHITE); // right vertical
    canvas.drawLine(v_x + v_w / 2, v_y + 4, v_x + v_w / 2, v_y + v_h - 4, ST77XX_WHITE); // center vertical
    canvas.fillCircle(v_x + v_pos + v_r, v_y + v_h / 2, v_r, ST77XX_CYAN); // position marker

    // draw gain text
    canvas.setTextSize(2);
    canvas.setCursor(2, MAIN_SCREEN_HEIGHT - bottom_offset + (size_3_c_h - size_2_c_h) / 2 + 1);
    canvas.printf("G%.1f", universal_gain);

    // draw the cursor position
    canvas.fillCircle(side_offset / 2, (size_3_c_h * cursor_pos) + size_3_c_h / 2, side_offset / 2 - 6, ST77XX_YELLOW);

    // draw all the text
    // if the length of the scene name is greater than the avialable characters downsize the text
    // center each name withing it's allotted vertical bar space
    for (int i = 0; i < max_rows; i++) {
        // for each row draw the scene name, break if we go over the number of scenes though
        if (i >= n_scenes)
            break;

        // // first determine which size text to use
        // if (strlen(scene_names[i + window_pos]) > max_chars_3) {
        //     // size 3 will overlap, go down a size and center
        //     canvas.setTextSize(2);
        //     canvas.setCursor(side_offset, (i * size_3_c_h) + (size_3_c_h - size_2_c_h) / 2);
        // } else {
        //     // size 3 will fit
        //     canvas.setTextSize(3);
        //     canvas.setCursor(side_offset, i * size_3_c_h);
        // }

        // canvas.setTextSize(3);
        // canvas.setCursor(side_offset, i * size_3_c_h);
        canvas.setTextSize(2);
        canvas.setCursor(side_offset, (i * size_3_c_h) + (size_3_c_h - size_2_c_h) / 2);

        // check if the active scene is being written to, if so change the color
        if (i + window_pos == active_scene) {
            canvas.setTextColor(ST77XX_MAGENTA);
        } else {
            canvas.setTextColor(ST77XX_WHITE);
        }
        // print out the scene name
        canvas.print(scene_names[i + window_pos]);
    }

    // pretty border
    canvas.drawRect(0, 0, MAIN_SCREEN_WIDTH, MAIN_SCREEN_HEIGHT, ST77XX_GREEN);
    canvas.drawLine(0, MAIN_SCREEN_HEIGHT - bottom_offset, MAIN_SCREEN_WIDTH, MAIN_SCREEN_HEIGHT - bottom_offset, ST77XX_GREEN);

    // write it out baby
    main_display.drawRGBBitmap(0, 0, canvas.getBuffer(), canvas.width(), canvas.height());
}

// select scene
static void handle_encoder_single()
{
    // if encoder is pressed, check if selection actually changes the scene
    if (selection != active_scene) {
        // need to stop playing all tracks
        play_sd_wav_0.stop();
        play_sd_wav_1.stop();
        play_sd_wav_2.stop();
        play_sd_wav_3.stop();
        play_sd_wav_4.stop();
        play_sd_wav_5.stop();
        play_sd_wav_6.stop();
        play_sd_wav_7.stop();
        // reset the active scene
        active_scene = selection;
        update_main_display();
    }
}

// stop playing
static void handle_main_single()
{
    // stop playing all tracks
    for (int i = 0; i < N_MODULES; i++) {
        modules[i].stop();
    }
}

// adjust global gain
static void handle_main_hold()
{
    universal_gain = universal_gain + gain_diff;

    if (universal_gain >= GAIN_MAX)
        universal_gain = GAIN_MAX;
    else if (universal_gain < GAIN_MIN)
        universal_gain = GAIN_MIN;

    gain_diff = 0;

    // update main gain
    left_final_mixer.gain(0, universal_gain);
    left_final_mixer.gain(1, universal_gain);
    right_final_mixer.gain(0, universal_gain);
    right_final_mixer.gain(1, universal_gain);
}

// i hate this so much T__T
int check_holds()
{
    // check main button first
    if (main_button.isLongPressed()) {
        return selection;
    }

    // check modules next
    for (int i = 0; i < N_MODULES; i++) {
        if (modules[i].button.isLongPressed())
            return selection;
    }

    // none are being held, so return the difference
    return selection + knob_diff;
}

// le setup
void setup()
{
    Serial.begin(9600);
    Wire.begin();
    AudioMemory(16);
    // wait for serial to open
    // while (!Serial) { }

    Serial.println();
    Serial.println("starting this heckin trainwreck now");

    knob_button.attachClick(handle_encoder_single);

    main_button.attachClick(handle_main_single);
    main_button.attachDuringLongPress(handle_main_hold);

    // set up SD card
    Serial.print("initializing SD card... ");

    if (!SD.begin(CHIP_SELECT)) {
        Serial.println();
        Serial.println("SD initialization failed");
        while (1) {
        }
    }
    Serial.println("done");

    // read config file
    load_config();

    Serial.println();
    for (int i = 0; i < n_scenes; i++) {
        for (int j = 0; j < N_MODULES; j++) {
            Serial.printf("Scene %d, Track %d\n", i, j);
            print_track(module_tracks[i][j]);
            Serial.println();
        }
        Serial.println();
    }

    // init display
    main_display.init(MAIN_SCREEN_WIDTH, MAIN_SCREEN_HEIGHT);

    // init gifplayer and play the startup animation
    init_player(&main_display);
    // play_gif(startup_gif_filename);

    for (int i = 0; i < N_MODULES; i++) {
        modules[i].init(module_tracks);
    }
}

// le loop
void loop()
{
    // tick that button
    knob_button.tick();
    main_button.tick();

    // read knob position and update
    new_position = knob.read();

    if (new_position != old_position) {
        // get the difference
        knob_diff = (old_position / KNOB_SCALE) - (new_position / KNOB_SCALE);
        // find the gain difference and scale it to be within the 0-1 range
        // gain_diff = double(((old_position - new_position)) * (GAIN_MAX - GAIN_MIN) / 80 + GAIN_MIN);
        gain_diff = double(map_float((old_position - new_position), 0, 80, GAIN_MIN, GAIN_MAX));

        // reset
        old_position = new_position;

        selection = check_holds();

        // constrain the selection between bounds
        if (selection >= n_scenes - 1) {
            selection = n_scenes - 1;
        } else if (selection < 0) {
            selection = 0;
        }
        // reset diff
        knob_diff = 0;
        update_main_display();
    }

    // update modules
    for (int i = 0; i < N_MODULES; i++) {
        modules[i].update(active_scene);
    }
}
