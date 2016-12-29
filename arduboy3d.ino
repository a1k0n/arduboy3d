#include <Arduboy2.h>
#include <avr/pgmspace.h>
#include "draw.h"
#include "sincos.h"
#include "vec.h"
#include "teapot.h"
// #include "icosahedron.h"

Arduboy2 arduboy_;
uint8_t *screen_;

static const size_t PROFILEBUFSIZ = 32;
static volatile uint8_t profilebuf_[PROFILEBUFSIZ];
static volatile uint8_t profileptr_ = 0;

ISR(TIMER4_OVF_vect) {
  if (profileptr_+2 <= PROFILEBUFSIZ) {
    uint8_t *profdata = profilebuf_ + profileptr_;
    // disassemble, count # pushes in prologue, add 1 to determine SP+x
    uint8_t *spdata = SP+17;
    profdata[1] = spdata[0];
    profdata[0] = spdata[1];
    profileptr_ += 2;
    for (uint8_t j = 2; j < 16 && profileptr_+2 < PROFILEBUFSIZ; j += 2) {
      // walk the stack and see if we have any return addresses available

      // subtract 2 words to get address of potential call instruction;
      // attribute this sample to that instruction
      uint16_t stackvalue = (spdata[j] << 8) + spdata[j+1] - 2;
      if (stackvalue >= 0x4000
          || pgm_read_word_near(stackvalue << 1) != 0x940e) {
        break;
      }
      profdata[j] = stackvalue & 255;
      profdata[j+1] = stackvalue >> 8;
      profdata[j-1] |= 0x80;  // add continuation bit on previous addr
      profileptr_ += 2;
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  arduboy_.begin();
  screen_ = arduboy_.getBuffer();
  arduboy_.setFrameRate(60);

  // set up timer interrupt, once every ... whatever
  TCCR4A = 0b00000000;    // no pwm, 
  TCCR4B = 0x08;    // clk / 128
  TIMSK4 = 0b00000100;  // enable ints
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

  angle_A_ += (A_target - angle_A_) >> 6;
  angle_B_ += (B_target - angle_B_) >> 6;
}

void DrawObject() {
  static const uint8_t NVERTS = sizeof(mesh_vertices) / 3;
  static const uint8_t NFACES = sizeof(mesh_faces) / 3;
  Vec216 verts[NVERTS];  // rotated, projected screen space vertices

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
  for (uint16_t i = 0, j = 0; i < NVERTS; i++, j += 3) {
    Vec38 obj_vert(
        pgm_read_byte_near(mesh_vertices + j),
        pgm_read_byte_near(mesh_vertices + j + 1),
        pgm_read_byte_near(mesh_vertices + j + 2));
    Vec388 world_vert(  // FIXME: extend precision a bit here, no >>8?
        Fx.dot(obj_vert),
        Fy.dot(obj_vert),
        Fz.dot(obj_vert));

    world_vert.project(scale_, distance_, verts + i);
#if 0
    if (screen_coord.x >= 0 && screen_coord.x < 128*16
        && screen_coord.y >= 0 && screen_coord.y <= 64*16) {
itCud2

      uint8_t x = screen_coord.x >> 4;
      uint8_t y = screen_coord.y >> 4;
      screen_[(y >> 3) * 128 + x] |= 1 << (y & 7);
    }
#endif
  }

  // draw faces
  for (uint16_t j = 0; j < NFACES * 3; j += 3) {
    uint8_t fa = pgm_read_byte_near(mesh_faces + j),
            fb = pgm_read_byte_near(mesh_faces + j + 1),
            fc = pgm_read_byte_near(mesh_faces + j + 2);
    Vec216 sa = verts[fb] - verts[fa];
    Vec216 sb = verts[fc] - verts[fa];
    if ((int32_t) sa.x * sb.y > (int32_t) sa.y * sb.x) {  // check winding order
      continue;  // back-facing
    }
#if 1
    Vec38 obj_normal(
        pgm_read_byte_near(mesh_normals + j),
        pgm_read_byte_near(mesh_normals + j + 1),
        pgm_read_byte_near(mesh_normals + j + 2));
    Vec388 world_normal(
        Fx.dot(obj_normal),
        Fy.dot(obj_normal),
        Fz.dot(obj_normal));
    int8_t illum = (world_normal.x + world_normal.y + 4 * world_normal.z) >> 5;
    uint8_t pat[4];
    GetDitherPattern(illum, pat);
    FillTriangle(
        verts[fa].x, verts[fa].y,
        verts[fb].x, verts[fb].y,
        verts[fc].x, verts[fc].y,
        pat, screen_);
#else
    if (verts[fa].x >= 0 && verts[fa].x < 16*128
        && verts[fa].y >= 0 && verts[fa].y <= 16*64) {
      screen_[(verts[fa].y >> 7) * 128 + (verts[fa].x >> 4)] |=
        1 << ((verts[fa].y >> 4) & 7);
    }
#endif
    // poll for profiling data
    if (profileptr_ == PROFILEBUFSIZ) {
      SerialUSB.write((char*) profilebuf_, PROFILEBUFSIZ);
      profileptr_ = 0;
    }
  }
}

Stars stars_;

static unsigned long t0_ = ~0L;
void loop() {
  if (t0_ == ~0L) {
    t0_ = micros();
  }
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
    unsigned long dt = micros() - t0_;
    int fps = 100000UL * frame_ / (dt / 100);
    arduboy_.setCursor(0, 0);
    arduboy_.print(fps/10);
    arduboy_.write('.');
    arduboy_.print(fps % 10);
    arduboy_.print(F(" FPS"));
  }
}
