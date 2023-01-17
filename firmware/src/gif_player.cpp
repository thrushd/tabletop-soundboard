#include "gif_player.h"

/*
Convert a blender mp4 into a gif for startup with ffmpeg
ffmpeg -i 0001-0030.mp4 -vf "split[s0][s1];[s0]palettegen[p];[s1][p]paletteuse"-loop 0 output.gif
*/

Adafruit_ST7789* display;
int16_t width;
int16_t height;

AnimatedGIF gif;
File f;

void init_player(Adafruit_ST7789* st7789_display){
    display = st7789_display;
    width = display->width();
    height = display->height();
    gif.begin(LITTLE_ENDIAN_PIXELS);
}

void* GIFOpenFile(const char* fname, int32_t* pSize)
{
    f = SD.open(fname);
    if (f) {
        *pSize = f.size();
        return (void*)&f;
    }
    // Serial.println("Opened file");
    return NULL;
} /* GIFOpenFile() */

void GIFCloseFile(void* pHandle)
{
    File* f = static_cast<File*>(pHandle);
    if (f != NULL)
        f->close();
    // Serial.println("Closed file");
} /* GIFCloseFile() */

int32_t GIFReadFile(GIFFILE* pFile, uint8_t* pBuf, int32_t iLen)
{
    int32_t iBytesRead;
    iBytesRead = iLen;
    File* f = static_cast<File*>(pFile->fHandle);
    // Note: If you read a file all the way to the last byte, seek() stops working
    if ((pFile->iSize - pFile->iPos) < iLen)
        iBytesRead = pFile->iSize - pFile->iPos - 1; // <-- ugly work-around
    if (iBytesRead <= 0)
        return 0;
    iBytesRead = (int32_t)f->read(pBuf, iBytesRead);
    pFile->iPos = f->position();
    return iBytesRead;
} /* GIFReadFile() */

int32_t GIFSeekFile(GIFFILE* pFile, int32_t iPosition)
{
    int i = micros();
    File* f = static_cast<File*>(pFile->fHandle);
    f->seek(iPosition);
    pFile->iPos = (int32_t)f->position();
    i = micros() - i;
    // Serial.printf("Seek time = %d us\n", i);
    return pFile->iPos;
} /* GIFSeekFile() */

void GIFDraw(GIFDRAW* pDraw)
{
    uint8_t* s;
    uint16_t *d, *usPalette, usTemp[320];
    int x, y, iWidth;

    iWidth = pDraw->iWidth;
    if (iWidth + pDraw->iX > width)
        iWidth = width - pDraw->iX;
    usPalette = pDraw->pPalette;
    y = pDraw->iY + pDraw->y; // current line

    if (y >= height || pDraw->iX >= width || iWidth < 1)
        // Serial.println("Returning from GIFDRAW");
        return;
    s = pDraw->pPixels;
    if (pDraw->ucDisposalMethod == 2) // restore to background color
    {
        for (x = 0; x < iWidth; x++) {
            if (s[x] == pDraw->ucTransparent)
                s[x] = pDraw->ucBackground;
        }
        pDraw->ucHasTransparency = 0;
    }

    // Apply the new pixels to the main image
    if (pDraw->ucHasTransparency) // if transparency used
    {
        uint8_t *pEnd, c, ucTransparent = pDraw->ucTransparent;
        int x, iCount;
        pEnd = s + iWidth;
        x = 0;
        iCount = 0; // count non-transparent pixels
        while (x < iWidth) {
            c = ucTransparent - 1;
            d = usTemp;
            while (c != ucTransparent && s < pEnd) {
                c = *s++;
                if (c == ucTransparent) // done, stop
                {
                    s--; // back up to treat it like transparent
                } else // opaque
                {
                    *d++ = usPalette[c];
                    iCount++;
                }
            } // while looking for opaque pixels
            if (iCount) // any opaque pixels?
            {
                display->startWrite();
                display->setAddrWindow(pDraw->iX + x, y, iCount, 1);
                display->writePixels(usTemp, iCount, false, false);
                display->endWrite();
                x += iCount;
                iCount = 0;
            }
            // no, look for a run of transparent pixels
            c = ucTransparent;
            while (c == ucTransparent && s < pEnd) {
                c = *s++;
                if (c == ucTransparent)
                    iCount++;
                else
                    s--;
            }
            if (iCount) {
                x += iCount; // skip these
                iCount = 0;
            }
        }
    } else {
        s = pDraw->pPixels;
        // Translate the 8-bit pixels through the RGB565 palette (already byte reversed)
        for (x = 0; x < iWidth; x++)
            usTemp[x] = usPalette[*s++];
        display->startWrite();
        display->setAddrWindow(pDraw->iX, y, iWidth, 1);
        display->writePixels(usTemp, iWidth, false, false);
        display->endWrite();
    }
} /* GIFDraw() */

void play_gif(String gif_file_name)
{
    Serial.printf("Opening %s, ", gif_file_name.c_str());
    if (gif.open(gif_file_name.c_str(), GIFOpenFile, GIFCloseFile, GIFReadFile, GIFSeekFile, GIFDraw)) {
        Serial.printf("success! Canvas size = %d x %d\n", gif.getCanvasWidth(), gif.getCanvasHeight());
        // fun info
        // GIFINFO gi;
        // if (gif.getInfo(&gi)) {
        //     Serial.printf("frame count: %d\n", gi.iFrameCount);
        //     Serial.printf("duration: %d ms\n", gi.iDuration);
        //     Serial.printf("max delay: %d ms\n", gi.iMaxDelay);
        //     Serial.printf("min delay: %d ms\n", gi.iMinDelay);
        //     Serial.println();
        // }
        while (gif.playFrame(true, NULL)) {
        }
        gif.close();
    } else {
        Serial.printf("failed to open file = %d\n", gif.getLastError());
    }
}