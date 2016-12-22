#include <Arduboy2.h>
#include "draw.h"

Arduboy2 arduboy_;
uint8_t *screen_;

void setup() {
  // put your setup code here, to run once:
  arduboy_.begin();
  screen_ = arduboy_.getBuffer();
}

uint16_t frame = 0;
void loop() {
  // put your main code here, to run repeatedly:

  float y1 = 16*32 + 16*30.0 * sin(0.01*frame);
  
  uint8_t pat[4];
  GetDitherPattern((int) (9 + 8*sin(0.002*frame)), pat);
  FillTriangle(
      16-1, 16*48,
      48*16 + 8, (int16_t) y1,
      127*16, 16*64,
      pat, screen_);
  arduboy_.display(true);

  ++frame;
}
