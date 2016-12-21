#include <Arduboy2.h>
#include "draw.h"

Arduboy2 arduboy_;
uint8_t *screen_;

void setup() {
  // put your setup code here, to run once:
  arduboy_.begin();
  screen_ = arduboy_.getBuffer();
  screen_[0] = 1;
/*
  uint8_t pat[] = {0xff, 0xff, 0xff, 0xff};
  FillTriangle(
      15, 16*48,
      48*16 + 8, 16*33 - 1,
      127*16, 16*64,
      pat, screen_);

  uint8_t pat2[] = {0x11, 0x00, 0x44, 0x00};
  FillTriangle(
      15, 16*48,
      127*16, 16*64,
      48*16 + 8, 16*63,
      pat2, screen_);
*/
  for (int i = 0; i < 8; i++) {
    FillVLine(i, 7, 0xff, screen_+i);
  }
  for (int i = 0; i < 8; i++) {
    FillVLine(0, i, 0xff, screen_+i + 8);
  }
  arduboy_.display(true);
}

void loop() {
  // put your main code here, to run repeatedly:
}
