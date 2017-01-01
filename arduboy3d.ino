#include <Arduboy2.h>
#include <avr/pgmspace.h>
#include "draw.h"
#include "sincos.h"
#include "vec.h"
#include "teapot.h"
// #include "icosahedron.h"

Arduboy2 arduboy_;
uint8_t *screen_;

#ifdef _PROFILE
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
#endif

void setup() {
  // put your setup code here, to run once:
  arduboy_.begin();
  screen_ = arduboy_.getBuffer();
  arduboy_.setFrameRate(60);

#ifdef _PROFILE
  // set up timer interrupt, once every ... whatever
  TCCR4A = 0b00000000;    // no pwm, 
  TCCR4B = 0x08;    // clk / 128
  TIMSK4 = 0b00000100;  // enable ints
#endif
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

int32_t angle_A_ = 0, angle_B_ = 0, angle_C_ = 0;
int16_t scale_ = 4096;

void ReadInput() {
//  static int32_t A_target = 100000, B_target = 50000;
  static int32_t A_target = 0, B_target = 0, C_target = 0;
  static const int32_t kTurnSpeed = 1000;

  if (arduboy_.pressed(A_BUTTON)) {
    if (arduboy_.pressed(LEFT_BUTTON)) {
      A_target -= kTurnSpeed;
    }
    if (arduboy_.pressed(RIGHT_BUTTON)) {
      A_target += kTurnSpeed;
    }
    if (arduboy_.pressed(UP_BUTTON)) {
      scale_ += 10;
    }
    if (arduboy_.pressed(DOWN_BUTTON)) {
      scale_ -= 10;
    }
  } else {
    if (arduboy_.pressed(LEFT_BUTTON)) {
      B_target -= kTurnSpeed;
    }
    if (arduboy_.pressed(RIGHT_BUTTON)) {
      B_target += kTurnSpeed;
    }
    if (arduboy_.pressed(UP_BUTTON)) {
      C_target -= kTurnSpeed;
    }
    if (arduboy_.pressed(DOWN_BUTTON)) {
      C_target += kTurnSpeed;
    }
  }

  angle_A_ += (A_target - angle_A_) >> 6;
  angle_B_ += (B_target - angle_B_) >> 6;
  angle_C_ += (C_target - angle_C_) >> 6;
}

static Vec216 verts[mesh_NVERTS];  // rotated, projected screen space vertices

// down-convert signed 1.10 precision to signed 0.7 precision
// (scaling down from -1024..1024 to -127..127)
static int8_t RescaleR(int16_t x) {
  return ((uint32_t) x*127 + 512) >> 10;
}

void DrawObject() {
  // construct rotation matrix
  int16_t cA, sA, cB, sB, cC, sC;

  GetSinCos((angle_A_ >> 6) & 1023, &sA, &cA);
  GetSinCos((angle_B_ >> 6) & 1023, &sB, &cB);
  GetSinCos((angle_C_ >> 6) & 1023, &sC, &cC);

  // rotate about X axis by C, then Y axis by B, then Z axis by A
  //     [            cA*cB,             cB*sA,    sB]
  // R = [-cA*sB*sC - cC*sA,  cA*cC - sA*sB*sC, cB*sC]
  //     [-cA*cC*sB + sA*sC, -cA*sC - cC*sA*sB, cB*cC]

  // local coordinate frame given rotation values
  // spend some time up front to get an accurate rotation matrix before
  // rounding to 0.7 fixed point
  // the 32-bit math is only done once per frame, and then we can do
  // all the per-vertex stuff in 8-bit math 
  Vec38 Fx(
      RescaleR((int32_t) cA*cB >> 10),
      RescaleR((int32_t) cB*sA >> 10),
      RescaleR(sB));
  Vec38 Fy(
      RescaleR(((int32_t) -cA*sB*sC >> 10) - (int32_t) cC*sA >> 10),
      RescaleR((int32_t) cA*cC - ((int32_t) sA*sB*sC >> 10) >> 10),
      RescaleR((int32_t) cB*sC >> 10));
  Vec38 Fz(
      RescaleR(((int32_t) -cA*cC*sB >> 10) + (int32_t) sA*sC >> 10),
      RescaleR((int32_t) -cA*sC - ((int32_t) cC*sA*sB >> 10) >> 10),
      RescaleR((int32_t) cB*cC >> 10));

  int8_t sortaxis = 0, sortaxisz = Fz.x;
  if (abs(Fz.y) > abs(sortaxisz)) {
    sortaxis = 1;
    sortaxisz = Fz.y;
  }
  if (abs(Fz.z) > abs(sortaxisz)) {
    sortaxis = 2;
    sortaxisz = Fz.z;
  }
  arduboy_.setCursor(0, 8);
  arduboy_.print(sortaxis);
  arduboy_.write(' ');
  arduboy_.print(sortaxisz);

#if 0
  Serial.println(F("A: "));
  Serial.print(angle_A_); Serial.print(' ');
  Serial.print(cA); Serial.print(' ');
  Serial.print(sA); Serial.print('\n');
  Serial.println(F("matrix:"));
  Serial.print(Fx.x); Serial.print(' ');
  Serial.print(Fx.y); Serial.print(' ');
  Serial.print(Fx.z); Serial.print('\n');
  Serial.print(Fy.x); Serial.print(' ');
  Serial.print(Fy.y); Serial.print(' ');
  Serial.print(Fy.z); Serial.print('\n');
  Serial.print(Fz.x); Serial.print(' ');
  Serial.print(Fz.y); Serial.print(' ');
  Serial.print(Fz.z); Serial.print('\n');
#endif

  // rotate and project all vertices
  for (uint16_t i = 0, j = 0; i < mesh_NVERTS; i++, j += 3) {
    Vec38 obj_vert(
        pgm_read_byte_near(mesh_vertices + j),
        pgm_read_byte_near(mesh_vertices + j + 1),
        pgm_read_byte_near(mesh_vertices + j + 2));
    Vec38 world_vert(  // FIXME: use Vec38 here?
        Fx.dot(obj_vert),
        Fy.dot(obj_vert),
        Fz.dot(obj_vert));

    world_vert.project(scale_, verts + i);
    // vertex_z[i] = world_vert.z;
  }

  // back-face cull and sort faces
  for (uint16_t i = 0; i < mesh_NFACES; i++) {
    uint16_t jf = i;
    // use face sort order depending on which axis is most facing toward camera
    if (sortaxisz < 0) {
      jf = mesh_NFACES - 1 - i;
    }
    if (sortaxis == 1) {
      jf = pgm_read_byte_near(mesh_ysort_faces + jf);
    } else if (sortaxis == 2) {
      jf = pgm_read_byte_near(mesh_zsort_faces + jf);
    }
    jf *= 3;
    uint8_t fa = pgm_read_byte_near(mesh_faces + jf),
            fb = pgm_read_byte_near(mesh_faces + jf + 1),
            fc = pgm_read_byte_near(mesh_faces + jf + 2);
    Vec216 sa = verts[fb] - verts[fa];
    Vec216 sb = verts[fc] - verts[fa];
    if ((int32_t) sa.x * sb.y > (int32_t) sa.y * sb.x) {  // check winding order
      continue;  // back-facing
    }

#if 1
    Vec38 obj_normal(
        pgm_read_byte_near(mesh_normals + jf),
        pgm_read_byte_near(mesh_normals + jf + 1),
        pgm_read_byte_near(mesh_normals + jf + 2));
    Vec38 world_normal(
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

#ifdef _PROFILE
    // poll for profiling data
    if (profileptr_ == PROFILEBUFSIZ) {
      SerialUSB.write((char*) profilebuf_, PROFILEBUFSIZ);
      profileptr_ = 0;
    }
#endif
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
    ReadInput();
    DrawObject();
    ++frame_;
    unsigned long dt = micros() - t0_;
    int fps = 100000UL * frame_ / (dt / 100);
    arduboy_.setCursor(0, 0);
    arduboy_.print(fps/10);
    arduboy_.write('.');
    arduboy_.print(fps % 10);
    arduboy_.print(F(" FPS"));
    arduboy_.display(true);
  }
}
