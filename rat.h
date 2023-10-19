static const uint16_t mesh_NVERTS = 32;
static const uint16_t mesh_NFACES = 42;

static const int8_t mesh_vertices[] PROGMEM = {  // xyz vertex
92, 9, -15, 83, -18, -10, 92, 9, 14, 83, -18, 9, 19, 10, -23, 35, -22, -29, 19, 10, 22, 35, -22, 28, 126, -19, -1, -3, -7, -1, -3, -18, -6, -3, -18, 5, 51, 24, -1, -126, -13, -1, 87, -28, 20, 88, -28, 14, 100, -28, 14, 87, -28, -21, 88, -28, -15, 101, -28, -15, 31, -28, 30, 32, -28, 24, 51, -28, 21, 31, -28, -30, 32, -28, -24, 51, -28, -21, 95, 23, -18, 98, 28, -11, 98, 21, -22, 95, 23, 17, 98, 27, 10, 98, 21, 21
};

static const float mesh_normal_scale = 7.874016e-03;
static const int8_t mesh_normals[] PROGMEM = {  // xyz face normal
-2, 56, -113, -6, -126, 0, -2, 56, 113, -76, 101, 0, -80, -23, 95, -64, -13, -108, -12, -126, 0, -52, 115, 0, 99, -76, 16, -98, 78, 16, 0, 126, 0, 0, -126, 0, -48, -2, -117, 48, -18, -115, 14, -77, 99, -12, 88, 90, 13, 27, -123, 8, 100, -76, 8, 100, 77, 12, -28, 123, 10, -126, 0, 43, 119, 0, 107, 65, 17, -106, -66, 17, -30, -98, -73, 30, 98, -73, 0, 62, 110, 0, -62, 110, 0, 127, 0, 0, -127, 0, 120, -30, -24, -120, 30, -23, 99, -4, 78, -99, 4, 78, -115, 42, -31, 115, -42, -31, 10, -107, 66, -11, 107, 65, -2, -126, 0, 25, -32, -119, 25, -32, 119, 82, 96, 0
};

static const uint8_t mesh_faces[] PROGMEM = {  // x-sorted vertex index
9, 10, 13, 10, 11, 13, 11, 9, 13, 6, 4, 9, 7, 6, 9, 4, 5, 10, 5, 7, 11, 4, 6, 12, 21, 7, 20, 24, 5, 23, 21, 20, 22, 24, 23, 25, 22, 20, 7, 25, 23, 5, 21, 22, 7, 24, 25, 5, 5, 4, 0, 0, 4, 12, 6, 2, 12, 3, 2, 6, 5, 1, 3, 2, 0, 12, 15, 3, 14, 18, 1, 17, 16, 14, 3, 19, 17, 1, 15, 16, 3, 18, 19, 1, 15, 14, 16, 18, 17, 19, 27, 0, 26, 30, 2, 29, 28, 26, 0, 31, 29, 2, 27, 28, 0, 30, 31, 2, 27, 26, 28, 30, 29, 31, 3, 1, 8, 1, 0, 8, 2, 3, 8, 0, 2, 8
};

static const uint8_t mesh_ysort_faces[] PROGMEM = {  // y-sorted face index
28, 10, 29, 11, 8, 12, 14, 13, 15, 9, 22, 24, 26, 23, 25, 27, 6, 20, 38, 1, 0, 2, 5, 40, 39, 4, 16, 41, 19, 3, 21, 18, 17, 7, 33, 32, 35, 34, 31, 30, 37, 36
};

static const uint8_t mesh_zsort_faces[] PROGMEM = {  // z-sorted face index
9, 13, 11, 15, 16, 5, 32, 36, 29, 34, 23, 25, 30, 27, 17, 20, 39, 0, 21, 41, 1, 3, 7, 38, 2, 6, 40, 18, 26, 31, 22, 24, 19, 35, 37, 28, 4, 33, 14, 10, 12, 8
};
