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

#define HEADER_LINES 6
#define NUM_CITIES 13510
#define NUM_DIMS 2

int num_runs = 1;

int** city; // city[id][0] = x of city id, city[id][1] = y of city id,
int** city_dis; // distance between city, city_dis[id1][id2] = distance(id1,id2)
int city_count;

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
	for (i = 0; i < lastIdx; i++)
		tot_dis += city_dis[city[i]][city[i + 1]]; // this reduction is memory bound
	tot_dis += city_dis[city[0]][city[lastIdx]];	// dist for tail ~ head
	return tot_dis;
}

int *gen_neighbor(int *a, int i, int j)
{ // swap the order of ith~jth city of array a and return
	int tmp, k;
	int *neighbor = (int *)malloc(sizeof(int) * city_count);
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
	int *start_state = (int *)malloc(sizeof(int) * city_count);
	bool *gened = (bool *)malloc(sizeof(bool) * city_count);
	memset(gened, 0, sizeof(bool) * city_count);
	int gen_count = 0, gen_id;
	while (gen_count < city_count) {
		gen_id = rand() % city_count;
		while (gened[gen_id])
			if (++gen_id == city_count)
				gen_id = 0;
		gened[gen_id] = 1;
		start_state[gen_count++] = gen_id;
	}
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
	city = (int**)malloc(sizeof(int*)*NUM_CITIES);
	city_dis = (int**)malloc(sizeof(int*)*NUM_CITIES);
	for (i = 0; i < NUM_CITIES; i++) {
		city[i] = (int*)malloc(sizeof(int)*NUM_DIMS);
		city_dis[i] = (int*)malloc(sizeof(int)*NUM_CITIES);
	}

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

				_mm_store_ps(&temp[0], d8);  // store sum into an array
				_mm_store_ps(&temp[4], d14); // store sum into an array

				city_dis[lastCity][i] = city_dis[i][lastCity] = round(temp[0]);
				city_dis[lastCity][i+1] = city_dis[i+1][lastCity] = round(temp[1]);
				city_dis[lastCity][i+2] = city_dis[i+2][lastCity] = round(temp[2]);
				city_dis[lastCity][i+3] = city_dis[i+3][lastCity] = round(temp[3]);
				city_dis[lastCity][i+4] = city_dis[i+4][lastCity] = round(temp[4]);
				city_dis[lastCity][i+5] = city_dis[i+5][lastCity] = round(temp[5]);
				city_dis[lastCity][i+6] = city_dis[i+6][lastCity] = round(temp[6]);
				city_dis[lastCity][i+7] = city_dis[i+7][lastCity] = round(temp[7]);
			}

			// Handle the rest
			for (; i < lastCity - 4; i += 4) {
				float temp[4];
				__m128 d1, d2, d3, d4, d5, d6, d7, d8, d9, d10;

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

				_mm_store_ps(&temp[0], d10); // store sum into an array

				city_dis[lastCity][i] = city_dis[i][lastCity] = round(temp[0]);
				city_dis[lastCity][i+1] = city_dis[i+1][lastCity] = round(temp[1]);
				city_dis[lastCity][i+2] = city_dis[i+2][lastCity] = round(temp[2]);
				city_dis[lastCity][i+3] = city_dis[i+3][lastCity] = round(temp[3]);
			}
			for (; i < lastCity; i++) {
				city_dis[lastCity][i] = city_dis[i][lastCity] = distance(i, lastCity);
			}
		}
	}

	double init = omp_get_wtime();
	int counter, distance, min = INT_MAX;
	float tot = 0;

	printf("Final distances:");
	#pragma omp parallel for shared(min,tot) private(distance)
	for (counter = 0; counter < num_runs; counter++) {
		distance = hill();
		printf(" %d", distance);

		if (distance < min)
			min = distance;
		tot += (float)distance;
	}
	double end = omp_get_wtime();
	printf("\nMin: %d Avg: %f\n", min, tot / num_runs);
	printf("Total initialization time: %f\n", (double)(init - start));
	printf("Total execution time: %f\n", (double)(end - start));

	return 0;
}
