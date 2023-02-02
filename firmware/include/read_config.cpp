#include "read_config.h"
#include "helper_functions.h"


// TODO: hangs if you specify track numbers over this limit
const int n_modules = 8;

// return the number of keys in a table
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
Track table_to_track(toml_table_t* input_table, double default_gain, bool default_loop, bool default_play)
{
    Track track;
    // read in filename, this is the only required attribute, so hard fail if it's not here
    toml_datum_t fixed_track_filename = toml_string_in(input_table, "filename");
    if (!fixed_track_filename.ok) {
        fatal("Cannot find fixed track filename", "");
    }
    memcpy(track.filename, fixed_track_filename.u.s, MAX_CHAR);

    // read in name, use filename if not given
    toml_datum_t fixed_track_name = toml_string_in(input_table, "name");
    if (!fixed_track_name.ok) {
        // TODO: find a way to trim off the file extension for the name
        memcpy(track.name, fixed_track_filename.u.s, MAX_CHAR);
    } else {
        memcpy(track.name, fixed_track_name.u.s, MAX_CHAR);
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

// load all settings from the config file
void load_config()
{
    // first check if the file even exists
    if (SD.exists(config_file_name)) {
        Serial.print("config found, ");
    } else {
        fatal("config not found", "");
    }

    // create file to hold contents
    File config_file;

    // read toml file
    config_file = SD.open(config_file_name, FILE_READ);
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
    if (gif_table){
        toml_datum_t startup_gif_toml = toml_string_in(gif_table, "startup");
        if (startup_gif_toml.ok){
            memcpy(gif_name, startup_gif_toml.u.s, MAX_CHAR);
            free(startup_gif_toml.u.s);
        }
    }

    // traverse and extract any fixed tracks
    // arrays need to be outside and fixed size because dumb
    Track fixed_tracks[n_modules];
    toml_table_t* fixed_table = toml_table_in(conf, "fixed");
    if (fixed_table) {
        // fixed tracks present, get em
        int n_fixed_tracks = size_of_toml_table(fixed_table);
        Serial.print("found ");
        Serial.print(n_fixed_tracks);
        Serial.println(" fixed tracks");

        // populate array
        for (int i = 0; ; i++) {
            const char* key = toml_key_in(fixed_table, i);
            if (!key){
                break;
            }
            // error check the track number that was entered
            if (atoi(key) > n_modules){
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
    int n_scenes = size_of_toml_table(scenes_table);
    
    // assign the external variable for use outside the function
    *n_scenes_ptr = n_scenes; 

    // allocate for scene names
    scene_names = (char**)calloc(n_scenes, sizeof(char*));
    // Serial.printf("Scene names sizeof: %d\n", sizeof(scene_names));
    for (int i = 0; i < n_scenes; i++){
        scene_names[i] = (char*)calloc(1, MAX_CHAR);
        // Serial.printf("Scene names sizeof at %d: %d\n", i, sizeof(scene_names[i]));
    }

    // allocate for track array
    module_tracks = (Track**)calloc(n_modules, sizeof(Track*));
    for (int i = 0; i < n_modules; i++) {
        module_tracks[i] = (Track*)calloc(n_scenes, sizeof(Track));
    }

    Serial.printf("found %d scenes\n", n_scenes);

    // how to write to one of the tracks
    // module_tracks[0][0] = Track{String("filename.wav"), String("name"), 1.0, false, false};

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
            memcpy(scene_names[i], scene_key, MAX_CHAR);
        } else {
            memcpy(scene_names[i], scene_name.u.s, MAX_CHAR);
            free(scene_name.u.s);
        }

        Serial.printf("Scene: %s\n", scene_names[i]);

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
                module_tracks[j][i] = table_to_track(track_subtable, scene_gain, scene_loop, scene_play);
            }
        } else {
            // track array is present, so use that to load in all the tracks
            for (int j = 0;; j++) {
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
        if (!fixed_table) {
            break;
        }
        // otherwise we need to load the fixed tracks into the array
        for (int k = 0; k < n_modules; k++) {
            if (sizeof(fixed_tracks[k].filename) > 0) {
                module_tracks[k][i] = fixed_tracks[k];
            }
        }
    }
    return;
}