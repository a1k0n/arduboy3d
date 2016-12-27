#include <stdint.h>
#include <string.h>

#ifndef TEST_
#include <avr/pgmspace.h>
#else

#include <stdio.h>
#ifndef PROGMEM
#define PROGMEM
#define pgm_read_byte_near(x) (*(x))
#endif
#endif  // TEST_

// since the AVR has no barrel shifter, we'll do a progmem lookup
const uint8_t topmask_[] PROGMEM = {
  0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80};
const uint8_t bottommask_[] PROGMEM = {
  0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff};

// 16-level 4x4 dither matrix, colors 0..16 (black, white, and 15 between)
const uint8_t dither_[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x11, 0x00, 0x44, 0x00,
  0x55, 0x00, 0x44, 0x00, 0x55, 0x00, 0x55, 0x00, 0x55, 0x22, 0x55, 0x00,
  0x55, 0x22, 0x55, 0x88, 0x55, 0xaa, 0x55, 0x88, 0x55, 0xaa, 0x55, 0xaa,
  0x77, 0xaa, 0x55, 0xaa, 0x77, 0xaa, 0xdd, 0xaa, 0xff, 0xaa, 0xdd, 0xaa,
  0xff, 0xaa, 0xff, 0xaa, 0xff, 0xbb, 0xff, 0xaa, 0xff, 0xbb, 0xff, 0xee,
  0xff, 0xff, 0xff, 0xee, 0xff, 0xff, 0xff, 0xff
};

// fill a vertical line from y0 to y1, inclusive, with bitmask pattern
// does not tolerate y1 < y0
void FillVLine(int8_t y0_, int8_t y1_, uint8_t pattern, uint8_t *screenptr) {
  if (y1_ < y0_ || y1_ < 0 || y0_ > 63) return;

  // clip (FIXME; clipping should be handled elsewhere)
  // cast to unsigned after clipping to simplify generated code below
  uint8_t y0 = y0_, y1 = y1_;
  if (y0_ < 0) y0 = 0;
  if (y1_ > 63) y1 = 63;

  uint8_t *page0 = screenptr + ((y0 & 0x38) << 4);
  uint8_t *page1 = screenptr + ((y1 & 0x38) << 4);
  if (page0 == page1) {
    uint8_t mask = pgm_read_byte_near(topmask_ + (y0&7))
      & pgm_read_byte_near(bottommask_ + (y1&7));
    *page0 &= ~mask;
    *page0 |= pattern & mask;
  } else {
    uint8_t mask = pgm_read_byte_near(topmask_ + (y0&7));
    *page0 &= ~mask;
    *page0 |= pattern & mask;
    page0 += 128;
    while (page0 != page1) {
      *page0 = pattern;
      page0 += 128;
    }
    mask = pgm_read_byte_near(bottommask_ + (y1&7));
    *page0 &= ~mask;
    *page0 |= pattern & mask;
  }
}

// get a dither pattern from color 0 to 16 (inclusive)
void GetDitherPattern(int8_t color, uint8_t *pat) {
  if (color <= 0) {
    pat[0] = pat[1] = pat[2] = pat[3] = 0;
    return;
  }
  if (color >= 16) {
    pat[0] = pat[1] = pat[2] = pat[3] = 0xff;
    return;
  }
  uint8_t offset = color * 4;
  // unroll to pgm_read_word?
  pat[0] = pgm_read_byte_near(dither_ + offset);
  pat[1] = pgm_read_byte_near(dither_ + offset + 1);
  pat[2] = pgm_read_byte_near(dither_ + offset + 2);
  pat[3] = pgm_read_byte_near(dither_ + offset + 3);
}

// get a two-frame dither pattern, colors 0 to 32 (inclusive)
void GetDitherPattern(int8_t color, uint8_t frame_offset, uint8_t *pat) {
  if (color <= 0) {
    pat[0] = pat[1] = pat[2] = pat[3] = 0;
    return;
  }
  if (color >= 32) {
    pat[0] = pat[1] = pat[2] = pat[3] = 0xff;
    return;
  }
  uint8_t offset = (color & 15) * 4;
  // unroll to pgm_read_word?
  pat[0] = pgm_read_byte_near(dither_ + offset);
  pat[1] = pgm_read_byte_near(dither_ + offset + 1);
  pat[2] = pgm_read_byte_near(dither_ + offset + 2);
  pat[3] = pgm_read_byte_near(dither_ + offset + 3);

  // 50% grayscale PWM
  if (color < 16) {
    if (frame_offset & 1) {
      pat[0] &= 0x33;
      pat[1] &= 0xcc;
      pat[2] &= 0x33;
      pat[3] &= 0xcc;
    } else {
      pat[0] &= 0xcc;
      pat[1] &= 0x33;
      pat[2] &= 0xcc;
      pat[3] &= 0x33;
    }
  } else {
    if (frame_offset & 1) {
      pat[0] |= 0x33;
      pat[1] |= 0xcc;
      pat[2] |= 0x33;
      pat[3] |= 0xcc;
    } else {
      pat[0] |= 0xcc;
      pat[1] |= 0x33;
      pat[2] |= 0xcc;
      pat[3] |= 0x33;
    }
  }
}

// draw triangle into screen buffer
// 4 bits of subpixel accuracy, so screen is 128*16 x 64*16 = 2048x1024
// does efficient left-right clipping, and unoptimized top-bottom clipping
void FillTriangle(
    int16_t x0, int16_t y0,
    int16_t x1, int16_t y1,
    int16_t x2, int16_t y2,
    uint8_t *pattern, uint8_t *screen) {
  // sort coordinates by x w/ optimal 3-sorting network
  {
    int16_t t;
    if (x0 > x1) {
      t = x1; x1 = x0; x0 = t;
      t = y1; y1 = y0; y0 = t;
    }
    if (x1 > x2) {
      t = x2; x2 = x1; x1 = t;
      t = y2; y2 = y1; y1 = t;
    }
    if (x0 > x1) {
      t = x1; x1 = x0; x0 = t;
      t = y1; y1 = y0; y0 = t;
    }
  }
  if (x2 <= 0 || x0 > 127*16) {  // entire triangle is off screen
    return;
  }

  // we want to fill in pixels which are *inside* the triangle
  // first we need to bump from x0,y0 to the next whole x (as x0 is
  // fractional)

  // we can use the standard ddx algorithm, with a pre-divided whole part
  // (normally assumed to be 0 in bresenham's)

  // first trapezoid: x0 to x1
  int16_t dx01 = x1 - x0;
  int16_t dx02 = x2 - x0;

  // the top and bottom variable names are a misnomer, as they can also
  // be inverted. the "upside down" case is handled by checking in the
  // inner loop; ideally that would be a separate branch here.
  int8_t yt = y0 >> 4, yb = y0 >> 4;  // "top" and "bottom" y coords
  int16_t ytf = y0 & 15, ybf = y0 & 15;  // fractional part of each

  // now we need to advance to the next whole pixel ((x0 + 15) & ~15)
  // update yt, ytf along the slope of (dy01/dx01) for x0 - ((x0 + 15) & ~15) steps
  // dx' = ((x0 + 15) & ~15) - x0
  // y' = y0 + dx' * (y1-y0) / (x1-x0)
  // yt/ytf are stored in fractions of (x1-x0)
  // so
  {
    int32_t dx0;
    if (x0 >= 0) {
      // round up to the next 16
      dx0 = (16 - x0) & 15;
    } else {
      // if x0 is off the left edge of the screen, advance all the way to the
      // left edge of the screen
      dx0 = -x0;
    }
    int32_t dyt = (dx0 * (y1 - y0)) >> 4;
    yt += dyt / dx01;
    ytf += dyt % dx01;

    int32_t dyb = (dx0 * (y2 - y0)) >> 4;
    yb += dyb / dx02;
    ybf += dyb % dx02;

    x0 += dx0;
  }

  // x0 is now aligned to a whole number of pixels,
  // and yt/yb/ytf/ybf are initialized
  int16_t dy01 = (y1 - y0) / dx01;  // FIXME: does this generate combined divmod?
  int16_t fy01 = (y1 - y0) % dx01;
  int16_t dy02 = (y2 - y0) / dx02;
  int16_t fy02 = (y2 - y0) % dx02;
  int8_t x = x0 >> 4;
  uint8_t pattern_offset = x & 3;
  screen += x;
  // so technically we are downsampling both dy and dx here by a factor of 16,
  // but that's a wash, so we can still use the existing fractional slope and
  // step along exactly (x1-x0) / 16 pixels and we should be just short of x1
  if (x1 > 128*16) {
    // safe to modify x1 now as all slopes have been computed
    x1 = 128*16;
  }
  while (x0 < x1) {
    // now, we include the bottom pixel if ybf != 0, otherwise we don't
    if (yt < yb) {
      FillVLine(yt, yb - (ybf == 0 ? 1 : 0), pattern[pattern_offset], screen);
    } else {
      FillVLine(yb, yt - (ytf == 0 ? 1 : 0), pattern[pattern_offset], screen);
    }
    yt += dy01;
    ytf += fy01;
    if (ytf < 0) { yt--; ytf += dx01; }
    if (ytf >= dx01) { yt++; ytf -= dx01; }

    yb += dy02;
    ybf += fy02;
    if (ybf < 0) { yb--; ybf += dx02; }
    if (ybf >= dx02) { yb++; ybf -= dx02; }
    ++screen;
    ++pattern_offset;
    pattern_offset &= 3;
    x0 += 16;
  }
  if (x0 >= 128*16) {
    return;  // off right edge of screen
  }
  // now x0 >= x1, we may have gone slightly too far.
  yt = y1 >> 4;  // new top y
  ytf = y1 & 15;  // .. and fractional part
  int16_t dx12 = x2 - x1;
  int16_t dy12 = (y2 - y1) / dx12;
  int16_t fy12 = (y2 - y1) % dx12;
  // we need to adjust yt, ytf for the new slope of (y2-y1)/(x2-x1)
  {
    // if x1 was also off the left edge of the screen, then we also
    // automatically handle that here since x0 is assumed to be past x1.
    int32_t dx0 = x0 - x1;
    int32_t dyt = (dx0 * (y2 - y1)) >> 4;
    yt += dyt / dx12;
    ytf += dyt % dx12;
  }
  // draw 2nd trapezoid
  if (x2 > 128*16) {  // clip to right edge
    x2 = 128*16;
  }
  while (x0 < x2) {
    // now, we include the bottom pixel if ybf != 0, otherwise we don't
    if (yt < yb) {
      FillVLine(yt, yb - (ybf == 0 ? 1 : 0), pattern[pattern_offset], screen);
    } else {
      FillVLine(yb, yt - (ytf == 0 ? 1 : 0), pattern[pattern_offset], screen);
    }
    yt += dy12;
    ytf += fy12;
    if (ytf < 0) { yt--; ytf += dx12; }
    if (ytf >= dx12) { yt++; ytf -= dx12; }

    yb += dy02;
    ybf += fy02;
    if (ybf < 0) { yb--; ybf += dx02; }
    if (ybf >= dx02) { yb++; ybf -= dx02; }
    ++screen;
    ++pattern_offset;
    pattern_offset &= 3;
    x0 += 16;
  }
}

#ifdef TEST_
void PrintScreen(uint8_t *screen) {
  for (int page = 0; page < 8; page++) {
    uint8_t mask = 0x01;
    for (int y = 0; y < 8; y++, mask <<= 1) {
      for (int x = 0; x < 128; x++) {
        putchar(screen[page*128 + x] & mask ? '#' : ' ');
      }
      puts("");
    }
  }
}

int main() {
  uint8_t screen[128*8];
  uint8_t pat[] = {0xff, 0xff, 0xff, 0xff};
#if 0
  for (int i = 0; i < 8; i++) {
    FillVLine(i, 7, 0xff, screen+i);
  }
  for (int i = 0; i < 8; i++) {
    FillVLine(0, i, 0xff, screen+i + 8);
  }
  for (int i = 18; i < 26; i++) {
    screen[i] = 0xff;
    screen[128+i] = 0xff;
    screen[128*2+i] = 0xff;
    screen[128*3+i] = 0xff;
    screen[128*4+i] = 0xff;
  }
  for (int i = 0; i < 16; i++) {
    FillVLine(i, 37-i, pgm_read_byte_near(dither_ + 8*4 + i),
        screen+i + 18);
  }

  FillTriangle(
      15, 16*48,
      48*16 + 8, 16*33 - 1,
      127*16, 16*64,
      pat, screen);

  PrintScreen(screen);

  uint8_t pat2[] = {0x11, 0x00, 0x44, 0x00};
  FillTriangle(
      15, 16*48,
      127*16, 16*64,
      48*16 + 8, 16*63,
      pat2, screen);
#endif
  for (int16_t x = 1024; x > -1024; --x) {
    memset(screen, 0, sizeof(screen));
    FillTriangle(
        -1024 + x, 32*16,
        x, -512,
        1024 + x, 1023,
        pat, screen);
    PrintScreen(screen);
  }
}

#endif  // TEST_
