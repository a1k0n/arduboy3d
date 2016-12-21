#include <Arduboy2.h>

Arduboy2 arduboy_;
uint8_t *screen_;

void DrawVLine(

void setup() {
  // put your setup code here, to run once:
  arduboy_.begin();
  screen_ = arduboy.getBuffer();
  screen_[0] = 1;
  arduboy_.display();
}

void loop() {
  // put your main code here, to run repeatedly:

}
