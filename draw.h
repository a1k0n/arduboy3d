#ifndef DRAW_H_
#define DRAW_H_

// draw triangle into screen buffer
// 4 bits of subpixel accuracy, so screen is 128*16 x 64*16 = 2048x1024
// does not detect bad triangles or clip (yet)
extern void FillTriangle(
    uint16_t x0, uint16_t y0,
    uint16_t x1, uint16_t y1,
    uint16_t x2, uint16_t y2,
    uint8_t *pattern, uint8_t *screen);

// get a dither pattern from color 0 to 16 (inclusive)
extern void GetDitherPattern(int8_t color, uint8_t *pattern);

extern void FillVLine(uint8_t y0, uint8_t y1, uint8_t pattern,
    uint8_t *screenptr);

#endif  // DRAW_H_
