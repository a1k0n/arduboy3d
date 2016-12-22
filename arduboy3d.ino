#include <Arduboy2.h>
#include "draw.h"
#include "vec.h"

Arduboy2 arduboy_;
uint8_t *screen_;

void setup() {
  // put your setup code here, to run once:
  arduboy_.begin();
  screen_ = arduboy_.getBuffer();
  arduboy_.setFrameRate(60);
}

uint16_t frame_ = 0;

void DrawOctahedron() {
  // vertices:
  //  1,  0,  0
  // -1,  0,  0
  //  0,  1,  0
  //  0, -1,  0
  //  0,  0,  1
  //  0,  0, -1
  
  // faces: (8)
  // 024, 124, 034, 134, 025, 125, 035, 135
  
  // so first, we perform our xyz rotations to find a new xyz coordinate
  // frame (essentially, the resultant rotation matrix R is our coordinate
  // frame)
  // so we need our premultiplied xyz rotation matrix
  // in fact let's just use two rotations, A and B

  float A = 5 * frame_ / 256.0;
  float B = 2 * M_PI * (frame_ & 127) / 128.0;
  float cA = cos(A), sA = sin(A),
        cB = cos(B), sB = sin(B);
  // local coordinate frame given rotation values
  Vec3f Fx(cB, -cA*sB, sA*sB),
        Fy(sB, cA*cB, -sA*cB),
        Fz(0, sA, cA);
  for (uint8_t f = 0; f < 8; f++) {
    Vec3f p0(f&1 ? Fx : -Fx),
          p1(f&2 ? Fy : -Fy),
          p2(f&4 ? Fz : -Fz);
    Vec3f N = p0 + p1 + p2;
    Vec2f s0, s1, s2;
    if (N.z < 0)  // skip back-facing faces
      continue;
    p0.project(1448, 4, &s0);
    p1.project(1448, 4, &s1);
    p2.project(1448, 4, &s2);
    int illum = 4 * N.x + 8 * N.y + 10 * N.z;
    uint8_t pat[4];
    GetDitherPattern(illum, pat);
    FillTriangle(s0.x, s0.y, s1.x, s1.y, s2.x, s2.y, pat, screen_);
  }
}

void loop() {
  if (arduboy_.nextFrame()) {
    DrawOctahedron();
    arduboy_.display(true);
    ++frame_;
  }
}
