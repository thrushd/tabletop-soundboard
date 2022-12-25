#include "read_config.h"
#include "helper_functions.h"

// return the number of keys in a table
int size_of_toml_table(toml_table_t *table) {
  for (int i = 0; ; i++) {
      const char* key = toml_key_in(table, i);
      if (!key){
        return i;
      }
  }
  return 0;
}

// print out track for debugging
void print_track(Track input_track) {
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

// print out a scene for debugging
void print_scene(Scene input_scene) {
    Serial.print("Scene: ");
    Serial.println(input_scene.name);
    Serial.print("Scene gain: ");
    Serial.println(input_scene.gain);
    Serial.print("Scene loop: ");
    Serial.println(input_scene.loop);
    Serial.print("Scene play: ");
    Serial.println(input_scene.play);
    Serial.println();

    for (unsigned int i = 0; i < (sizeof(input_scene.track) / sizeof(input_scene.track[0])); i++) {
      Serial.print("Track ");
      Serial.println(i+1);
      print_track(input_scene.track[i]);
    }
    Serial.println();
}

// extract all track data to a track object from a given toml track table
Track extract_track_table(toml_table_t *input_table, double default_gain, bool default_loop, bool default_play) {
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
    strcpy(track.name, fixed_track_filename.u.s); //TODO: find a way to trim off the file extension for the name
  }
  else{
    strcpy(track.name, fixed_track_name.u.s);
    free(fixed_track_name.u.s);
  }
  free(fixed_track_filename.u.s);

  // read in gain, use default if not given
  toml_datum_t fixed_track_gain = toml_double_in(input_table, "gain");
  if (!fixed_track_gain.ok){
    track.gain = default_gain;
  }
  else {
    track.gain = fixed_track_gain.u.d;
  }

  // read in loop, use default if not given
  toml_datum_t fixed_track_loop = toml_bool_in(input_table, "loop");
  if (!fixed_track_loop.ok){
    track.loop = default_loop;
  }
  else {
    track.loop = fixed_track_loop.u.b;
  }

  // read in play, use default if not given
  toml_datum_t fixed_track_play = toml_bool_in(input_table, "play");
  if (!fixed_track_play.ok){
    track.play = default_play;
  }
  else {
    track.play = fixed_track_play.u.b;
  }
  return track;
}

// make a track object with only a filename and defaults
Track extract_track_array(const char *input_filename, double default_gain, bool default_loop, bool default_play){
  Track track;
  strcpy(track.filename, input_filename);
  strcpy(track.name, input_filename); //TODO: find a way to trim off the file extension for the name
  track.gain = default_gain;
  track.loop = default_loop;
  track.play = default_play;
  return track;
}

// load all settings from the config file
Scene *load_config(char config_filename[], int* n_scenes) {
  // first check if the file even exists
  if (SD.exists(config_filename)) {
    Serial.print("config found, ");
  }
  else {
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
    if (c == 0) break;  // end of string
    index = index + 1;
    if (index == config_size) break; // buffer full
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

  if (!conf) {
    fatal("cannot parse - ", errbuf);
  }
  Serial.println("parsed.");

  // find the default table, can't proceed without it
  toml_table_t *default_table = toml_table_in(conf, "default");
  if (!default_table) fatal("missing default settings", "");
  
  // extract default values, hard quit if none are present
  toml_datum_t default_loop_datum = toml_bool_in(default_table, "loop");
  if (!default_loop_datum.ok) fatal("missing default loop", "");
  bool default_loop = default_loop_datum.u.b;

  toml_datum_t default_play_datum = toml_bool_in(default_table, "play");
  if (!default_play_datum.ok) fatal("missing default play", "");
  bool default_play = default_play_datum.u.b;
  
  toml_datum_t default_gain_datum = toml_double_in(default_table, "gain");
  if (!default_gain_datum.ok) fatal("missing default gain", "");
  double default_gain = default_gain_datum.u.d;
  
  // traverse and extract any fixed tracks
  // arrays need to be outside and fixed size because dumb
  // TODO: make not dumb
  Track fixed_tracks[num_channels];
  toml_table_t *fixed_table = toml_table_in(conf, "fixed");
  if (fixed_table) {
    // fixed tracks present, get em
    int n_fixed_tracks = size_of_toml_table(fixed_table);
    Serial.print("found ");
    Serial.print(n_fixed_tracks);
    Serial.println(" fixed tracks");

    // populate array
    for (int i = 0; ; i++) {
      const char* key = toml_key_in(fixed_table, i);
      if (!key) break;
      toml_table_t *fixed_subtable = toml_table_in(fixed_table, key);
      fixed_tracks[atoi(key)-1] = extract_track_table(fixed_subtable, default_gain, default_loop, default_play);
    }
  }
  else {
    Serial.println("no fixed tracks found");
  }

  // find the scene table
  toml_table_t *scenes_table = toml_table_in(conf, "scenes");
  if (!scenes_table) {
    fatal("no scenes found", "");
  }

  // allocate for scene array
  *n_scenes = size_of_toml_table(scenes_table);
  Scene *scene_array = (Scene *)calloc(*n_scenes, sizeof(Scene));

  Serial.print("found ");
  Serial.print(*n_scenes);
  Serial.println(" scenes");

  // loop through all scenes to load them and their tracks into the array of scenes
  /* the chungus has arrived
  ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣧⠀⠀⠀⠀⠀⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣿⣧⠀⠀⠀⢰⡿⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⡟⡆⠀⠀⣿⡇⢻⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⠀⣿⠀⢰⣿⡇⢸⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⡄⢸⠀⢸⣿⡇⢸⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⣿⡇⢸⡄⠸⣿⡇⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢿⣿⢸⡅⠀⣿⢠⡏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⣿⣿⣥⣾⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⣿⣿⣿⣿⣿⣿⣆⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⣿⡿⡿⣿⣿⡿⡅⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⠉⠀⠉⡙⢔⠛⣟⢋⠦⢵⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣾⣄⠀⠀⠁⣿⣯⡥⠃⠀⢳⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣴⣿⡇⠀⠀⠀⠐⠠⠊⢀⠀⢸⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⢀⣴⣿⣿⣿⡿⠀⠀⠀⠀⠀⠈⠁⠀⠀⠘⣿⣄⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⣠⣿⣿⣿⣿⣿⡟⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⣿⣷⡀⠀⠀⠀
⠀⠀⠀⠀⣾⣿⣿⣿⣿⣿⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⣿⣿⣧⠀⠀
⠀⠀⠀⡜⣭⠤⢍⣿⡟⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⢛⢭⣗⠀
⠀⠀⠀⠁⠈⠀⠀⣀⠝⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠄⠠⠀⠀⠰⡅
⠀⠀⠀⢀⠀⠀⡀⠡⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠁⠔⠠⡕⠀
⠀⠀⠀⠀⣿⣷⣶⠒⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢰⠀⠀⠀⠀
⠀⠀⠀⠀⠘⣿⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠰⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠈⢿⣿⣦⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⠊⠉⢆⠀⠀⠀⠀
⠀⢀⠤⠀⠀⢤⣤⣽⣿⣿⣦⣀⢀⡠⢤⡤⠄⠀⠒⠀⠁⠀⠀⠀⢘⠔⠀⠀⠀⠀
⠀⠀⠀⡐⠈⠁⠈⠛⣛⠿⠟⠑⠈⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠉⠑⠒⠀⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
  */
  
  for (int i = 0; ; i++) {
    const char* scene_key = toml_key_in(scenes_table, i); // this will be the toml name of the scene ("tavern", "camp", etc)
    if (!scene_key) break;
    toml_table_t *scene_subtable = toml_table_in(scenes_table, scene_key); // this will be used to extract all the scene specific settings
    
    // get scene details and defaults
    toml_datum_t scene_name = toml_string_in(scene_subtable, "name");
    if (!scene_name.ok) {
      strcpy(scene_array[i].name, scene_key);
    }
    else {
      strcpy(scene_array[i].name, scene_name.u.s);
      free(scene_name.u.s);
    }
    toml_datum_t scene_gain = toml_double_in(scene_subtable, "gain");
    if (!scene_gain.ok) {
      scene_array[i].gain = default_gain;
    }
    else {
      scene_array[i].gain = scene_gain.u.d;
    }
    toml_datum_t scene_loop = toml_bool_in(scene_subtable, "loop");
    if (!scene_loop.ok) {
      scene_array[i].loop = default_loop;
    }
    else {
      scene_array[i].loop = scene_loop.u.b;
    }
    toml_datum_t scene_play = toml_bool_in(scene_subtable, "play");
    if (!scene_play.ok) {
      scene_array[i].play = default_play;
    }
    else {
      scene_array[i].play = scene_play.u.b;
    }

    toml_array_t* tracks_array = toml_array_in(scene_subtable, "tracks");
    if (!tracks_array){
      // minimal track array not found, loop through track keys to get values
      toml_table_t *track_table = toml_table_in(conf, scene_key);
      // get tracks
      for (int j = 0; ; j++) {
        // TODO: if no tracks are defined in the config program will just hang here, should have a way to error handle this
        const char* track_key = toml_key_in(track_table, j); // this will be the toml track number ("1", "2", "3", etc)
        if (!track_key) break;
        toml_table_t *track_subtable = toml_table_in(track_table, track_key); // table with the track settings
        if (!track_subtable) fatal("No tracks found for scene ", scene_key); // hard exit if no tracks are given for scene
        scene_array[i].track[j] = extract_track_table(track_subtable, scene_array[i].gain, scene_array[i].loop, scene_array[i].play);
      }
    }
    else {
      // track array is present, so use that to load in all the tracks
      for (int j = 0; ; j++){
        toml_datum_t track_attack = toml_string_at(tracks_array, j);
        if (!track_attack.ok) break;
        scene_array[i].track[j] = extract_track_array(track_attack.u.s, scene_array[i].gain, scene_array[i].loop, scene_array[i].play);
        free(track_attack.u.s);
      }
    }

    // load fixed tracks into scene, only if they are present
    if (fixed_table) {
      for (int k = 0; k < num_channels; k++) {
        if (strlen(fixed_tracks[k].filename) > 0) {
          scene_array[i].track[k] = fixed_tracks[k];
        }
      }
    }

  }
  Serial.println();
  return scene_array;
}