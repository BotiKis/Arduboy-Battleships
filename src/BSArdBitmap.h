/*
   Copyright (C) 2016 Ignacio Vina (@igvina)

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

// ArdBitmap: version 2.0.3

#ifndef ARDBITMAP_H
#define ARDBITMAP_H

//Uncomment NO_SPEED_HACK if speed is not important (reduce ~100 bytes)
//#define NO_SPEED_HACK

//Uncomment RESIZE_HACK for fast drawResized with resize >= 1.0
//#define RESIZE_HACK

#include <Arduino.h>

#define ALIGN_H_LEFT    0b00000000
#define ALIGN_H_RIGHT   0b00000001
#define ALIGN_H_CENTER  0b00000010
#define ALIGN_V_TOP     0b00000000
#define ALIGN_V_BOTTOM  0b00000100
#define ALIGN_V_CENTER  0b00001000
#define ALIGN_CENTER    0b00001010
#define ALIGN_NONE      0b00000000

#define MIRROR_NONE       0b00000000
#define MIRROR_HORIZONTAL 0b00000001
#define MIRROR_VERTICAL   0b00000010
#define MIRROR_HOR_VER    0b00000011

static const uint8_t BIT_SHIFT[8] = {
  0b00000001,
  0b00000010,
  0b00000100,
  0b00001000,
  0b00010000,
  0b00100000,
  0b01000000,
  0b10000000,
};

class ArdBitmap
{
  public:
    ArdBitmap(uint8_t *screenBuffer, int16_t width, int16_t height);
    void drawCompressed(int16_t sx, int16_t sy, const uint8_t *compBitmap, uint8_t color, uint8_t align, uint8_t mirror);
    void drawCompressedResized(int16_t sx, int16_t sy, const uint8_t *compBitmap, uint8_t color,uint8_t align, uint8_t mirror, float resize);

    void drawBitmap(int16_t sx, int16_t sy, const uint8_t *bitmap,uint8_t w, uint8_t h, uint8_t color, uint8_t align, uint8_t mirror);
    void drawBitmapResized(int16_t sx, int16_t sy, const uint8_t *bitmap, uint8_t w,uint8_t h, uint8_t color,uint8_t align, uint8_t mirror, float resize);

  private:
    uint8_t *sBuffer;
    int16_t sWidth;
    int16_t sHeight;
};
#endif
