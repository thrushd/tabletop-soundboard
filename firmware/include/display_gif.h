#include "Adafruit_GFX.h"
#include "SPI.h"
#include <Adafruit_ST7789.h>
#include <AnimatedGIF.h> // https://github.com/bitbank2/AnimatedGIF
#include <SD.h>

//https://github.com/bitbank2/AnimatedGIF/blob/master/examples/adafruit_gfx_sdcard/adafruit_gfx_sdcard.ino

#define GIF_DISPLAY_WIDTH 240
#define GIF_DISPLAY_HEIGHT 240

// a gif player for the adafruit ST7788
class GifPlayer {
public:
    GifPlayer();
    GifPlayer(String gif_file_name, Adafruit_ST7789* st7789_display);
    void play();
    void* GIFOpenFile(const char* fname, int32_t* pSize);
    void GIFCloseFile(void* pHandle);
    int32_t GIFReadFile(GIFFILE* pFile, uint8_t* pBuf, int32_t iLen);
    int32_t GIFSeekFile(GIFFILE* pFile, int32_t iPosition); 
    void GIFDraw(GIFDRAW* pDraw);
    void init();

private:
    String gif_file_name;
    Adafruit_ST7789* display;
    // int16_t width;
    // int16_t height;

    AnimatedGIF gif;
    File f;
};

void* gif_open_file_callback(const char* fname, int32_t* pSize);
void gif_close_file_callback(void* pHandle);
int32_t gif_read_file_callback(GIFFILE* pFile, uint8_t* pBuf, int32_t iLen);
int32_t gif_seek_file_callback(GIFFILE* pFile, int32_t iPosition);
void gif_draw_callback(GIFDRAW* pDraw);