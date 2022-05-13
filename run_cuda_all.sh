#!/bin/bash
make cuda

TOTAL_ITERATION=3
BASELINE=2692.740000
NUM_RUNS=20
TOTAL_TIME=0

cmds=(
    "./hill-cuda -f data/eil51.tsp -r $NUM_RUNS"
    "./hill-cuda -f data/lin105.tsp -r $NUM_RUNS"
    "./hill-cuda -f data/pcb442.tsp -r $NUM_RUNS"
    "./hill-cuda -f data/pr2392.tsp -r $NUM_RUNS"
    # "./hill-cuda -f data/usa13509.tsp -r $NUM_RUNS"
)

min_vals=(  # Known optimal values for each dataset
    "426"
    "14379"
    "50778"
    "378032"
    # "19982859"
)

SUB="Total execution time: "
SUB2="Final distances:"

for ((i=1;i<=$TOTAL_ITERATION;i++));
do
    echo ""
    echo "============== Hill Climbing ($i/$TOTAL_ITERATION) =============="
    echo "Running each dataset $NUM_RUNS times... "
    TIME=0
    for j in "${!cmds[@]}"; do
        readarray -t lines < <(${cmds[$j]})
        for line in "${lines[@]}"; do
            echo ${line}
            if [[ "${line}" == *"$SUB"* ]]; then
                TIME=$(echo "$TIME + ${line//$SUB/}" | bc)
            fi
            if [[ "${line}" == *"$SUB2"* ]]; then
                while IFS=' ' read -ra vals; do
                    for k in "${!vals[@]}"; do
                        if [[ $k -gt 1 ]]; then
                            if [[ ${vals[$k]} < ${min_vals[$j]} ]]; then
                                echo "Error: Invalid output ${vals[$k]} < ${min_vals[$j]}!"
                            fi
                            if [[ ${vals[$k]} == ${min_vals[$j]} ]]; then
                                echo "We've found the optimal minimum path ${vals[$k]}!"
                            fi
                        fi
                    done
                done <<< "${line}"
            fi
        done
    done

    SPEEDUP=$(echo "$BASELINE / $TIME" | bc -l)
    TOTAL_TIME=$(echo "$TOTAL_TIME + $TIME" | bc -l)

    echo ""
    echo "================= Results ($i/$TOTAL_ITERATION) ================="
    echo "Total execution time: $TIME"
    echo "Total speed-up: x$SPEEDUP"
    echo "================================================="
done

AVG_TIME=$(echo "$TOTAL_TIME / $TOTAL_ITERATION" | bc -l)
AVG_SPEEDUP=$(echo "$BASELINE / $AVG_TIME" | bc -l)
echo ""
echo ""
echo "================ Overall Results ================"
echo "Average execution time: $AVG_TIME"
echo "Average speed-up: x$AVG_SPEEDUP"
echo "================================================="
