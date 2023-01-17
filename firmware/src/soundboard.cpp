#include "soundboard.h"

SoundBoard::SoundBoard(String config_file_name)
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

    // init display
    main_display = new Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
    main_display->init(MAIN_SCREEN_WIDTH, MAIN_SCREEN_HEIGHT);

    // init gifplayer and play the startup animation
    init_player(main_display);
    play_gif("STARTUP.GIF");
    play_gif("STARTUP_1.GIF");
    
    main_display->fillScreen(ST77XX_CYAN);
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

void SoundBoard::display_startup_splash()
{
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