#include <Arduboy2.h>
#include "draw.h"

Arduboy2 arduboy_;
uint8_t *screen_;

void setup() {
  // put your setup code here, to run once:
  arduboy_.begin();
  screen_ = arduboy_.getBuffer();

  for (int i = 0; i < 8; i++) {
    FillVLine(i, 7, 0xff, screen_+i);
  }
  for (int i = 0; i < 8; i++) {
    FillVLine(0, i, 0x55 << (i & 1), screen_+i + 8);
  }

  uint8_t pat[] = {0xff, 0xff, 0xff, 0xff};
  FillTriangle(
      16-1, 16*48,
      48*16 + 8, 16*33 - 1,
      127*16, 16*64,
      pat, screen_);
  arduboy_.display(true);
}

void loop() {
  // put your main code here, to run repeatedly:
}
