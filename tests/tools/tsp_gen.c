#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

int main(int argc, char **argv)
{
	unsigned int amount_of_cities, random_number, max_distance, i, j;
	unsigned int **distance_matrix;
	
	if(argc == 3) {
		amount_of_cities = atoi(argv[1]);
		random_number = atoi(argv[2]);
		max_distance = (INT_MAX/amount_of_cities);
	} else if (argc != 4) {
		printf("Error: wrong number of arguments! | Usage: tsp_gen amount_of_cities random_number [max_distance]\n");
		return EXIT_FAILURE;
	} else {
		amount_of_cities = atoi(argv[1]);
		random_number = atoi(argv[2]);
		max_distance = atoi(argv[3]);
	}
	srand(time(NULL)+random_number);
	
	distance_matrix = (unsigned int**)malloc(amount_of_cities*sizeof(unsigned int*));
	printf("%u\n",amount_of_cities);
	for(i = 0; i < amount_of_cities; i++) distance_matrix[i] =(int*)malloc(amount_of_cities*sizeof(int));
	for(i = 0; i < amount_of_cities; i++) {
		distance_matrix[i][i] = 0;
		for(j = i+1; j < amount_of_cities; j++) {
			random_number = (rand()%max_distance)+1; /* Afstand minstens 1 */
			distance_matrix[i][j] = random_number;
			distance_matrix[j][i] = random_number;
		}
		for(j = 0; j < amount_of_cities; j++) printf("%u ",distance_matrix[i][j]);
		printf("\n");
		free(distance_matrix[i]);
	}
	free(distance_matrix);
}
