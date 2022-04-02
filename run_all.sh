#!/bin/bash
make;
echo "========== running hill climbing =========="
./hill data/eil51.tsp
./hill data/lin105.tsp
./hill data/pcb442.tsp
./hill data/pr2392.tsp
./hill data/usa13509.tsp