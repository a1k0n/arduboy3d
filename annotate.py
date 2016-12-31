import sys

'''
annotate an objdump disassembly with profile histogram information
avr-objdump -SlC $TMPDIR/arduino_build_xxx/arduboy3d.ino.elf | python annotate.py hist.txt
'''

samples = {}
maxsample = 0
for line in open(sys.argv[1]):
    line = line.strip().split()
    addr = int(line[0][:-1], 16)
    n = int(line[1])
    samples[addr] = n
    maxsample = max(maxsample, n)


for line in sys.stdin:
    line = line.rstrip()
    if line[8:10] == ":\t":
        addr = int(line[:8], 16)
        if addr in samples:
            graphsiz = min(60, 200*samples[addr] / maxsample)
            print line[:10], '\t\t\t\t\t\t', '#' * (graphsiz), samples[addr]
    print line
