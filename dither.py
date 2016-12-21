import numpy as np

pattern = np.array([
    [0, 8, 2, 10],
    [12, 4, 14, 6],
    [3, 11, 1, 9],
    [15, 7, 13, 5]])


def bitmasks(pattern, threshold):
    bits = 1 << np.arange(4)
    p = np.dot(pattern < threshold, bits)
    return p + (p << 4)

masks = []
for i in range(17):
    masks.extend(bitmasks(pattern, i))

print ', '.join(["0x%02x" % m for m in masks])
