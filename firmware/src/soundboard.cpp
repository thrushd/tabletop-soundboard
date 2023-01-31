#include "soundboard.h"

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

SoundBoard::SoundBoard(char *config_file_name)
{
    this->config_file_name = config_file_name;
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

    if (!SD.begin(CHIP_SELECT)) {
        Serial.println();
        Serial.println("SD initialization failed");
        while (1) { }
    }
    Serial.println("done");

    // read config file
    load_config(config_file_name, scene_names, module_tracks, startup_gif_filename);
    
    Serial.printf("Scene: %s\n", scene_names[0]);
    

    // init display
    main_display = new Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
    main_display->init(MAIN_SCREEN_WIDTH, MAIN_SCREEN_HEIGHT);

    // init gifplayer and play the startup animation
    init_player(main_display);
    play_gif(startup_gif_filename);
    
    while (1) { }

    // Module mod_1(0, 33, tracks, &play_sd_wav_0, &left_mixer_0, &right_mixer_0, &Wire, &knobbo);
}

void SoundBoard::handle_single(void* ptr)
{
    SoundBoard* sb_ptr = (SoundBoard*)ptr;
    sb_ptr->process_single();
}

void SoundBoard::process_single()
{
    Serial.println("Encoder single click");
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