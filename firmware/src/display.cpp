#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "helper_functions.h"
#include "bitmaps.h"
#include "hardware.h"

#define MAIN_SCREEN_WIDTH 128 // OLED display width, in pixels
#define MAIN_SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define MAIN_SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 main_display(MAIN_SCREEN_WIDTH, MAIN_SCREEN_HEIGHT, &Wire, OLED_RESET);

void display_setup() {
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!main_display.begin(SSD1306_SWITCHCAPVCC, MAIN_SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    fatal("failed to set up display", "");
  }
  main_display.clearDisplay();
}

void display_startup_splash() {
  for (int m = 0; m < 10; m++) {
    for (int i = 0; i < bitmap_d20_anim_all_len; i++){
        main_display.clearDisplay();
        main_display.drawBitmap(0, 0, bitmap_d20_anim_all[i], MAIN_SCREEN_WIDTH, MAIN_SCREEN_HEIGHT, 1);
        main_display.display();
        delay(50);
    }
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


int window_pos = 0; // position of the window in the array, defined as the minimum
int cursor_pos = 0; // position of the cursor in the displayed scenes
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