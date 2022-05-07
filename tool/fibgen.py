#!/usr/bin/env python
import sys

table = [ 16, 24 ]
until = int(sys.argv[1])

while True:
    new = table[-1] + table[-2]
    new += new % 16
    if new > until:
        break;
    table.append(new)

print 'table size:', len(table)

print  ','.join([str(j) for j in table])
