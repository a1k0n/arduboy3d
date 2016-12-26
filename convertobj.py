import numpy as np
import sys


def LoadObj(infile):
    objname = None
    verts = []
    normals = []
    faces = []   # 3 vertex indices

    for line in infile:
        line = line.strip().split()
        if len(line) > 1:
            if line[0] == 'g':
                objname = line[1]
            if line[0] == 'v':  # vertex
                v = np.array(map(float, line[1:]))
                verts.append(v)
            elif line[0] == 'vn':  # normal
                v = np.array(map(float, line[1:]))
                v /= np.linalg.norm(v)
                normals.append(v)
            elif line[0] == 'f':  # face
                coords = [f.split('/') for f in line[1:]]
                # we assume normals must be in face order
                assert int(coords[0][2]) - 1 == len(faces)
                # if they aren't, they either have shared normals
                # or we can just reorder
                faces.append(
                    [int(coords[0][0]) - 1, int(coords[1][0]) - 1,
                     int(coords[2][0]) - 1])
    verts = np.array(verts)
    normals = np.array(normals)
    faces = np.array(faces)
    return objname, verts, normals, faces


def ConvertObj(infile):
    objname, verts, normals, faces = LoadObj(infile)
    verts = (verts*127 / np.max(verts)).astype(np.int8)
    normal_scale = np.max(normals) / 127.0
    normals = (normals*127 / np.max(normals)).astype(np.int8)
    # note: object is resized for maximum precision within an 8 bit space
    print 'static const int8_t %s_vertices[] PROGMEM = {' % (objname)
    print ', '.join(map(str, verts.flatten()))
    print '};\n'

    # note: and the normals are as well, which means they are denormalized
    print 'static const float %s_normal_scale = %e;' % (objname, normal_scale)
    print 'static const int8_t %s_normals[] PROGMEM = {' % (objname)
    print ', '.join(map(str, normals.flatten()))
    print '};\n'

    print 'static const uint8_t %s_faces[] PROGMEM = {' % (objname)
    print ', '.join(map(str, faces.flatten()))
    print '};\n'

if __name__ == '__main__':
    ConvertObj(open(sys.argv[1]))
