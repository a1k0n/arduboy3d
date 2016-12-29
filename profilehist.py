import sys
import numpy as np
from collections import defaultdict


def DumpHistogram(h):
    f = open("hist.txt", 'w')
    for addr in sorted(h.keys()):
        print >>f, hex(addr), h[addr]
    f.close()


def CollectSamples(infile):
    histogram = defaultdict(int)
    checkpointctr = 0
    while True:
        buf = infile.read(32)
        if len(buf) < 32:
            break
        addrs = np.frombuffer(buf, np.uint16)
        # stackframe = []
        for addr in addrs:
            # for now, ignore stack frames and just add a single sample for each
            # if addr & 0x8000:
            #     stackframe.append((addr & 0x7fff) * 2)
            # else:
            histogram[(addr & 0x7fff) * 2] += 1
        checkpointctr += 1
        if checkpointctr > 100:
            DumpHistogram(histogram)
            checkpointctr = 0
    return histogram


if __name__ == '__main__':
    CollectSamples(open(sys.argv[1], 'rb'))
