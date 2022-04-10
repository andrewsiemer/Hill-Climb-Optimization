# Hill Climb Optimization 
The projects goal is to optimize an old homework project that uses Hill climbing to solve the TSP problem.
Our implementation strictly looks as using OpenMP and SIMD to gain speed-up.

There are five example data files in `/data`. The number in each testfile indicate the number of cities.

### The Team
* Yen-Chun Chiu
* Fu-Chi (Scarlett) Ao
* Andrew Siemer

This is a working project for Carnegie Mellon University's 18646::How to Write Fast Code II course.

***

## Usage
`Usage: ./hill [-h] -f <datafile> -r <num_runs>`

### Example usage
Using 'eil51.tsp' file and running 20 times.
```sh
make
./hill -f data/eil51.tsp -r 20
```

## Run All Tests
To compile, run all tests, and calculate total time/speed-up use:
```sh
bash run_all.sh
```

## Baseline
The baseline refers to the total execution time of 20 runs of each data file before improvements made.

```sh
============= hill climbing =============
running each 20 times...

data/eil51.tsp
---------------------
Final distances: 445 437 467 441 448 453 442 449 439 448 451 459 449 431 465 446 447 464 447 450
min: 431 avg: 448.899994
total execution time: 0.010000

data/lin105.tsp
---------------------
Final distances: 15430 14888 15139 15325 15039 14880 15016 15396 15421 15824 15890 16030 14930 15272 15441 15568 15386 15380 15509 15408
min: 14880 avg: 15358.599609
total execution time: 0.050000

data/pcb442.tsp
---------------------
Final distances: 54253 55988 55376 54161 54767 56328 55967 54988 53948 56613 55423 54185 55311 55626 55277 56067 54802 54917 54334 53989
min: 53948 avg: 55116.000000
total execution time: 4.970000

data/pr2392.tsp
---------------------
Final distances: 408568 414040 411081 414499 414497 411914 416457 411809 418314 413042 414371 409176 411185 411861 418526 415912 417090 412908 415669 410558
min: 408568 avg: 413573.843750
total execution time: 2687.710000

================ results ================
TOTAL EXECTUTION TIME: 2692.740000
TOTAL SPEED-UP: x1.0
=========================================
```
