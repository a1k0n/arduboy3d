#ifndef SINCOS_H_
#define SINCOS_H_

#include <avr/pgmspace.h>

// FIXME: minimize this to 64 uint8_t entries for space optimization
// i don't want to mess with this right now
static const uint8_t kSinTbl_[64] PROGMEM = {
  0, 6, 12, 18, 25, 31, 37, 43, 49, 56, 62, 68, 74,
  80, 86, 92, 97, 103, 109, 115, 120, 126, 131, 136, 142, 147,
  152, 157, 162, 167, 171, 176, 181, 185, 189, 193, 197, 201, 205,
  209, 212, 216, 219, 222, 225, 228, 231, 234, 236, 238, 241, 243,
  244, 246, 248, 249, 251, 252, 253, 254, 254, 255, 255, 255
};

static uint16_t LookupSin(uint8_t theta) {
  uint8_t angle_in_quadrant = theta & 63;
  // special case for angle 0
  if (angle_in_quadrant == 0) {
    // { 0,  1,  0, -1}[theta >> 6]
    //  00  01  10  11   <- theta & 0xc0
    if (theta & 0x40) {
      return theta & 0x80 ? -256 : 256;
    } else {
      return 0;
    }
  }

  // + - + -  <- table direction
  // + + - -  <- sign
  if (theta & 0x40) {
    angle_in_quadrant = 64 - angle_in_quadrant;
  }
  uint8_t value = pgm_read_byte_near(kSinTbl_ + angle_in_quadrant);
  if (theta & 0x80) {
    return -(int16_t) value;
  } else {
    return (int16_t) value;
  }
}

static void GetSinCos(uint8_t theta,
    int16_t *sin, int16_t *cos) {
  *sin = LookupSin(theta);
  *cos = LookupSin(theta + 0x40);
}

#endif  // SINCOS_H_
