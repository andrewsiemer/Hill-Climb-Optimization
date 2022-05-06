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

================ Results (1/1) ================
Total execution time: 2692.740000
Total speed-up: x1.0
=================================================
```

## Current Best
```
============== Hill Climbing (3/3) ==============
Running each dataset 20 times... 

data/eil51.tsp
---------------------
Final distances: 456 459 442 436 445 444 435 454 448 434 460 447 447 443 444 443 441 436 436 458
Min: 434 Avg: 445.399994
Total initialization time: 0.015136
Total execution time: 0.015993

data/lin105.tsp
---------------------
Final distances: 14835 15946 15501 14701 15477 14585 15458 15638 15372 15666 14887 15292 15502 15564 15186 14820 14701 15393 15318 15620
Min: 14585 Avg: 15273.099609
Total initialization time: 0.014992
Total execution time: 0.022062

data/pcb442.tsp
---------------------
Final distances: 54539 54081 54724 54926 55134 54623 56072 54930 54297 55327 55981 55326 54640 55038 54611 53366 55960 56063 56536 56257
Min: 53366 Avg: 55121.550781
Total initialization time: 0.015457
Total execution time: 0.685226

data/pr2392.tsp
---------------------
Final distances: 415383 418791 413557 415800 411269 415899 409696 412782 414276 409344 413855 412601 416890 412492 415442 418454 411471 409454 416954 413322
Min: 409344 Avg: 413886.593750
Total initialization time: 0.047818
Total execution time: 339.507847

================ Results (3/3) ================
Total execution time: 340.231128
Total speed-up: x7.91444338391048099514
=================================================
```