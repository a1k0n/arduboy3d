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

bool CheckWinding(const Vec2f &s0, const Vec2f &s1, const Vec2f &s2) {
  float x1 = s1.x - s0.x,
        x2 = s2.x - s0.x,
        y1 = s1.y - s0.y,
        y2 = s2.y - s0.y;
  return x1*y2 < x2*y1;
}

class Stars {
  static const int kNumStars = 30;
  uint8_t ypos_[kNumStars];
  uint8_t xpos_[kNumStars];
  uint8_t xspeed_[kNumStars];

 public:
  Stars() {
    for (uint8_t i = 0; i < kNumStars; i++) {
      ypos_[i] = rand() & 63;
      xpos_[i] = rand() & 255;
      xspeed_[i] = 1 + (rand() & 7);
    }
  }

  void Draw() {
    for (uint8_t i = 0; i < kNumStars; i++) {
      uint16_t page = (ypos_[i] >> 3) << 7;
      uint8_t mask = 1 << (ypos_[i] & 7);
      uint8_t x = xpos_[i] >> 1;
      screen_[page + x] |= mask;
      if (xpos_[i] < xspeed_[i]) {
        xpos_[i] = 255;
        ypos_[i] = rand() & 63;
        xspeed_[i] = 1 + (rand() & 7);
      } else {
        xpos_[i] -= xspeed_[i];
      }
    }
  }
};

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

  static float A = 0, A_target = 10;
  static float B = 0, B_target = 10;
  static const float kTurnSpeed = 0.1;
  if (arduboy_.pressed(LEFT_BUTTON)) {
    B_target -= kTurnSpeed;
  }
  if (arduboy_.pressed(RIGHT_BUTTON)) {
    B_target += kTurnSpeed;
  }
  if (arduboy_.pressed(UP_BUTTON)) {
    A_target -= kTurnSpeed;
  }
  if (arduboy_.pressed(DOWN_BUTTON)) {
    A_target += kTurnSpeed;
  }

  A += (A_target - A) * 0.01;
  B += (B_target - B) * 0.01;

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
    Vec2f s0, s1, s2;
    p0.project(2448, 4, &s0);
    // for each flipped axis, flip the "parity" so we generate a consistent
    // clockwise winding order on each face
    if ((f ^ (f>>1) ^ (f>>2)) & 1) {
      p1.project(2448, 4, &s1);
      p2.project(2448, 4, &s2);
    } else {
      p1.project(2448, 4, &s2);
      p2.project(2448, 4, &s1);
    }
    // check winding order in screen space, cull back faces
    if (!CheckWinding(s0, s1, s2)) {
      continue;
    }

    int16_t s0x = s0.x, s0y = s0.y,
            s1x = s1.x, s1y = s1.y,
            s2x = s2.x, s2y = s2.y;
    // hold B to show what it looks like without subpixel rendering
    if (arduboy_.pressed(B_BUTTON)) {
      s0x = (s0x + 8) & ~15;
      s0y = (s0y + 8) & ~15;
      s1x = (s1x + 8) & ~15;
      s1y = (s1y + 8) & ~15;
      s2x = (s2x + 8) & ~15;
      s2y = (s2y + 8) & ~15;
    }

    Vec3f N = p0 + p1 + p2;  // normal vector
    int illum = 4 * N.x + 4 * N.y + 10 * N.z;
    uint8_t pat[4];
    GetDitherPattern(1 + illum, pat);
#if 0
    if (frame_ & 1) {
      // PWM greyscale dithering?
      // there's a better way to do this, but it does sort of work
      uint8_t t = pat[0];
      pat[0] = (pat[1] << 7) | (pat[1] >> 1);
      pat[1] = (pat[2] << 7) | (pat[2] >> 1);
      pat[2] = (pat[3] << 7) | (pat[3] >> 1);
      pat[3] = (t << 7) | (t >> 1);
    }
#endif
    FillTriangle(s0x, s0y, s1x, s1y, s2x, s2y, pat, screen_);
  }
}

Stars stars_;

void loop() {
  if (arduboy_.nextFrame()) {
    stars_.Draw();
    DrawOctahedron();
    arduboy_.display(true);
    ++frame_;
  }
}
