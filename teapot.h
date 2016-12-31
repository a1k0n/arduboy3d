static const int8_t mesh_vertices[] PROGMEM = {
47, 0, 33, 31, -39, 33, 31, -39, 36, 47, 0, 36, -8, -55, 33, -8, -55, 36, 34, -42, 33, 51, 0, 33, -8, -59, 33, -48, -39, 33, -48, -39, 36, -64, 0, 33, -64, 0, 36, -51, -42, 33, -68, 0, 33, -48, 39, 33, -48, 39, 36, -8, 55, 33, -8, 55, 36, -51, 42, 33, -8, 59, 33, 31, 39, 33, 31, 39, 36, 34, 42, 33, 43, -52, 2, 64, 0, 2, -8, -73, 2, 48, -56, -27, 71, 0, -27, -8, -79, -27, -60, -52, 2, -81, 0, 2, -65, -56, -27, -87, 0, -27, -60, 52, 2, -8, 73, 2, -65, 56, -27, -8, 79, -27, 43, 52, 2, 48, 56, -27, 41, -49, -47, 61, 0, -47, -8, -69, -47, 34, -42, -56, 51, 0, -56, -8, -59, -56, -58, -49, -47, -78, 0, -47, -51, -42, -56, -68, 0, -56, -58, 49, -47, -8, 69, -47, -51, 42, -56, -8, 59, -56, 41, 49, -47, 34, 42, -56, -72, 0, 18, -70, -9, 22, -108, -9, 21, -104, 0, 17, -68, 0, 27, -112, 0, 24, -121, -9, 9, -115, 0, 9, -127, 0, 9, -70, 9, 22, -108, 9, 21, -121, 9, 9, -113, -9, -13, -109, 0, -9, -116, 0, -16, -86, -9, -33, -84, 0, -39, -113, 9, -13, -86, 9, -33, 59, 0, -6, 59, -20, -22, 92, -13, 2, 86, 0, 9, 59, 0, -39, 98, 0, -5, 110, -7, 33, 98, 0, 33, 122, 0, 33, 59, 20, -22, 92, 13, 2, 110, 7, 33, 115, -6, 35, 103, 0, 35, 127, 0, 36, 110, -4, 33, 102, 0, 33, 118, 0, 33, 115, 6, 35, 110, 4, 33, 1, -9, 56, 4, 0, 56, -8, 0, 62, -8, -13, 56, -3, -6, 44, -1, 0, 44, -8, -8, 44, -18, -9, 56, -21, 0, 56, -14, -6, 44, -16, 0, 44, -18, 9, 56, -8, 13, 56, -14, 6, 44, -8, 8, 44, 1, 9, 56, -3, 6, 44, 15, -23, 39, 24, 0, 39, -8, -33, 39, 28, -36, 33, 43, 0, 33, -8, -51, 33, -32, -23, 39, -41, 0, 39, -45, -36, 33, -60, 0, 33, -32, 23, 39, -8, 33, 39, -45, 36, 33, -8, 51, 33, 15, 23, 39, 28, 36, 33, 28, 36, -60, 42, 0, -60, -8, 0, -62, -8, 51, -60, -44, 36, -60, -59, 0, -60, -44, -36, -60, -8, -51, -60, 28, -36, -60
};

static const float mesh_normal_scale = 7.622044e-03;
static const int8_t mesh_normals[] PROGMEM = {
4, 1, -127, 1, 4, -126, 54, 22, -112, 22, 54, -112, -1, 4, -127, -4, 1, -126, -22, 54, -112, -54, 22, -112, -4, -1, -127, -1, -4, -126, -54, -22, -112, -22, -54, -112, 1, -4, -127, 4, -1, -126, 22, -54, -112, 54, -22, -112, 83, -34, -89, 34, -83, -89, -34, -83, -89, -83, -34, -89, -83, 34, -89, -34, 83, -89, 34, 83, -89, 83, 34, -89, 54, 22, -112, 22, 54, -112, -22, 54, -112, -54, 22, -112, -54, -22, -112, -22, -54, -112, 22, -54, -112, 54, -22, -112, 107, -43, -52, 43, -107, -52, 83, -34, -89, 34, -83, -89, -43, -107, -52, -107, -43, -52, -34, -83, -89, -83, -34, -89, -107, 43, -52, -43, 107, -52, -83, 34, -89, -34, 83, -89, 43, 107, -52, 107, 43, -52, 34, 83, -89, 83, 34, -89, -69, -47, -94, 71, 67, -80, 76, -43, 91, -65, 50, -96, 115, -47, 24, 47, -115, 24, -47, -115, 24, -115, -47, 24, -115, 47, 24, -47, 115, 24, 47, 115, 24, 115, 47, 24, 107, -43, -52, 43, -107, -52, -43, -107, -52, -107, -43, -52, -107, 43, -52, -43, 107, -52, 43, 107, -52, 107, 43, -52, 69, 47, 94, 69, -68, -81, -49, 75, 90, -103, -61, -40, -64, 65, -87, 104, -61, 37, -60, -64, -90, -102, 62, -42, 69, 65, 83, 64, -65, 87, 103, 61, 40, -51, -74, 89, 70, -71, -78, 98, 66, -45, 108, -44, 48, 44, -108, 48, 115, -47, 24, 47, -115, 24, -44, -108, 48, -108, -44, 48, -47, -115, 24, -115, -47, 24, -108, 44, 48, -44, 108, 48, -115, 47, 24, -47, 115, 24, 44, 108, 48, 108, 44, 48, 47, 115, 24, 115, 47, 24, -50, -70, 92, 89, -69, -57, 68, 71, -79, -95, 67, 49, 67, -44, -98, -77, -51, 86, -82, 55, 78, 77, 51, -86, 100, -66, 39, -99, -66, -41, -100, 66, -39, 101, 67, 36, -84, -70, 63, -53, 70, 91, 71, -68, -79, 5, 52, -115, 4, -55, -114, 4, -52, -115, -77, -68, 73, -5, 53, 115, 60, 67, -88, -5, -55, 114, 3, 56, -113, -5, -52, 115, -71, 68, 79, -4, 55, 114, -117, 48, 3, -48, 117, 3, 86, -35, 85, 35, -86, 85, 48, 117, 3, 117, 48, 3, -35, -86, 85, -86, -35, 85, 117, -48, 3, 48, -117, 3, -86, 35, 85, -35, 86, 85, -48, -117, 3, -117, -48, 3, 35, 86, 85, 86, 35, 85, 108, -44, 48, 44, -108, 48, -44, -108, 48, -108, -44, 48, -108, 44, 48, -44, 108, 48, 44, 108, 48, 108, 44, 48, -64, -103, 33, 66, -106, -18, 63, 102, -40, -60, 97, 55, -37, -60, 105, 50, -80, -83, 60, 108, -27, -35, 63, 103, 59, 95, -59, -43, 69, 96, -35, -63, 104, 57, -101, -51, 37, -15, 120, 15, -37, 120, -15, -37, 120, -37, -15, 120, -37, 15, 120, -15, 37, 120, 15, 37, 120, 37, 15, 120, -41, -76, 92, 54, 106, -43, -35, 63, 104, 46, 88, -77, -35, 64, 103, 56, -111, -24, -117, 48, 3, -48, 117, 3, 86, -35, 85, 35, -86, 85, 48, 117, 3, 117, 48, 3, -35, -86, 85, -86, -35, 85, 117, -48, 3, 48, -117, 3, -86, 35, 85, -35, 86, 85, -48, -117, 3, -117, -48, 3, 35, 86, 85, 86, 35, 85, 54, -106, -43, -35, -64, 103, 29, -12, 123, 12, -29, 123, 37, -15, 120, 15, -37, 120, -12, -29, 123, -29, -12, 123, -15, -37, 120, -37, -15, 120, -29, 12, 123, -12, 29, 123, -37, 15, 120, -15, 37, 120, 12, 29, 123, 29, 12, 123, 15, 37, 120, 37, 15, 120, 108, -44, -48, 44, -108, -48, -44, -108, -48, -108, -44, -48, -108, 44, -48, -44, 108, -48, 44, 108, -48, 108, 44, -48, 29, -12, 123, 12, -29, 123, -12, -29, 123, -29, -12, 123, -29, 12, 123, -12, 29, 123, 12, 29, 123, 29, 12, 123, 57, -23, 110, 23, -57, 110, 108, -44, -48, 44, -108, -48, -23, -57, 110, -57, -23, 110, -44, -108, -48, -108, -44, -48, -57, 23, 110, -23, 57, 110, -108, 44, -48, -44, 108, -48, 23, 57, 110, 57, 23, 110, 44, 108, -48, 108, 44, -48
};

// 240 faces
static const uint8_t mesh_faces[] PROGMEM = {
128, 129, 130, 131, 128, 130, 129, 128, 55, 128, 131, 53, 132, 131, 130, 133, 132, 130, 131, 132, 52, 132, 133, 49, 134, 133, 130, 135, 134, 130, 133, 134, 48, 134, 135, 45, 136, 135, 130, 129, 136, 130, 135, 136, 43, 136, 129, 44, 43, 44, 41, 45, 43, 40, 48, 45, 42, 49, 48, 46, 52, 49, 47, 53, 52, 50, 55, 53, 51, 44, 55, 54, 55, 44, 129, 53, 55, 128, 52, 53, 131, 49, 52, 132, 48, 49, 133, 45, 48, 134, 43, 45, 135, 44, 43, 136, 40, 41, 28, 42, 40, 27, 41, 40, 43, 40, 42, 45, 46, 42, 29, 47, 46, 32, 42, 46, 48, 46, 47, 49, 50, 47, 33, 51, 50, 36, 47, 50, 52, 50, 51, 53, 54, 51, 37, 41, 54, 39, 51, 54, 55, 54, 41, 44, 72, 71, 68, 79, 84, 85, 71, 33, 69, 74, 72, 70, 27, 28, 25, 29, 27, 24, 32, 29, 26, 33, 32, 30, 36, 33, 31, 37, 36, 34, 39, 37, 35, 28, 39, 38, 28, 27, 40, 27, 29, 42, 29, 32, 46, 32, 33, 47, 33, 36, 50, 36, 37, 51, 37, 39, 54, 39, 28, 41, 33, 74, 73, 76, 79, 80, 84, 75, 78, 70, 68, 62, 70, 73, 74, 68, 69, 63, 68, 70, 72, 73, 70, 64, 73, 69, 33, 69, 68, 71, 69, 73, 67, 75, 76, 77, 80, 77, 76, 80, 85, 86, 24, 25, 7, 26, 24, 6, 25, 24, 27, 24, 26, 29, 30, 26, 8, 31, 30, 13, 26, 30, 32, 30, 31, 33, 34, 31, 14, 35, 34, 19, 31, 34, 36, 34, 35, 37, 38, 35, 20, 25, 38, 23, 35, 38, 39, 38, 25, 28, 77, 78, 75, 77, 80, 83, 85, 80, 79, 85, 78, 82, 62, 63, 59, 64, 62, 58, 67, 64, 61, 63, 67, 66, 63, 62, 68, 62, 64, 70, 64, 67, 73, 67, 63, 69, 78, 77, 81, 78, 85, 84, 59, 58, 62, 59, 66, 65, 56, 57, 58, 58, 59, 56, 58, 61, 64, 66, 61, 60, 66, 59, 63, 57, 60, 61, 65, 56, 59, 61, 58, 57, 61, 66, 67, 60, 65, 66, 0, 1, 2, 1, 4, 5, 6, 7, 3, 8, 6, 2, 4, 9, 10, 9, 11, 12, 13, 8, 5, 14, 13, 10, 11, 15, 16, 15, 17, 18, 19, 14, 12, 20, 19, 16, 17, 21, 22, 21, 0, 3, 23, 20, 18, 7, 23, 22, 7, 6, 24, 6, 8, 26, 8, 13, 30, 13, 14, 31, 14, 19, 34, 19, 20, 35, 20, 23, 38, 23, 7, 25, 81, 82, 78, 83, 81, 77, 86, 83, 80, 82, 86, 85, 82, 81, 87, 81, 83, 89, 90, 91, 88, 92, 90, 87, 83, 86, 93, 86, 82, 88, 94, 92, 89, 91, 94, 93, 115, 116, 113, 117, 115, 112, 120, 117, 114, 121, 120, 118, 124, 121, 119, 125, 124, 122, 127, 125, 123, 116, 127, 126, 87, 88, 82, 88, 87, 90, 87, 89, 92, 93, 89, 83, 88, 93, 86, 93, 88, 91, 2, 3, 0, 5, 2, 1, 3, 2, 6, 2, 5, 8, 10, 5, 4, 12, 10, 9, 5, 10, 13, 10, 12, 14, 16, 12, 11, 18, 16, 15, 12, 16, 19, 16, 18, 20, 22, 18, 17, 3, 22, 21, 18, 22, 23, 22, 3, 7, 89, 87, 81, 89, 93, 94, 112, 113, 100, 114, 112, 99, 113, 112, 115, 112, 114, 117, 118, 114, 101, 119, 118, 104, 114, 118, 120, 118, 119, 121, 122, 119, 105, 123, 122, 108, 119, 122, 124, 122, 123, 125, 126, 123, 109, 113, 126, 111, 123, 126, 127, 126, 113, 116, 99, 100, 96, 101, 99, 95, 104, 101, 98, 105, 104, 102, 108, 105, 103, 109, 108, 106, 111, 109, 107, 100, 111, 110, 100, 99, 112, 99, 101, 114, 101, 104, 118, 104, 105, 119, 105, 108, 122, 108, 109, 123, 109, 111, 126, 111, 100, 113, 95, 96, 97, 98, 95, 97, 96, 95, 99, 95, 98, 101, 102, 98, 97, 103, 102, 97, 98, 102, 104, 102, 103, 105, 106, 103, 97, 107, 106, 97, 103, 106, 108, 106, 107, 109, 110, 107, 97, 96, 110, 97, 107, 110, 111, 110, 96, 100
};

