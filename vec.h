#ifndef VEC_H_
#define VEC_H_

struct Vec216 {
  int16_t x, y;

  Vec216() {}
  Vec216(int16_t _x, int16_t _y):
    x(_x), y(_y) {}
};

static Vec216 operator-(const Vec216 &a, const Vec216 &b) {
  return Vec216(a.x - b.x, a.y - b.y);
}

// 8 bit fixed point 3-vector, all magnitude < 1
struct Vec38 {
  int8_t x, y, z;
  Vec38() {}
  Vec38(int8_t _x, int8_t _y, int8_t _z):
    x(_x), y(_y), z(_z) {}

  int8_t dot(const Vec38 &b) {
    int16_t result16 = __builtin_avr_fmuls(x, b.x)
      + __builtin_avr_fmuls(y, b.y)
      + __builtin_avr_fmuls(z, b.z);
    return result16 >> 8;
  }

  void project(int16_t scale, Vec216 *out) {
    // projection is a precomputed lookup table, with a fixed distance from
    // camera of 1024 and an assumed z range of -128..127

    // "ooz" is one over z = 256 * scale / (dist - z)
    // where dist is fixed at 1024 and z ranges from -128 to 127
    //
    // we approximate it here with a linear taylor expansion which is just
    // ooz = 1024 + z!
    //
    // another term (z*z >> 10) adds a bit of accuracy but you can't see it
    // anyway on this tiny screen
    int16_t ooz = 1024 + z;
    ooz += scale - 4096;  // fudge the scale here
    out->x = 64*16 + ((int32_t) ooz * x >> 8);
    out->y = 32*16 - ((int32_t) ooz * y >> 8);
  }
};

#endif  // VEC_H_
