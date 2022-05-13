#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include <omp.h>
#include <immintrin.h>
#include <assert.h>
#include <curand_kernel.h>

#define HEADER_LINES 6
#define NUM_CITIES 13510
#define NUM_DIMS 2

// CUR_THREAD_NUM should be >= num_runs's next power of 2
#define CUR_THREAD_NUM 256
// The total amount of shared memory per block
#define MAX_SHARED_MEM_SIZE_PER_BLOCK 49152

#define msg(format, ...) do { fprintf(stderr, format, ##__VA_ARGS__); } while (0)
#define err(format, ...) do { fprintf(stderr, format, ##__VA_ARGS__); exit(1); } while (0)

#define malloc2D(name, xDim, yDim, type) do {             \
	name = (type **)malloc(xDim * sizeof(type *));          \
	assert(name != NULL);                                   \
	name[0] = (type *)malloc(xDim * yDim * sizeof(type));   \
	assert(name[0] != NULL);                                \
	size_t i;                                               \
	for (i = 1; i < xDim; i++)                              \
		name[i] = name[i-1] + yDim;                           \
} while (0)

int num_runs = 1;

int** city;     // city[id][0] = x of city id, city[id][1] = y of city id,
int** city_dis; // distance between city, city_dis[id1][id2] = distance(id1,id2)
int city_count;
int city_count_int_size;
int city_count_bool_size;

__attribute__((always_inline)) inline void checkCuda(cudaError_t e) {
	if (e != cudaSuccess) {
		err("CUDA Error:%s\n", cudaGetErrorString(e));
	}
}

__attribute__((always_inline)) inline void checkLastCudaError() {
	checkCuda(cudaGetLastError());
}

__attribute__((always_inline)) static inline int powerOfTwo(int n) {
	n--;

	n = n >>  1 | n;
	n = n >>  2 | n;
	n = n >>  4 | n;
	n = n >>  8 | n;
	n = n >> 16 | n;
	// n = n >> 32 | n;    //  For 64-bit ints

	return n;
}

__attribute__((always_inline)) inline int distance(int a, int b)
{
	int x_dis = city[a][0] - city[b][0];
	int y_dis = city[a][1] - city[b][1];
	return round(sqrt(x_dis * x_dis + y_dis * y_dis));
}

__attribute__((always_inline)) __device__ inline int route_distance(int *city, int *city_dis, int city_count)
{
	int i, tot_dis = 0;
	int lastIdx = city_count - 1;

	for (i = 0; i < lastIdx - 4; i += 4) {
		// this reduction is memory bound
		tot_dis += (city_dis[city[i] * city_count + city[i + 1]] + city_dis[city[i + 1] * city_count + city[i + 2]]
			+ city_dis[city[i + 2] * city_count + city[i + 3]] + city_dis[city[i + 3] * city_count + city[i + 4]]);
	}
	for (; i < lastIdx; i++)
		tot_dis += city_dis[city[i] * city_count + city[i + 1]];

	tot_dis += city_dis[city[0] * city_count + city[lastIdx]];	// dist for tail ~ head
	return tot_dis;
}

__attribute__((always_inline)) __device__ inline
void gen_neighbor(int *a, int *new_a, int i, int j, int *city_dis, int city_count)
{ // swap the order of ith~jth city of array a and return
	int tmp, k;
	for (tmp = 0; tmp < i; tmp++)
		new_a[tmp] = a[tmp];
	for (tmp = j + 1; tmp < city_count; tmp++)
		new_a[tmp] = a[tmp];
	for (k = 0; k <= j - i; k++) {
		new_a[i + k] = a[j - k];
	}
}

__attribute__((always_inline)) __device__ inline
int neighbor_dis(int *a, int p1, int p2, int ori_part, int *city_dis, int city_count)
{
	int p1_left, p2_right, tmp;
	if (p1 > p2) {
		tmp = p1;
		p1 = p2;
		p2 = tmp;
	}
	int lastCity = city_count - 1;
	p1_left = p1 - 1;
	if (p1 == 0)
		p1_left = lastCity;
	p2_right = p2 + 1;
	if (p2 == lastCity)
		p2_right = 0;

	return city_dis[a[p1_left] * city_count + a[p2]] + city_dis[a[p2_right] * city_count + a[p1]] - ori_part;
}

__device__ bool get_neighbor(int *a, int *new_a, int *city_dis, int city_count)
{
	int i, j, j_right;

	int lastIdx = city_count - 1;
	int beforeLastIdx = city_count - 2;
	float ori_base = city_dis[a[0] * city_count + a[lastIdx]];

	for (i = 0; i < 2;) {
		for (j = i + 1; j < city_count; j++) {
			if (j - i < beforeLastIdx) {
				j_right = j + 1;
				if (j == lastIdx) {
					j_right = 0;
				}
				if (neighbor_dis(a, i, j, ori_base + city_dis[a[j] * city_count + a[j_right]], city_dis, city_count) < 0) {
					gen_neighbor(a, new_a, i, j, city_dis, city_count);
					return true;
				}
			}
		}
		i++;
		ori_base = city_dis[a[i] * city_count + a[i - 1]];
	}

	for (i = 2; i < lastIdx; i++) {
		ori_base = city_dis[a[i] * city_count + a[i - 1]];
		for (j = i + 1; j < lastIdx; j++) {
			if (j - i < beforeLastIdx) {
				if (neighbor_dis(a, i, j, ori_base + city_dis[a[j] * city_count + a[j+1]], city_dis, city_count) < 0) {
					gen_neighbor(a, new_a, i, j, city_dis, city_count);
					return true;
				}
			}
		}
		if (neighbor_dis(a, i, j, ori_base + city_dis[a[j]* city_count + a[0]], city_dis, city_count) < 0) {
			gen_neighbor(a, new_a, i, j, city_dis, city_count);
			return true;
		}
	}
	return false;
}

__attribute__((always_inline)) inline __device__
int hill(int *city_dis, int city_count, curandState *state)
{
	int ans;
	int start_state[NUM_CITIES];
	int swapped_state[NUM_CITIES];
	bool gened[NUM_CITIES] = {0};
	int gen_count = 0, gen_id;

	while (gen_count < city_count) {
		gen_id = curand(state) % city_count;
		while (gened[gen_id])
			if (++gen_id == city_count)
				gen_id = 0;
		gened[gen_id] = 1;
		start_state[gen_count++] = gen_id;
	}
	int *cur_state = &start_state[0];
	int *new_state = &swapped_state[0];

	while (1) {
		ans = route_distance(cur_state, city_dis, city_count);
		if (!get_neighbor(cur_state, new_state, city_dis, city_count)) {
			break;
		}
		// Swap arrays
		int *temp_ptr = cur_state;
		cur_state = new_state;
		new_state = temp_ptr;
	}

	return ans;
}

__global__ static void calculate_city_distances(
	int cities_count,
	int* dev_cities,      // city[id][0] = x of city id, city[id][1] = y of city id,
	int* dev_cities_dis)  // distance between city, city_dis[id1][id2] = distance(id1,id2)
{
	extern __shared__ int sharedMemory[];
	int *city = (int *)sharedMemory;
	int tid = threadIdx.x;
	int gtid = blockIdx.x * blockDim.x + tid;

	for (int i = tid; i < cities_count*NUM_DIMS; i += blockDim.x) {
		city[i] = dev_cities[i];
	}
	__syncthreads();

	if (gtid < cities_count) {
		// city_dis[city_count - 1][i] = city_dis[i][city_count - 1] = distance(i, city_count - 1);
		for (int i = 0; i < gtid; ++i) {
			int base_p1 = NUM_DIMS*i;
			int base_p2 = NUM_DIMS*gtid;
			int x_dis = city[base_p1] - city[base_p2];
			int y_dis = city[base_p1 + 1] - city[base_p2 + 1];
			int dist = round(sqrtf(x_dis * x_dis + y_dis * y_dis));

			dev_cities_dis[cities_count*gtid + i] = dev_cities_dis[cities_count*i + gtid] = dist;
		}
	}
}

__global__ static void do_hill_climbing_shared(
	int num_runs,
	int city_count,				 // Total # of cities
	int first_step,
	int *dev_cities_dis)   // distance between city, city_dis[id1][id2] = distance(id1,id2)
{
	extern __shared__ int sharedMemory[];
	int *distances = (int *)sharedMemory;
	int *city_dist = (int *)sharedMemory + num_runs;

	for (int i = threadIdx.x; i < city_count*city_count; i += blockDim.x) {
		city_dist[i] = dev_cities_dis[i];
	}
	 __syncthreads();

	if (threadIdx.x < num_runs) {
		curandState rng_states;
		curand_init(clock64(), threadIdx.x, 0, &rng_states);

		int local_distance = hill(city_dist, city_count, &rng_states);
		printf(" %d", local_distance);
		distances[threadIdx.x] = local_distance;
	}

	__syncthreads();

	if (threadIdx.x == 0) {
		distances[num_runs] = 0;  // Store the reduction result
	}
	for (unsigned int s = first_step; s > 0; s >>= 1) {
		if (threadIdx.x < s) {
			int dist = 0, sum = distances[threadIdx.x];
			int idx = threadIdx.x + s;
			if (idx < num_runs) {
				dist = distances[idx];
				if (dist < distances[threadIdx.x]) {
					distances[threadIdx.x] = dist;
				}
			}
			if (s == first_step) {
				atomicAdd(&distances[num_runs], dist + sum);
			}
		}
		__syncthreads();
	}

	if (threadIdx.x == 0) {
		printf("\nMin: %d Avg: %f\n", distances[0], (double) distances[num_runs] / num_runs);
	}
}

__global__ static void do_hill_climbing(
	int num_runs,
	int city_count,				 // Total # of cities
	int first_step,
	int *dev_cities_dis)   // distance between city, city_dis[id1][id2] = distance(id1,id2)
{
	extern __shared__ int sharedMemory[];
	int *distances = (int *)sharedMemory;
	int *city_dist = dev_cities_dis;

	if (threadIdx.x < num_runs) {
		curandState rng_states;
		curand_init(clock64(), threadIdx.x, 0, &rng_states);
		int local_distance = hill(city_dist, city_count, &rng_states);
		printf(" %d", local_distance);
		distances[threadIdx.x] = local_distance;
	}

	__syncthreads();

	if (threadIdx.x == 0) {
		distances[num_runs] = 0;  // Store the reduction result
	}
	for (unsigned int s = first_step; s > 0; s >>= 1) {
		if (threadIdx.x < s) {
			int dist = 0, sum = distances[threadIdx.x];
			int idx = threadIdx.x + s;
			if (idx < num_runs) {
				dist = distances[idx];
				if (dist < distances[threadIdx.x]) {
					distances[threadIdx.x] = dist;
				}
			}
			if (s == first_step) {
				atomicAdd(&distances[num_runs], dist + sum);
			}
		}
		__syncthreads();
	}

	if (threadIdx.x == 0) {
		printf("\nMin: %d Avg: %f\n", distances[0], (double) distances[num_runs] / num_runs);
	}
}

int main(int argc, char **argv)
{
	int optFlag;
	char *file = NULL;
	while ((optFlag = getopt(argc, argv, "f:r:h")) != -1) {
		switch (optFlag) {
		case 'f':
			file = optarg;
			break;
		case 'r':
			num_runs = atoi(optarg);
			break;
		case 'h':
			printf("Usage: ./hill [-h] -f <datafile> -r <num_runs>\n");
			printf("Required:\n");
			printf("  -f <datafile>		Input data file.\n");
			printf("  -r <num>		Number of runs (Default: 1).\n");
			printf("Options:\n");
			printf("  -h              Prints this usage info.\n");
			exit(0);
		default:
			printf("Usage: ./hill [-h] -f <datafile> -r <num_runs>\n");
			exit(0);
		}
	}
	if (!file) {
		printf("Usage: ./hill [-h] -f <datafile> -r <num_runs>\n");
		exit(0);
	}

	double start = omp_get_wtime();
	srand(time(NULL));
	char line[200];
	int jmp_counter = HEADER_LINES;
	float x, y;
	malloc2D(city, NUM_CITIES, NUM_DIMS, int);

	FILE *f;
	f = fopen(file, "r");
	printf("\n%s\n---------------------\n", file);
	city_count = 0;
	while (fgets(line, 198, f) != NULL) {
		if (strstr(line, "EOF") || line[0] == '\n')
			break;
		else if (jmp_counter-- <= 0) { // skip data file header
			sscanf(line, "%*d %E %E", &x, &y);
			city[city_count][0] = (int)x, city[city_count++][1] = (int)y;
		}
	}

	malloc2D(city_dis, city_count, city_count, int);

	const unsigned int cities_size = city_count * NUM_DIMS * sizeof(int);
	const unsigned int city_dis_size = city_count * city_count * sizeof(int);
	const unsigned int cities_quotient = city_count / CUR_THREAD_NUM;
	const unsigned int init_num_blocks = cities_quotient + 1;
	const unsigned int init_block_shared_data_size = cities_size;

	int *dev_cities;     // city[id][0] = x of city id, city[id][1] = y of city id,
	int *dev_cities_dis; // distance between city, city_dis[id1][id2] = distance(id1,id2)

	checkCuda(cudaMalloc(&dev_cities, cities_size));
	checkCuda(cudaMalloc(&dev_cities_dis, city_dis_size));
	checkCuda(cudaMemcpy(dev_cities, city[0], cities_size, cudaMemcpyHostToDevice));
	checkCuda(cudaMemset(dev_cities_dis, 0, city_dis_size));
	calculate_city_distances<<< init_num_blocks, CUR_THREAD_NUM, init_block_shared_data_size >>>(
		city_count, dev_cities, dev_cities_dis);
	cudaDeviceSynchronize(); checkLastCudaError();

	double init = omp_get_wtime();

	city_count_int_size = sizeof(int) * city_count;
	city_count_bool_size = sizeof(bool) * city_count;
	const unsigned int first_step = powerOfTwo(num_runs);
	const unsigned int num_runs_int_size = (num_runs + 1) * sizeof(int); // The last one is for reduction
	const unsigned int hill_block_shared_data_size = city_dis_size + num_runs_int_size;

	printf("Final distances:");
	if (hill_block_shared_data_size <= MAX_SHARED_MEM_SIZE_PER_BLOCK) {
		do_hill_climbing_shared<<< 1, CUR_THREAD_NUM, hill_block_shared_data_size >>>(
			num_runs, city_count, first_step, dev_cities_dis);
	} else {
		do_hill_climbing<<< 1, CUR_THREAD_NUM, num_runs_int_size >>>(
			num_runs, city_count, first_step, dev_cities_dis);
	}
	cudaDeviceSynchronize(); checkLastCudaError();

	double end = omp_get_wtime();
	printf("Total initialization time: %f\n", (double)(init - start));
	printf("Total execution time: %f\n", (double)(end - start));

	checkCuda(cudaFree(dev_cities));
	checkCuda(cudaFree(dev_cities_dis));

	free(city[0]);
	free(city);
	free(city_dis[0]);
	free(city_dis);

	return 0;
}
