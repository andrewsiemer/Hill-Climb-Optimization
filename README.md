To run compile and all tests,
```
bash run_all.sh
```

-------------------- Below are original doc
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

## Usage example
```sh
make
./hill -f data/eil51.tsp -r 20
```

## Baseline
Average tour length, total runtime, best solution out of 20 run

### hill climbing => eil51.tsp
min: 432, avg: 447 
execution time: 0.006962  
* Final distance: 453
* Final distance: 437
* Final distance: 435
* Final distance: 464
* Final distance: 468
* Final distance: 444
* Final distance: 449
* Final distance: 451
* Final distance: 467
* Final distance: 443
* Final distance: 438
* Final distance: 443
* Final distance: 443
* Final distance: 448
* Final distance: 440
* Final distance: 453
* Final distance: 468
* Final distance: 432
* Final distance: 433
* Final distance: 439


### hill climbing => lin105.tsp
min: 14467, avg: 15398 
execution time: 0.073968
* Final distance: 14845
* Final distance: 15112
* Final distance: 14849
* Final distance: 15702
* Final distance: 16110
* Final distance: 15003
* Final distance: 15234
* Final distance: 15874
* Final distance: 15878
* Final distance: 14917
* Final distance: 14919
* Final distance: 15873
* Final distance: 15990
* Final distance: 15661
* Final distance: 15731
* Final distance: 15166
* Final distance: 16100
* Final distance: 15324
* Final distance: 15203
* Final distance: 14467

### hill climbing => pcb442.tsp
min: 53088, avg: 54832 
execution time: 8.057427  
* Final distance: 54804
* Final distance: 54460
* Final distance: 55256
* Final distance: 53901
* Final distance: 54952
* Final distance: 54752
* Final distance: 55957
* Final distance: 55520
* Final distance: 56147
* Final distance: 53088
* Final distance: 55147
* Final distance: 55615
* Final distance: 54908
* Final distance: 56073
* Final distance: 54759
* Final distance: 54258
* Final distance: 54102
* Final distance: 54227
* Final distance: 54841
* Final distance: 53873  
