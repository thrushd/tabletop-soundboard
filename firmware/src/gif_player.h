#include "Adafruit_GFX.h"
#include "SPI.h"
#include <Adafruit_ST7789.h>
#include <AnimatedGIF.h> // https://github.com/bitbank2/AnimatedGIF
#include <SD.h>

//https://github.com/bitbank2/AnimatedGIF/blob/master/examples/adafruit_gfx_sdcard/adafruit_gfx_sdcard.ino

void init_player(Adafruit_ST7789* st7789_display);
void play_gif(char *gif_file_name);