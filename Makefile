all:
	gcc -fopenmp hill.c -O3 -o hill -lm -mno-avx -msse2 -msse3 -msse4 -msse4.1 -msse4.2 -O1
clean:
	rm hill
