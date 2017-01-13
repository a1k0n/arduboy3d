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
#if 1
    int16_t result16 = __builtin_avr_fmuls(x, b.x)
      + __builtin_avr_fmuls(y, b.y)
      + __builtin_avr_fmuls(z, b.z);
    return result16 >> 8;
#else
    /*
     * optimized version (eliminating redundant loads/stores/clr r1s) of this:
     * ...this seems to work in isolation, but not in the main loop. wtf.
     */
    register uint8_t result;
    asm("ld r18, Y\n\t"
        "ld r19, Z\n\t"
        "fmuls r18, r19\n\t"
        "movw r24, r0\n\t"
        "ldd r18, Y+1\n\t"
        "ldd r19, Z+1\n\t"
        "fmuls r18, r19\n\t"
        "add r24, r0\n\t"
        "adc r25, r1\n\t"
        "ldd r18, Y+2\n\t"
        "ldd r19, Z+2\n\t"
        "fmuls r18, r19\n\t"
        "add r24, r0\n\t"
        "adc r25, r1\n\t"
        "mov %0, r25\n\t"
        "clr r1\n\t"
        : "=r" (result) : "y" (&b), "z" (this) : "r0", "r1", "r18", "r19", "r25");
    return result;
#endif
  }

  void project(int16_t scale, Vec216 *out) {
    // "ooz" is one over z = 256 * scale / (dist - z)
    // where dist is fixed at 1024 and z ranges from -128 to 127
    //
    // we approximate it here with a linear taylor expansion which is just
    // ooz = 1024 + z!
    //
    // another term (z*z >> 10) adds a bit of accuracy but you can't see it
    // anyway on this tiny screen

    // since scale is O(1024), this is approx scale*1024/(1024-z)
    int16_t ooz = scale + z;
    out->x = 64*16 + ((int32_t) ooz * x >> 8);
    out->y = 32*16 - ((int32_t) ooz * y >> 8);
  }
};

struct Mat338 {
  Vec38 x, y, z;

  Mat338() {}

  Mat338(Vec38 &&_x, Vec38 &&_y, Vec38 &&_z):
    x(_x), y(_y), z(_z) {}

  // optimized combined rotation and projection of n vertices from progmem to SRAM
  void RotateAndProject(const int8_t* PROGMEM vertices, uint8_t n,
      int16_t scale, Vec216 *out) {
    do {
      int8_t v;
      v = pgm_read_byte_near(vertices++);
      int16_t px = __builtin_avr_fmuls(v, x.x);
      int16_t py = __builtin_avr_fmuls(v, y.x);
      int16_t pz = __builtin_avr_fmuls(v, z.x);
      v = pgm_read_byte_near(vertices++);
      px += __builtin_avr_fmuls(v, x.y);
      py += __builtin_avr_fmuls(v, y.y);
      pz += __builtin_avr_fmuls(v, z.y);
      v = pgm_read_byte_near(vertices++);
      px += __builtin_avr_fmuls(v, x.z);
      py += __builtin_avr_fmuls(v, y.z);
      pz += __builtin_avr_fmuls(v, z.z);

      pz = scale + (pz >> 8);  // approximate 1/z
      out->x = 64*16 + ((uint32_t) pz * (px>>8) >> 8);
      out->y = 32*16 - ((uint32_t) pz * (py>>8) >> 8);
      ++out;
    } while (--n);
  }

  // use fixed light source [1, 1, 4] for now... TODO: light source vector arg?
  int8_t CalcIllumination(const int8_t* PROGMEM normal) {
    // read and rotate normal vector
    int16_t ill;
    int8_t v = pgm_read_byte_near(normal++);
    int16_t px = __builtin_avr_fmuls(v, x.x);
    int16_t py = __builtin_avr_fmuls(v, y.x);
    int16_t pz = __builtin_avr_fmuls(v, z.x);
    v = pgm_read_byte_near(normal++);
    px += __builtin_avr_fmuls(v, x.y);
    py += __builtin_avr_fmuls(v, y.y);
    pz += __builtin_avr_fmuls(v, z.y);
    v = pgm_read_byte_near(normal);
    px += __builtin_avr_fmuls(v, x.z);
    py += __builtin_avr_fmuls(v, y.z);
    pz += __builtin_avr_fmuls(v, z.z);
    return (px >> (8+5)) + (py >> (8+5)) + (pz >> (8+3));
  }
};

#endif  // VEC_H_
