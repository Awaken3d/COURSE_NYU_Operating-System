#!/bin/bash

LINKER=./Lab01
INS="1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21
22 23 24 25 26 27 28 29 30 31 32 33"

for f in ${INS}; do
    echo "${LINKER} input-${f} > result-${f}"
    ${LINKER} input-${f} > result-${f}
done
