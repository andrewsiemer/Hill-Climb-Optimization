#!/bin/bash
make

BASELINE=2692.740000
NUM_RUNS=20
TOTAL_TIME=0

echo "============= hill climbing ============="
echo "running each $NUM_RUNS times..."

cmds=(
    "./hill -f data/eil51.tsp -r $NUM_RUNS"
    "./hill -f data/lin105.tsp -r $NUM_RUNS"
    "./hill -f data/pcb442.tsp -r $NUM_RUNS"
    "./hill -f data/pr2392.tsp -r $NUM_RUNS"
    # "./hill -f data/usa13509.tsp -r $NUM_RUNS"
)

SUB="total execution time: "
for cmd in "${cmds[@]}"; do
    readarray -t lines < <($cmd)
    for line in "${lines[@]}"; do
        echo ${line}
        if [[ "${line}" == *"$SUB"* ]]; then
            TOTAL_TIME=$(echo "$TOTAL_TIME + ${line//$SUB/}" | bc)
        fi
    done
done

SPEEDUP=$(echo "$BASELINE / $TOTAL_TIME" | bc -l)

echo ""
echo "================ results ================"
echo "total execution time: $TOTAL_TIME"
echo "total speed-up: x$SPEEDUP"
echo "========================================="