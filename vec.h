#ifndef VEC_H_
#define VEC_H_

#if 0
struct Vec2f {
  float x, y;

  Vec2f() {}
  Vec2f(float _x, float _y):
    x(_x), y(_y) {}
};

struct Vec3f {
  float x, y, z;

  Vec3f() {}
  Vec3f(float _x, float _y, float _z):
    x(_x), y(_y), z(_z) {}

  float dot(const Vec3f &a) {
    return x * a.x + y * a.y + z * a.z;
  }

  void project(float scale, float dist, Vec2f *out) {
    float ooz = 1.0f / (dist - z);
    out->x = 64*16 + scale * x * ooz;
    out->y = 32*16 - scale * y * ooz;
  }

  Vec3f operator-() {
    return Vec3f(-x, -y, -z);
  }
};

static Vec3f operator+(const Vec3f &a, const Vec3f &b) {
  return Vec3f(a.x + b.x, a.y + b.y, a.z + b.z);
}
#endif

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
};

static const uint8_t project_lut_[] PROGMEM = {
	0,   1,   1,   2,   3,   4,   4,   5,   6,   7,   8,   8,   9,
   10,  11,  12,  13,  13,  14,  15,  16,  17,  17,  18,  19,  20,
   21,  22,  22,  23,  24,  25,  26,  27,  27,  28,  29,  30,  31,
   32,  32,  33,  34,  35,  36,  37,  38,  38,  39,  40,  41,  42,
   43,  44,  44,  45,  46,  47,  48,  49,  50,  51,  51,  52,  53,
   54,  55,  56,  57,  58,  59,  60,  60,  61,  62,  63,  64,  65,
   66,  67,  68,  69,  69,  70,  71,  72,  73,  74,  75,  76,  77,
   78,  79,  80,  81,  82,  82,  83,  84,  85,  86,  87,  88,  89,
   90,  91,  92,  93,  94,  95,  96,  97,  98,  99, 100, 101, 102,
  103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115,
  116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128,
  129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141,
  142, 143, 144, 145, 147, 148, 149, 150, 151, 152, 153, 154, 155,
  156, 157, 158, 159, 161, 162, 163, 164, 165, 166, 167, 168, 169,
  171, 172, 173, 174, 175, 176, 177, 178, 179, 181, 182, 183, 184,
  185, 186, 187, 189, 190, 191, 192, 193, 194, 196, 197, 198, 199,
  200, 201, 203, 204, 205, 206, 207, 209, 210, 211, 212, 213, 215,
  216, 217, 218, 219, 221, 222, 223, 224, 226, 227, 228, 229, 230,
  232, 233, 234, 235, 237, 238, 239, 241, 242, 243, 244, 246, 247,
  248, 249, 251, 252, 253, 255, 0, 1, 2 /* 256, 257, 258 */
};
static const uint16_t project_lut_offset_ = 910;

// 8.8 fixed point 3-vector
struct Vec388 {
  int16_t x, y, z;

  Vec388() {}
  Vec388(int16_t _x, int16_t _y, int16_t _z):
    x(_x), y(_y), z(_z) {}

  int16_t dot(const Vec388 &a) {
    return ((int32_t) x * a.x + (int32_t) y * a.y + (int32_t) z * a.z) >> 8;
  }

  int16_t dot(const Vec38 &a) {
    return ((int32_t) x * a.x + (int32_t) y * a.y + (int32_t) z * a.z) >> 8;
  }

  void project(int16_t scale, Vec216 *out) {
    // projection is a precomputed lookup table, with a fixed distance from
    // camera of 1024 and an assumed z range of -128..127
    uint8_t lutoffset = 128 + z;
    /* z should be in -127..127 already
     * if not, i guess we could shift to compensate
    if (lutoffset < 0) lutoffset = 0;
    if (lutoffset > 255) lutoffset = 255;
    */
	int16_t ooz = (uint8_t) pgm_read_byte_near(project_lut_ + lutoffset);
    ooz += project_lut_offset_;
    if (lutoffset >= 256-3) {
      ooz += 256;
    }
    ooz += scale - 4096;
    out->x = 64*16 + ((int32_t) ooz * x >> 8);
    out->y = 32*16 - ((int32_t) ooz * y >> 8);
  }

  Vec388 operator-() {
    return Vec388(-x, -y, -z);
  }
};

// static Vec388 operator+(const Vec388 &a, const Vec388 &b) {
//   return Vec388(a.x + b.x, a.y + b.y, a.z + b.z);
// }

#endif  // VEC_H_
