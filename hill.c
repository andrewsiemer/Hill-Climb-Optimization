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

#define HEADER_LINES 6
#define NUM_CITIES 13510
#define NUM_DIMS 2

#define malloc2D(name, xDim, yDim, type) do {               \
	name = (type **)malloc(xDim * sizeof(type *));          \
	assert(name != NULL);                                   \
	name[0] = (type *)malloc(xDim * yDim * sizeof(type));   \
	assert(name[0] != NULL);                                \
	size_t i;                                               \
	for (i = 1; i < xDim; i++)                              \
		name[i] = name[i-1] + yDim;                         \
} while (0)

int num_runs = 1;

int** city;     // city[id][0] = x of city id, city[id][1] = y of city id,
int** city_dis; // distance between city, city_dis[id1][id2] = distance(id1,id2)
int city_count;
int city_count_int_size;
int city_count_bool_size;

__attribute__((always_inline)) inline int distance(int a, int b)
{
	int x_dis = city[a][0] - city[b][0];
	int y_dis = city[a][1] - city[b][1];
	return round(sqrt(x_dis * x_dis + y_dis * y_dis));
}

__attribute__((always_inline)) inline int route_distance(int *city)
{
	int i, tot_dis = 0;
	int lastIdx = city_count - 1;

	for (i = 0; i < lastIdx - 4; i += 4) {
		// this reduction is memory bound
		tot_dis += (city_dis[city[i]][city[i + 1]] + city_dis[city[i + 1]][city[i + 2]]
			+ city_dis[city[i + 2]][city[i + 3]] + city_dis[city[i + 3]][city[i + 4]]);
	}
	for (; i < lastIdx; i++)
		tot_dis += city_dis[city[i]][city[i + 1]];

	tot_dis += city_dis[city[0]][city[lastIdx]];	// dist for tail ~ head
	return tot_dis;
}

int *gen_neighbor(int *a, int i, int j)
{ // swap the order of ith~jth city of array a and return
	int tmp, k;
	int *neighbor = (int *)malloc(city_count_int_size);
	for (tmp = 0; tmp < i; tmp++)
		neighbor[tmp] = a[tmp];
	for (tmp = j + 1; tmp < city_count; tmp++)
		neighbor[tmp] = a[tmp];
	for (k = 0; k <= j - i; k++)
		neighbor[i + k] = a[j - k];
	free(a);
	return neighbor;
}

__attribute__((always_inline)) inline int neighbor_dis(int *a, int p1, int p2, int ori_part)
{
	int p1_left, p2_right, tmp, ans;
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
	return city_dis[a[p1_left]][a[p2]] + city_dis[a[p2_right]][a[p1]] - ori_part;
}

int *get_neighbor(int *a)
{
	int i, j, j_right;

	int lastIdx = city_count - 1;
	int beforeLastIdx = city_count - 2;
	float ori_base = city_dis[a[0]][a[lastIdx]];

	for (i = 0; i < 2;) {
		for (j = i + 1; j < city_count; j++) {
			if (j - i < beforeLastIdx) {
				j_right = j + 1;
				if (j == lastIdx) {
					j_right = 0;
				}
				if (neighbor_dis(a, i, j, ori_base + city_dis[a[j]][a[j_right]]) < 0)
					return gen_neighbor(a, i, j);
			}
		}
		i++;
		ori_base = city_dis[a[i]][a[i - 1]];
	}

	for (i = 2; i < lastIdx; i++) {
		ori_base = city_dis[a[i]][a[i - 1]];
		for (j = i + 1; j < lastIdx; j++) {
			if (j - i < beforeLastIdx) {
				if (neighbor_dis(a, i, j, ori_base + city_dis[a[j]][a[j+1]]) < 0) {
					return gen_neighbor(a, i, j);
				}
			}
		}
		if (neighbor_dis(a, i, j, ori_base + city_dis[a[j]][a[0]]) < 0) {
			return gen_neighbor(a, i, j);
		}
	}
	return NULL;
}

int *get_start_state()
{
	int *start_state = (int *)malloc(city_count_int_size);
	bool *gened = (bool *)malloc(city_count_bool_size);
	memset(gened, 0, city_count_bool_size);
	int gen_count = 0, gen_id;
	while (gen_count < city_count) {
		gen_id = rand() % city_count;
		while (gened[gen_id])
			if (++gen_id == city_count)
				gen_id = 0;
		gened[gen_id] = 1;
		start_state[gen_count++] = gen_id;
	}
	free(gened);
	return start_state;
}

__attribute__((always_inline)) inline int hill()
{
	int ans;
	int *cur_state = get_start_state();
	while (1) {
		ans = route_distance(cur_state);
		if ((cur_state = get_neighbor(cur_state)) == NULL)
			break;
	}
	free(cur_state);
	return ans;
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
	int jmp_counter = HEADER_LINES, i, j;
	float x, y;
	malloc2D(city, NUM_CITIES, NUM_DIMS, int);
	malloc2D(city_dis, NUM_CITIES, NUM_CITIES, int);

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
			int lastCity = city_count - 1;
			for (i = 0; i < lastCity - 8; i += 8) {
				float temp[8];
				__m128 d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11, d12, d13, d14, d15, d16;

				d1 = _mm_setr_ps(city[i][0], city[i+1][0], city[i+2][0], city[i+3][0]);  // (x1,x2,x3,x4)
				d2 = _mm_set_ps1(city[lastCity][0]); // (x999,x999,x999,x999)

				d3 = _mm_setr_ps(city[i][1], city[i+1][1], city[i+2][1], city[i+3][1]);  // (y1,y1,y2,y2)
				d4 = _mm_set_ps1(city[lastCity][1]); // (y999,y999,y999,y999)

				d5 = _mm_sub_ps(d1, d2);  // (x1-x999,x2-x999,x3-x999,x4-x999)
				d1 = _mm_mul_ps(d5, d5);
				d6 = _mm_sub_ps(d3, d4);  // (y1-y999,y2-y999,y3-y999,y4-y999)
				d3 = _mm_mul_ps(d6, d6);
				d7 = _mm_add_ps(d1, d3);
				d8 = _mm_sqrt_ps(d7);

				d9 = _mm_setr_ps(city[i+4][0], city[i+5][0], city[i+6][0], city[i+7][0]);  // (x5,x6,x7,x8)
				d10 = _mm_setr_ps(city[i+4][1], city[i+5][1], city[i+6][1], city[i+7][1]); // (y5,y6,y7,y8)

				d11 = _mm_sub_ps(d9, d2);  // (x5-x999,x6-x999,x7-x999,x8-x999)
				d9 = _mm_mul_ps(d11, d11);
				d12 = _mm_sub_ps(d10, d4); // (y5-y999,y6-y999,y7-y999,y8-y999)
				d10 = _mm_mul_ps(d12, d12);
				d13 = _mm_add_ps(d9, d10);
				d14 = _mm_sqrt_ps(d13);

				d15 = _mm_round_ps(d8, (_MM_FROUND_TO_NEAREST_INT |_MM_FROUND_NO_EXC));
				d16 = _mm_round_ps(d14, (_MM_FROUND_TO_NEAREST_INT |_MM_FROUND_NO_EXC));
				_mm_store_ps(&temp[0], d15);  // store sum into an array
				_mm_store_ps(&temp[4], d16); // store sum into an array

				city_dis[lastCity][i] = city_dis[i][lastCity] = temp[0];
				city_dis[lastCity][i+1] = city_dis[i+1][lastCity] = temp[1];
				city_dis[lastCity][i+2] = city_dis[i+2][lastCity] = temp[2];
				city_dis[lastCity][i+3] = city_dis[i+3][lastCity] = temp[3];
				city_dis[lastCity][i+4] = city_dis[i+4][lastCity] = temp[4];
				city_dis[lastCity][i+5] = city_dis[i+5][lastCity] = temp[5];
				city_dis[lastCity][i+6] = city_dis[i+6][lastCity] = temp[6];
				city_dis[lastCity][i+7] = city_dis[i+7][lastCity] = temp[7];
			}

			// Handle the rest
			for (; i < lastCity - 4; i += 4) {
				float temp[4];
				__m128 d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11;

				d1 = _mm_setr_ps(city[i][0], city[i+1][0], city[i+2][0], city[i+3][0]); // (x1,x2,x3,x4)
				d2 = _mm_set_ps1(city[lastCity][0]); // (x999,x999,x999,x999)

				d3 = _mm_setr_ps(city[i][1], city[i+1][1], city[i+2][1], city[i+3][1]); // (y1,y1,y2,y2)
				d4 = _mm_set_ps1(city[lastCity][1]); // (y999,y999,y999,y999)

				d5 = _mm_sub_ps(d1, d2); // (x1-x999,x2-x999,x3-x999,x4-x999)
				d6 = _mm_sub_ps(d3, d4); // (y1-y999,y2-y999,y3-y999,y4-y999)

				d7 = _mm_mul_ps(d5, d5);
				d8 = _mm_mul_ps(d6, d6);

				d9 = _mm_add_ps(d7, d8);
				d10 = _mm_sqrt_ps(d9);
				d11 = _mm_round_ps(d10, (_MM_FROUND_TO_NEAREST_INT |_MM_FROUND_NO_EXC));

				_mm_store_ps(&temp[0], d11); // store sum into an array

				city_dis[lastCity][i] = city_dis[i][lastCity] = temp[0];
				city_dis[lastCity][i+1] = city_dis[i+1][lastCity] = temp[1];
				city_dis[lastCity][i+2] = city_dis[i+2][lastCity] = temp[2];
				city_dis[lastCity][i+3] = city_dis[i+3][lastCity] = temp[3];
			}
			for (; i < lastCity; i++) {
				city_dis[lastCity][i] = city_dis[i][lastCity] = distance(i, lastCity);
			}
		}
	}

	double init = omp_get_wtime();
	int counter, distance, min = INT_MAX;
	float tot = 0;
	city_count_int_size = sizeof(int) * city_count;
	city_count_bool_size = sizeof(bool) * city_count;

	printf("Final distances:");
	int *distances = malloc(sizeof(int) * num_runs);
	#pragma omp parallel for
	for (counter = 0; counter < num_runs; counter++) {
		int local_distance = hill();
		printf(" %d", local_distance);
		distances[counter] = local_distance;
	}
	for (counter = 0; counter < num_runs; counter++) {
		int d = distances[counter];
		if (d < min)
			min = d;
		tot += d;
	}

	double end = omp_get_wtime();
	printf("\nMin: %d Avg: %f\n", min, tot / num_runs);
	printf("Total initialization time: %f\n", (double)(init - start));
	printf("Total execution time: %f\n", (double)(end - start));

	free(distances);
	free(city[0]);
	free(city);
	free(city_dis[0]);
	free(city_dis);

	return 0;
}
