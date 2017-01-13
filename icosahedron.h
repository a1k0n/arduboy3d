static const uint16_t mesh_NVERTS = 12;
static const uint16_t mesh_NFACES = 20;

static const int8_t mesh_vertices[] PROGMEM = {  // xyz vertex
0, -67, 108, 108, 0, 67, 108, 0, -67, -108, 0, -67, -108, 0, 67, -67, 108, 0, 67, 108, 0, 67, -108, 0, -67, -108, 0, 0, -67, -108, 0, 67, -108, 0, 67, 108
};

static const float mesh_normal_scale = 7.355687e-03;
static const int8_t mesh_normals[] PROGMEM = {  // xyz face normal
-127, 48, 0, -127, -48, 0, -78, 78, -78, -78, 78, 78, -78, -78, -78, -78, -78, 78, -48, 0, -127, -48, 0, 127, 0, 127, 48, 0, 127, -48, 0, -127, -48, 0, -127, 48, 48, 0, -127, 48, 0, 127, 78, 78, -78, 78, 78, 78, 78, -78, -78, 78, -78, 78, 127, 48, 0, 127, -48, 0
};

static const uint8_t mesh_faces[] PROGMEM = {  // x-sorted vertex index
3, 4, 5, 4, 3, 8, 3, 5, 10, 5, 4, 11, 3, 9, 8, 4, 8, 0, 10, 9, 3, 0, 11, 4, 6, 5, 11, 5, 6, 10, 7, 8, 9, 8, 7, 0, 9, 10, 2, 11, 0, 1, 6, 2, 10, 1, 6, 11, 2, 7, 9, 7, 1, 0, 1, 2, 6, 1, 7, 2
};

static const uint8_t mesh_ysort_faces[] PROGMEM = {  // y-sorted face index
10, 11, 4, 5, 16, 17, 1, 19, 6, 7, 12, 13, 0, 18, 2, 3, 14, 15, 8, 9
};

static const uint8_t mesh_zsort_faces[] PROGMEM = {  // z-sorted face index
6, 12, 2, 4, 14, 16, 9, 10, 0, 1, 18, 19, 8, 11, 3, 5, 15, 17, 7, 13
};

