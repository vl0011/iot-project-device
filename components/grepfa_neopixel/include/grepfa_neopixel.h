//
// Created by vl0011 on 23. 6. 9.
//

#ifndef P1_GREPFA_NEOPIXEL_H
#define P1_GREPFA_NEOPIXEL_H

void GrepfaNeoPixelSetGPIO(int gpio);
void GrepfaNeoPixelStart();

void GrepfaNeoPixelOn(int r, int g, int b);
void GrepfaNeoPixelOff();
void GrepfaNeoPixelInit();

#endif //P1_GREPFA_NEOPIXEL_H
