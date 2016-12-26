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

  void project(int16_t scale, int16_t dist, Vec216 *out) {
    // int16_t ooz = 256 / (dist - z);  // is this going to be precise enough?!
    out->x = 64*16 + (int32_t) scale * x / (dist - z);
    out->y = 32*16 - (int32_t) scale * y / (dist - z);
  }

  Vec388 operator-() {
    return Vec388(-x, -y, -z);
  }
};

// static Vec388 operator+(const Vec388 &a, const Vec388 &b) {
//   return Vec388(a.x + b.x, a.y + b.y, a.z + b.z);
// }

#endif  // VEC_H_
