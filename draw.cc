#include <stdint.h>
#include <stdio.h>
#include <string.h>

// since the AVR has no barrel shifter, we'll do a progmem lookup
const uint8_t topmask_[] = {0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80};
const uint8_t bottommask_[] = {0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff};

// 16-level 4x4 dither matrix, colors 0..16
const uint8_t dither_[] = {0, 0, 0, 0, 17, 0, 0, 0, 17, 0, 68, 0, 85, 0, 68, 0,
  85, 0, 85, 0, 85, 34, 85, 0, 85, 34, 85, 136, 85, 170, 85, 136, 85, 170, 85,
  170, 119, 170, 85, 170, 119, 170, 221, 170, 255, 170, 221, 170, 255, 170,
  255, 170, 255, 187, 255, 170, 255, 187, 255, 238, 255, 255, 255, 238, 255,
  255, 255, 255};

// fill a vertical line from y0 to y1, inclusive, with bitmask pattern
// does not tolerate y1 < y0
void FillVLine(uint8_t y0, uint8_t y1, uint8_t pattern, uint8_t *screenptr) {
  if (y1 < y0) return;
  uint8_t *page0 = screenptr + ((y0 >> 3) << 7);  // check: does avr-gcc optimize this?
  uint8_t *page1 = screenptr + ((y1 >> 3) << 7);
  if (page0 == page1) {
    uint8_t mask = topmask_[y0&7] & bottommask_[y1&7];
    *page0 &= ~mask;
    *page0 |= pattern & mask;
  } else {
    uint8_t mask = topmask_[y0&7];
    *page0 &= ~mask;
    *page0 |= pattern & mask;
    page0 += 128;
    while (page0 != page1) {
      *page0 = pattern;
      page0 += 128;
    }
    mask = bottommask_[y1&7];
    *page0 &= ~mask;
    *page0 |= pattern & mask;
  }
}

// draw triangle into screen buffer
// 4 bits of subpixel accuracy, so screen is 128*16 x 64*16 = 2048x1024
// assumes clockwise winding order, does not detect bad triangles or clip (yet)
void FillTriangle(
    uint16_t x0, uint16_t y0,
    uint16_t x1, uint16_t y1,
    uint16_t x2, uint16_t y2,
    uint8_t *pattern, uint8_t *screen) {
  // basic assumption: y1 < y0 + (y2-y0)*(x1-x0)/(x2-x0)
  // maybe we can relax that assumption
  // so we want to fill in pixels which are *inside* the triangle
  // first we need to bump from x0,y0 to the next whole x (as x0 is fractional)

  // we can use the standard ddx algorithm, with a pre-divided whole part
  // (normally assumed to be 0 in bresenham's)

  // FIXME: optimize tiny triangle cases, as they are common

  // first trapezoid: x0 to x1, the p0-p1 line above the p0-p2 line (clockwise)
  uint16_t dx01 = x1 - x0;
  uint16_t dx02 = x2 - x0;

  uint16_t yt = y0 >> 4, yb = y0 >> 4;  // top and bottom y
  int16_t ytf = y0 & 15, ybf = y0 & 15;

  printf("init x0: %d yt: %d %d yb: %d %d\n", x0, yt, ytf, yb, ybf);

  // now we need to advance to the next whole pixel ((x0 + 15) & ~15)
  // update yt, ytf along the slope of (dy01/dx01) for x0 - ((x0 + 15) & ~15) steps
  // dx' = ((x0 + 15) & ~15) - x0
  // y' = y0 + dx' * (y1-y0) / (x1-x0)
  // yt/ytf are stored in fractions of (x1-x0)
  // so
  {
    uint16_t dx0 = ((x0 + 15) & ~15) - x0;
    int16_t dyt = dx0 * (y1 - y0) >> 4;
    yt += dyt / dx01;
    ytf += dyt % dx01;

    int16_t dyb = dx0 * (y2 - y0) >> 4;
    yb += dyb / dx02;
    ybf += dyb % dx02;
    x0 += dx0;
  }

  printf("bumped x0: %d yt: %d %d yb: %d %d\n", x0, yt, ytf, yb, ybf);
  // x0 is now aligned to a whole number of pixels, and yt/yb/ytf/ybf are initialized
  int16_t dy01 = (y1 - y0) / dx01;  // FIXME: does this generate combined divmod?
  int16_t fy01 = (y1 - y0) % dx01;
  int16_t dy02 = (y2 - y0) / dx02;
  int16_t fy02 = (y2 - y0) % dx02;
  uint8_t x = x0 >> 4;
  uint8_t pattern_offset = x & 3;
  screen += x;
  // so technically we are downsampling both dy and dx here by a factor of 16,
  // but that's a wash, so we can still use the existing fractional slope and
  // step along exactly (x1-x0) / 16 pixels and we should be just short of x1
  while (x0 < x1) {
    // now, we include the bottom pixel if ybf != 0, otherwise we don't
    FillVLine(yt, yb - (ybf == 0 ? 1 : 0), pattern[pattern_offset], screen);
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
  // now x0 > x1, we went slightly too far.
  yt = y1 >> 4;  // new top y
  ytf = y1 & 15;  // .. and fractional part
  uint16_t dx12 = x2 - x1;
  int16_t dy12 = (y2 - y1) / dx12;
  int16_t fy12 = (y2 - y1) % dx12;
  // we need to adjust yt, ytf for the new slope of (y2-y1)/(x2-x1)
  {
    uint16_t dx0 = x0 - x1;
    int16_t dyt = dx0 * (y2 - y1) >> 4;
    yt += dyt / dx12;
    ytf += dyt % dx12;
  }
  printf("bumped x0 past x1: %d yt: %d %d %d %d yb: %d %d\n", x0, yt, ytf, dy12, fy12, yb, ybf);
  // draw 2nd trapezoid
  while (x0 < x2) {
    // now, we include the bottom pixel if ybf != 0, otherwise we don't
    FillVLine(yt, yb - (ybf == 0 ? 1 : 0), pattern[pattern_offset], screen);
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
  memset(screen, 0, sizeof(screen));
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
    FillVLine(i, 37-i, dither_[8*4 + i], screen+i + 18);
  }

  uint8_t pat[] = {0xff, 0xff, 0xff, 0xff};
  FillTriangle(
      15, 16*48,
      48*16 + 8, 16*33 - 1,
      127*16, 16*64,
      pat, screen);

  PrintScreen(screen);
}

