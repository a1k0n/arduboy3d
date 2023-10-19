import numpy as np
import sys


def LoadObj(infile):
    verts = []
    obj_normals = []
    out_normals = []
    faces = []   # 3 vertex indices

    for line in infile:
        line = line.strip().split()
        if len(line) > 1:
            if line[0] == 'v':  # vertex
                v = np.array([float(x) for x in  line[1:]])
                verts.append(v)
            elif line[0] == 'vn':  # normal
                v = np.array([float(x) for x in line[1:]])
                v /= np.linalg.norm(v)
                obj_normals.append(v)
            elif line[0] == 'f':  # face
                coords = [f.split('/') for f in line[1:]]
                # if they aren't, they either have shared normals
                # or we can just reorder
                faces.append(
                    [int(coords[0][0]) - 1, int(coords[1][0]) - 1,
                     int(coords[2][0]) - 1])
                n = (obj_normals[int(coords[0][2]) - 1]
                     + obj_normals[int(coords[1][2]) - 1]
                     + obj_normals[int(coords[2][2]) - 1])
                n /= np.linalg.norm(n)
                out_normals.append(n)
    verts = np.array(verts)
    normals = np.array(out_normals)
    faces = np.array(faces)
    return verts, normals, faces


def ComputeNormals(verts, faces):
    a = verts[faces[:, 0]]
    b = verts[faces[:, 1]]
    c = verts[faces[:, 2]]
    n = np.cross(b-a, c-a)
    return (n.T / np.linalg.norm(n, axis=1)).T


def MergeVertices(verts, faces):
    ''' merge vertices which have the same position after quantization '''
    vertex_map = {}
    out_vertices = []
    out_faces = []
    for v in verts:
        vkey = (v[0], v[1], v[2])  # tuple-ify
        if vkey not in vertex_map:
            vertex_map[vkey] = len(out_vertices)
            out_vertices.append(v)
    for f in faces:
        vs = []
        for v in f:
            vkey = (verts[v][0], verts[v][1], verts[v][2])
            vs.append(vertex_map[vkey])
        out_faces.append(vs)
    return np.array(out_vertices), np.array(out_faces)


def ConvertObj(infile):
    verts, normals, faces = LoadObj(infile)
    normals = ComputeNormals(verts, faces)  # ignore normals from obj
    objname = 'mesh'  # fixed name for now
    vcenter = (np.max(verts, axis=0) + np.min(verts, axis=0)) / 2.0
    verts -= vcenter
    # get face centroids (for sort ordering) before quantizing
    face_centroid = np.sum(verts[faces], axis=1)
    verts = np.round(verts*127 / np.max(np.linalg.norm(verts, axis=1))).astype(np.int8)
    verts, faces = MergeVertices(verts, faces)

    # sort faces/normals by x,y,z coords
    cfn = list(zip(face_centroid, faces, normals))
    x_sorted = sorted(cfn, key=lambda c: c[0][0])
    faces = np.array([f[1] for f in x_sorted])
    normals = np.array([f[2] for f in x_sorted])
    ci = list(zip(x_sorted, range(len(x_sorted))))  # enumerate x-sorted faces
    # get face indices in x-sorted list for y and z sorts
    y_sorted_idx = [c[1] for c in sorted(ci, key=lambda c: c[0][0][1])]
    z_sorted_idx = [c[1] for c in sorted(ci, key=lambda c: c[0][0][2])]

    normal_scale = np.max(normals) / 127.0
    normals = (normals*127 / np.max(np.abs(normals))).astype(np.int8)
    # note: object is resized for maximum precision within an 8 bit space
    print('static const uint16_t %s_NVERTS = %d;' % (objname, len(verts)))
    print('static const uint16_t %s_NFACES = %d;\n' % (objname, len(faces)))

    print('static const int8_t %s_vertices[] PROGMEM = {  // xyz vertex' % (
        objname))
    print(', '.join(map(str, verts.flatten())))
    print('};\n')

    # note: and the normals are as well, which means they are denormalized
    print('static const float %s_normal_scale = %e;' % (objname, normal_scale))
    print('static const int8_t %s_normals[] PROGMEM = {  // xyz face normal' % (
        objname))
    print(', '.join(map(str, normals.flatten())))
    print('};\n')

    print('static const uint%d_t %s_faces[] PROGMEM = {  // x-sorted vertex index' % (
        (len(verts) > 255) and 16 or 8, objname))
    print(', '.join(map(str, faces.flatten())))
    print('};\n')

    print('static const uint%d_t %s_ysort_faces[] PROGMEM = {  // y-sorted face index' % (
        (len(faces) > 255) and 16 or 8, objname))
    print(', '.join(map(str, y_sorted_idx)))
    print('};\n')

    print('static const uint%d_t %s_zsort_faces[] PROGMEM = {  // z-sorted face index' % (
        (len(faces) > 255) and 16 or 8, objname))
    print(', '.join(map(str, z_sorted_idx)))
    print('};\n')


if __name__ == '__main__':
    ConvertObj(open(sys.argv[1]))
