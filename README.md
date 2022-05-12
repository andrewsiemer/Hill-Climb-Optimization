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
============== Hill Climbing (1/1) ==============
Running each dataset 20 times... 

data/eil51.tsp
---------------------
Final distances: 445 437 467 441 448 453 442 449 439 448 451 459 449 431 465 446 447 464 447 450
Min: 431 Avg: 448.899994
Total execution time: 0.010000

data/lin105.tsp
---------------------
Final distances: 15430 14888 15139 15325 15039 14880 15016 15396 15421 15824 15890 16030 14930 15272 15441 15568 15386 15380 15509 15408
Min: 14880 Avg: 15358.599609
Total execution time: 0.050000

data/pcb442.tsp
---------------------
Final distances: 54253 55988 55376 54161 54767 56328 55967 54988 53948 56613 55423 54185 55311 55626 55277 56067 54802 54917 54334 53989
Min: 53948 Avg: 55116.000000
Total execution time: 4.970000

data/pr2392.tsp
---------------------
Final distances: 408568 414040 411081 414499 414497 411914 416457 411809 418314 413042 414371 409176 411185 411861 418526 415912 417090 412908 415669 410558
Min: 408568 Avg: 413573.843750
Total execution time: 2687.710000

================= Results (1/1) =================
Total execution time: 2692.740000
Total speed-up: x1.0
=================================================
```

## Current Best
```
============== Hill Climbing (2/3) ==============
Running each dataset 20 times... 

data/eil51.tsp
---------------------
Final distances: 447 446 438 454 454 443 467 438 446 437 439 441 442 447 448 443 435 455 449 454
Min: 435 Avg: 446.149994
Total initialization time: 0.000238
Total execution time: 0.002116

data/lin105.tsp
---------------------
Final distances: 15632 15043 15644 14942 14515 15187 14821 16101 15358 15071 14798 15541 15046 14973 15130 15527 15923 15882 14734 16097
Min: 14515 Avg: 15298.250000
Total initialization time: 0.000326
Total execution time: 0.006385

data/pcb442.tsp
---------------------
Final distances: 54787 56476 54858 55152 54466 56173 54731 53787 54845 54616 53940 55201 54846 53859 54439 54098 54240 55151 54761 53531
Min: 53531 Avg: 54697.851562
Total initialization time: 0.001442
Total execution time: 0.638123

data/pr2392.tsp
---------------------
Final distances: 417535 419034 410432 415229 414485 415162 410174 412098 414421 418457 413302 414798 413052 410028 413828 418627 414247 409752 412302 411813
Min: 409752 Avg: 413938.812500
Total initialization time: 0.012770
Total execution time: 315.227425

================= Results (2/3) =================
Total execution time: 315.874049
Total speed-up: x8.52472689201511454332
=================================================
```
