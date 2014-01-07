#!/usr/bin/env python
import sys

table = [ 2, 3 ]

while True:
    new = table[-1] + table[-2]
    if new > int(sys.argv[1]):
        break;
    table.append(new)

print 'table size:', len(table)

for i in range(0, len(table), 16):
    row = ','.join([str(j) for j in table[i:i+16]])
    print row
