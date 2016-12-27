#include <Arduboy2.h>
#include <avr/pgmspace.h>
#include "draw.h"
#include "sincos.h"
#include "vec.h"
#include "icosahedron.h"

Arduboy2 arduboy_;
uint8_t *screen_;

void setup() {
  // put your setup code here, to run once:
  arduboy_.begin();
  screen_ = arduboy_.getBuffer();
  arduboy_.setFrameRate(60);
}

uint16_t frame_ = 0;

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

int32_t angle_A_ = 0, angle_B_ = 0;
int16_t distance_ = 1024;
int16_t scale_ = 4096;

void ReadInput() {
  static int32_t A_target = 100000, B_target = 50000;
  static const int32_t kTurnSpeed = 1000;

  if (arduboy_.pressed(A_BUTTON)) {
    if (arduboy_.pressed(LEFT_BUTTON)) {
      scale_ -= 10;
    }
    if (arduboy_.pressed(RIGHT_BUTTON)) {
      scale_ += 10;
    }
    if (arduboy_.pressed(UP_BUTTON)) {
      distance_ += 3;
    }
    if (arduboy_.pressed(DOWN_BUTTON)) {
      distance_ -= 3;
    }
  } else {
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
  }

  angle_A_ += (A_target - angle_A_) >> 7;
  angle_B_ += (B_target - angle_B_) >> 7;
}

void DrawObject() {
  static const uint8_t NVERTS = sizeof(icosahedron_vertices) / 3;
  static const uint8_t NFACES = sizeof(icosahedron_faces) / 3;
  Vec216 verts[NVERTS];  // rotated, projected screen space vertices

#if 0
  {
    uint8_t i = 0;
    do {
      int16_t s, c;
      GetSinCos(i, &s, &c);
      uint8_t x = 64 + (c >> 3);
      uint8_t y = 32 + (s >> 3);
      screen_[(y >> 3) * 128 + x] |= 1 << (y & 7);
    } while (++i != 0);
  }

  return;
#endif

  // construct rotation matrix
  int16_t cA, sA, cB, sB;

  GetSinCos((angle_A_ >> 6) & 1023, &sA, &cA);
  GetSinCos((angle_B_ >> 6) & 1023, &sB, &cB);

  // local coordinate frame given rotation values
  // 8.8 fixed point format rotation matrix
#if 1
  Vec388 Fx(cB, (int32_t) -cA*sB >> 8, (int32_t) sA*sB >> 8),
         Fy(sB, (int32_t) cA*cB >> 8, (int32_t) -sA*cB >> 8),
         Fz(0, sA, cA);
#else
  Vec388 Fx(cA, 0, sA),
         Fy(0, 256, 0),
         Fz(-sA, 0, cA);
#endif

  // rotate and project all vertices
  for (uint8_t i = 0, j = 0; i < NVERTS; i++, j += 3) {
    Vec38 obj_vert(
        pgm_read_byte_near(icosahedron_vertices + j),
        pgm_read_byte_near(icosahedron_vertices + j + 1),
        pgm_read_byte_near(icosahedron_vertices + j + 2));
    Vec388 world_vert(  // FIXME: extend precision a bit here, no >>8?
        Fx.dot(obj_vert),
        Fy.dot(obj_vert),
        Fz.dot(obj_vert));

    world_vert.project(scale_, distance_, verts + i);
#if 0
    if (screen_coord.x >= 0 && screen_coord.x < 128*16
        && screen_coord.y >= 0 && screen_coord.y <= 64*16) {
      uint8_t x = screen_coord.x >> 4;
      uint8_t y = screen_coord.y >> 4;
      screen_[(y >> 3) * 128 + x] |= 1 << (y & 7);
    }
#endif
  }

  // draw faces
  for (uint8_t j = 0; j < NFACES * 3; j += 3) {
    uint8_t fa = pgm_read_byte_near(icosahedron_faces + j),
            fb = pgm_read_byte_near(icosahedron_faces + j + 1),
            fc = pgm_read_byte_near(icosahedron_faces + j + 2);
    Vec216 sa = verts[fb] - verts[fa];
    Vec216 sb = verts[fc] - verts[fa];
    if ((int32_t) sa.x * sb.y > (int32_t) sa.y * sb.x) {  // check winding order
      continue;  // back-facing
    }
    Vec38 obj_normal(
        pgm_read_byte_near(icosahedron_normals + j),
        pgm_read_byte_near(icosahedron_normals + j + 1),
        pgm_read_byte_near(icosahedron_normals + j + 2));
    Vec388 world_normal(
        Fx.dot(obj_normal),
        Fy.dot(obj_normal),
        Fz.dot(obj_normal));
    int8_t illum = (world_normal.x + world_normal.y + 4 * world_normal.z) >> 4;
    uint8_t pat[4];
    GetDitherPattern(illum, frame_ & 1, pat);
    FillTriangle(
        verts[fa].x, verts[fa].y,
        verts[fb].x, verts[fb].y,
        verts[fc].x, verts[fc].y,
        pat, screen_);
  }
}

#if 0
bool CheckWinding(const Vec2f &s0, const Vec2f &s1, const Vec2f &s2) {
  float x1 = s1.x - s0.x,
        x2 = s2.x - s0.x,
        y1 = s1.y - s0.y,
        y2 = s2.y - s0.y;
  return x1*y2 < x2*y1;
}

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

  static const float kTurnSpeed = 0.1;
  static float scale = 2448;
  static float distance = 4;

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
    p0.project(scale, distance, &s0);
    // for each flipped axis, flip the "parity" so we generate a consistent
    // clockwise winding order on each face
    if ((f ^ (f>>1) ^ (f>>2)) & 1) {
      p1.project(scale, distance, &s1);
      p2.project(scale, distance, &s2);
    } else {
      p1.project(scale, distance, &s2);
      p2.project(scale, distance, &s1);
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
    int illum = 7 * N.x + 6 * N.y + 17 * N.z + 1;
    uint8_t pat[4];
    GetDitherPattern(illum & 15, frame_ & 1, pat);
    FillTriangle(s0x, s0y, s1x, s1y, s2x, s2y, pat, screen_);
  }
}
#endif

Stars stars_;

void loop() {
  if (arduboy_.nextFrame()) {
    stars_.Draw();
#if 0
    // big polygon clipping test
    uint8_t pat[] = {0xff, 0xff, 0xff, 0xff};
    int16_t x = ((frame_ & 511) - 256) * 16;
    FillTriangle(
        -1024 - x, 32*16,
        - x, -512,
        1024 - x, 1023,
        pat, screen_);
#endif

    ReadInput();
    DrawObject();
    arduboy_.display(true);
    ++frame_;
  }
}
