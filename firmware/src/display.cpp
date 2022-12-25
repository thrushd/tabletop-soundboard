#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "helper_functions.h"
#include "bitmaps.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels


#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void display_setup(){
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    fatal("failed to set up display", "");
  }
  display.clearDisplay();
}

void display_startup_splash(){
  while(1){
    for (int i = 0; i < bitmap_d20_anim_all_len; i++){
        display.clearDisplay();
        display.drawBitmap(0, 0, bitmap_d20_anim_all[i], SCREEN_WIDTH, SCREEN_HEIGHT, 1);
        display.display();
        delay(50);
    }
  }
}