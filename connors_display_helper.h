#ifndef CONNORS_DISPLAY_HELPER_H
#define CONNORS_DISPLAY_HELPER_H

#include <stdint.h>
#include "../inc/ST7735.h"

void CDH_DrawBitmap2x(int16_t x, int16_t y, const uint16_t* image, int16_t w, int16_t h);
void CDH_DrawBitmapTransparent(int16_t x, int16_t y, const uint16_t* image, int16_t w, int16_t h, uint16_t transparentColor = 0xF81F);
uint32_t CDH_DrawString(uint16_t x, uint16_t y, char *pt, int16_t textColor, int bgColor = ST7735_BLACK, int size = 1);
void CDH_OutUDec(uint16_t x, uint16_t y, uint32_t n, int16_t textColor, int bgColor = ST7735_BLACK, int size = 1);

#endif
