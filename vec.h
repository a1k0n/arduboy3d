#ifndef VEC_H_
#define VEC_H_

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

#endif  // VEC_H_
