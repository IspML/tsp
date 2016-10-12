#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

int main(int argc, char **argv)
{
	unsigned int amount_of_cities, i, j;
	unsigned int **distance_matrix;
	FILE *input_file;

	if (argc != 2) {
		printf("Error: wrong number of arguments! | Usage: tsp_convert_upper input.txt\n");
		return EXIT_FAILURE;
	}
	input_file = fopen(argv[1], "r");
        if (input_file == NULL) {
		printf("Error opening file | Usage: tsp_convert_upper input.txt\n");
		return EXIT_FAILURE;
	}
	if(fscanf(input_file,"%d",&amount_of_cities) == EOF) {
		printf("Incorrect input in file %s | Usage: tsp_convert_upper input.txt\n",argv[1]);
		fclose(input_file);
		return EXIT_FAILURE;
	}

	distance_matrix = (unsigned int**)malloc(amount_of_cities*sizeof(unsigned int*));
	printf("%u\n",amount_of_cities);
	for(i = 0; i < amount_of_cities; i++) distance_matrix[i] =(int*)malloc(amount_of_cities*sizeof(int));
	for(i = 0; i < amount_of_cities; i++) {
		distance_matrix[i][i] = 0;
		for(j = i+1; j < amount_of_cities; j++) {
			if(fscanf(input_file,"%u",&distance_matrix[i][j]) == EOF) {
				printf("Incorrect input in file %s | Usage: tsp_convert_upper input.txt\n",argv[1]);
				fclose(input_file);
				for(i = 0; i < amount_of_cities; i++) free(distance_matrix[i]);
				free(distance_matrix);
				return EXIT_FAILURE;
			}
			distance_matrix[j][i] = distance_matrix[i][j];
		}
	}
	for(i = 0; i < amount_of_cities; i++) {
		for(j = 0; j < amount_of_cities; j++) printf("%u ",distance_matrix[i][j]);
		printf("\n");
		free(distance_matrix[i]);
	}
	free(distance_matrix);
	fclose(input_file);
}
