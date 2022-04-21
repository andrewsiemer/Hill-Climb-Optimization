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

### Usage example
Using 'eil51.tsp' file and running 20 times.
```sh
make
./hill -f data/eil51.tsp -r 20
```

### Reference usage example
Using 'eil51.tsp' file and running 20 times using the baseline reference.
```sh
./hill-ref -f data/eil51.tsp -r 20
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
total execution time: 2692.740000
total speed-up: x1.0
=========================================
```

## Current 
```
============= hill climbing =============
running each 20 times...

data/eil51.tsp
---------------------
Final distances: 447 443 434 449 443 448 447 431 449 448 456 446 441 433 437 453 446 441 438 464
min: 431 avg: 444.700012
total execution time: 0.017052

data/lin105.tsp
---------------------
Final distances: 15487 14697 15212 15545 14819 14997 14976 15324 15263 14818 15709 15443 15107 15849 15099 15143 15293 15210 15265 14680
min: 14680 avg: 15196.799805
total execution time: 0.025614

data/pcb442.tsp
---------------------
Final distances: 55047 54825 56740 56166 54975 54257 55185 55421 55429 54153 54827 56074 55764 54503 55140 55315 55696 56385 54755 55680
min: 54153 avg: 55316.851562
total execution time: 0.960872

data/pr2392.tsp
---------------------
Final distances: 415267 415092 415559 412751 420700 413611 411289 415553 415182 417412 413751 411944 413513 416989 422692 412902 416630 414334 413190 416453
min: 411289 avg: 415240.687500
total execution time: 674.889279

================ results ================
total execution time: 675.892817
total speed-up: x3.98397487334149313795
=========================================
```