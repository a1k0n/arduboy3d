import sys
from collections import defaultdict

'''
avr-nm $TMPDIR/arduino_build_xxx/arduboy3d.ino.elf | python histsymbols.py hist.txt
'''

symbols = []
for line in sys.stdin:
    line = line.strip().split()
    # 00800155 B _ZN12Arduboy2Core9cspinmaskE
    if line[1] != 'T':
        continue
    addr = int(line[0], 16)
    sym = line[2]
    symbols.append((addr, sym))

symbols.sort()

print symbols


def lookup(addr):
    for i in range(len(symbols) - 1):
        if addr >= symbols[i][0] and addr < symbols[i+1][0]:
            return symbols[i][1]
    print 'no symbol for address', addr
    return str(addr)

samples = defaultdict(int)
for line in open(sys.argv[1]):
    line = line.strip().split()
    addr = int(line[0][:-1], 16)
    n = int(line[1])
    samples[lookup(addr)] += n

for sym, n in samples.iteritems():
    print n, sym
