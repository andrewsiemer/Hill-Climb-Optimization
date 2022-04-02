#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include <omp.h>

#define HEADER_LINES 6
#define NUM_RUNS 20
#define NUM_CITIES 13510
#define NUM_DIMS 2

int** city; // city[id][0] = x of city id, city[id][1] = y of city id,
int** city_dis; // distance between city, city_dis[id1][id2] = distance(id1,id2)
int city_count;

int distance(int a, int b)
{
	int x_dis = city[a][0] - city[b][0];
	int y_dis = city[a][1] - city[b][1];
	return round(sqrt(x_dis * x_dis + y_dis * y_dis));
}

int route_distance(int *city)
{
	int i, tot_dis = 0;
	for (i = 0; i < city_count - 1; i++)
		tot_dis += city_dis[city[i]][city[i + 1]];
	tot_dis += city_dis[city[0]][city[city_count - 1]];	// dist for tail ~ head
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

int neighbor_dis(int *a, int p1, int p2, int ori_part)
{
	int p1_left, p2_right, tmp, ans;
	if (p1 > p2)
	{
		tmp = p1;
		p1 = p2;
		p2 = tmp;
	}
	p1_left = (p1 == 0 ? city_count - 1 : p1 - 1);
	p2_right = (p2 == city_count - 1 ? 0 : p2 + 1);
	return city_dis[a[p1_left]][a[p2]] + city_dis[a[p2_right]][a[p1]] - ori_part;
}

int *get_neighbor(int *a)
{
	int i, j, j_right, k;
	float ori_base;
	for (i = 0; i < city_count - 1; i++)
	{
		ori_base = i == 0 ? city_dis[a[i]][a[city_count - 1]] : city_dis[a[i]][a[i - 1]];
		for (j = i + 1; j < city_count; j++)
			if (j - i < city_count - 2)
			{
				j_right = (j == city_count - 1 ? 0 : j + 1);
				if (neighbor_dis(a, i, j, ori_base + city_dis[a[j]][a[j_right]]) < 0)
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
	while (gen_count < city_count)
	{
		gen_id = rand() % city_count;
		while (gened[gen_id])
			if (++gen_id == city_count)
				gen_id = 0;
		gened[gen_id] = 1;
		start_state[gen_count++] = gen_id;
	}
	return start_state;
}

int hill()
{
	int i, j, ans;
	int *cur_state = get_start_state();
	while (1)
	{
		ans = route_distance(cur_state);
		if ((cur_state = get_neighbor(cur_state)) == NULL)
			break;
	}
	return ans;
}

int main(int argc, char **argv)
{
	clock_t start = clock(), stop;
	srand(time(NULL));
	char line[200];
	int jmp_counter = HEADER_LINES, i, j;
	float x, y;
	city = (int**)malloc(sizeof(int*)*NUM_CITIES);
	city_dis = (int**)malloc(sizeof(int*)*NUM_CITIES);
	for(i = 0; i < NUM_CITIES; i++){
		city[i] = (int*)malloc(sizeof(int)*NUM_DIMS);
		city_dis[i] = (int*)malloc(sizeof(int)*NUM_CITIES);
	}
		
	if (argc < 2)
	{
		printf("format: ./hill filename\nfiles => eil51.tsp, lin105.tsp, pcb442.tsp\n");
		exit(0);
	}
	FILE *f;
	f = fopen(argv[1], "r");
	printf("\n%s\n---------------------\n", argv[1]);
	city_count = 0;
	while (fgets(line, 198, f) != NULL)
	{
		if (strstr(line, "EOF") || line[0] == '\n')
			break;
		else if (jmp_counter-- <= 0) // skip data file header
		{
			sscanf(line, "%*d %E %E", &x, &y);
			city[city_count][0] = (int)x, city[city_count++][1] = (int)y;
			// TODO: change to 1 dimension for speedup
			for (i = 0; i < city_count - 1; i++)
				city_dis[city_count - 1][i] = city_dis[i][city_count - 1] = distance(i, city_count - 1);
		}
	}

	int counter = NUM_RUNS, distance, min = 9999999;
	float tot = 0;
	printf("Final distances: ");
	while (counter--) {
		distance = hill();
		if (distance < min)
			min = distance;
		tot += (float)distance;
		printf(" %d", distance);
	}
	printf("\nmin: %d avg: %f\n", min, tot / 20);
	stop = clock();
	printf("total execution time: %f\n", (double)(stop - start) / (double)CLOCKS_PER_SEC);
	return 0;
}