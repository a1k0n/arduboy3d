#ifndef SINCOS_H_
#define SINCOS_H_

#ifndef TEST_
#include <avr/pgmspace.h>
#else

#include <stdint.h>
#ifndef PROGMEM
#define PROGMEM
#define pgm_read_byte_near(x) (*(x))
#endif
#endif  // TEST_


// 10-bit precision sine table with 10-bit angles; the lookup table is the
// residual of 1024*sin(2*pi*x/1024) - 4*x
static const uint8_t kSinTbl_[256] PROGMEM = {
	0,   2,   5,   7,   9,  11,  14,  16,  18,  21,  23,  25,  27,
   30,  32,  34,  36,  39,  41,  43,  45,  48,  50,  52,  54,  56,
   59,  61,  63,  65,  67,  70,  72,  74,  76,  78,  80,  82,  85,
   87,  89,  91,  93,  95,  97,  99, 101, 103, 105, 107, 109, 111,
  113, 115, 117, 119, 121, 123, 125, 127, 129, 130, 132, 134, 136,
  138, 139, 141, 143, 145, 146, 148, 150, 151, 153, 155, 156, 158,
  160, 161, 163, 164, 166, 167, 169, 170, 172, 173, 174, 176, 177,
  179, 180, 181, 182, 184, 185, 186, 187, 188, 190, 191, 192, 193,
  194, 195, 196, 197, 198, 199, 200, 201, 202, 202, 203, 204, 205,
  206, 206, 207, 208, 208, 209, 210, 210, 211, 211, 212, 212, 213,
  213, 213, 214, 214, 214, 215, 215, 215, 215, 215, 215, 215, 216,
  216, 216, 216, 215, 215, 215, 215, 215, 215, 214, 214, 214, 214,
  213, 213, 212, 212, 211, 211, 210, 210, 209, 208, 208, 207, 206,
  206, 205, 204, 203, 202, 201, 200, 199, 198, 197, 196, 195, 193,
  192, 191, 190, 188, 187, 186, 184, 183, 181, 180, 178, 176, 175,
  173, 171, 170, 168, 166, 164, 162, 160, 158, 156, 154, 152, 150,
  148, 146, 143, 141, 139, 137, 134, 132, 129, 127, 124, 122, 119,
  116, 114, 111, 108, 106, 103, 100,  97,  94,  91,  88,  85,  82,
   79,  76,  72,  69,  66,  62,  59,  56,  52,  49,  45,  42,  38,
   34,  31,  27,  23,  20,  16,  12,   8,   4};

static uint16_t LookupSin(uint16_t theta) {
  uint8_t angle_in_quadrant = theta & 255;
  // special case for angle 0
  if (angle_in_quadrant == 0) {
    // { 0,  1,  0, -1}[theta >> 6]
    //  00  01  10  11   <- theta & 0xc0
    if (theta & 0x100) {
      return theta & 0x200 ? -1024 : 1024;
    } else {
      return 0;
    }
  }

  // + - + -  <- table direction
  // + + - -  <- sign
  if (theta & 0x100) {
    angle_in_quadrant = 256 - angle_in_quadrant;
  }
  uint8_t value = pgm_read_byte_near(kSinTbl_ + angle_in_quadrant);
  if (theta & 0x200) {
    return -4*angle_in_quadrant - value;
  } else {
    return 4*angle_in_quadrant + value;
  }
}

static void GetSinCos(uint16_t theta,
    int16_t *sin, int16_t *cos) {
  *sin = LookupSin(theta);
  *cos = LookupSin(theta + 0x100);
}

#endif  // SINCOS_H_
