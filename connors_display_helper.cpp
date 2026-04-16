#include "connors_display_helper.h"
#include <stdio.h>
#include <stdint.h>


void CDH_DrawBitmap2x(int16_t x, int16_t y, const uint16_t* image, int16_t w, int16_t h) {
    if ((image == nullptr) || (w <= 0) || (h <= 0)) {
        return;
    }

    int16_t topY = y - (2 * h) + 1;

    for (int16_t row = 0; row < h; row++) {
        int16_t srcRow = (h - 1) - row;
        for (int16_t col = 0; col < w; col++) {
            uint16_t color = image[(srcRow * w) + col];
            int16_t px = x + (2 * col);
            int16_t py = topY + (2 * row);

            ST7735_DrawPixel(px, py, color);
            ST7735_DrawPixel(px + 1, py, color);
            ST7735_DrawPixel(px, py + 1, color);
            ST7735_DrawPixel(px + 1, py + 1, color);
        }
    }
}

void CDH_DrawBitmapTransparent(int16_t x, int16_t y, const uint16_t* image, int16_t w, int16_t h, uint16_t transparentColor) {
    if ((image == nullptr) || (w <= 0) || (h <= 0)) {
        return;
    }

    int16_t topY = y - h + 1;

    for (int16_t row = 0; row < h; row++) {
        int16_t srcRow = (h - 1) - row; // matches ST7735_DrawBitmap row convention
        for (int16_t col = 0; col < w; col++) {
            uint16_t color = image[(srcRow * w) + col];
            if (color != transparentColor) {
                ST7735_DrawPixel(x + col, topY + row, color);
            }
        }
    }
}

//------------CDH_DrawString------------
// String draw function.
// 16 rows (0 to 15) and 21 characters (0 to 20)
// Requires (11 + size*size*6*8) bytes of transmission for each character
// Input: x         columns from the left edge (0 to 20)
//        y         rows from the top edge (0 to 15)
//        pt        pointer to a null terminated string to be printed
//        textColor 16-bit color of the characters
// bgColor is Black and size is 1
// Output: number of characters printed
uint32_t CDH_DrawString(uint16_t x, uint16_t y, char *pt, int16_t textColor, int bgColor, int size){
  uint32_t count = 0;
  if(y>15) return 0;
  while(*pt){
    ST7735_DrawCharS(x*6, y*10, *pt, textColor, bgColor, size);
    pt++;
    x = x+1;
    if(x>20) return count;  // number of characters printed
    count++;
  }
  return count;  // number of characters printed
}
