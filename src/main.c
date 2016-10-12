#include "tsp.h"
/* This file contains the main function of the program */

int main(int argc, char * argv[]) {
	FILE *input_file;
	unsigned int i, j;
	
	/* Initialize MPI */
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

	/* Open file */
	if (argc != 2) {
		if(comm_rank == 0) printf("Error: wrong number of arguments! | Usage: tsp input.txt\n");
		MPI_Finalize();
		return EXIT_FAILURE;
	}
	input_file = fopen(argv[1], "r");
	if (input_file == NULL) {
		if(comm_rank == 0) printf("Error opening file %s | Usage: tsp input.txt\n", argv[1]);
		MPI_Finalize();
		return EXIT_FAILURE;
	}

	/* Read input and exit on incorrect input (using %d skips whitespace) */
	if(fscanf(input_file,"%d",&amount_of_cities) == EOF) {
		if(comm_rank == 0) printf("Incorrect input in file %s | Usage: tsp input.txt\n", argv[1]);
		fclose(input_file);
		MPI_Finalize();
		return EXIT_FAILURE;
	}

	/* Initialize weight array */
	weight_array = (unsigned int**) malloc(amount_of_cities*sizeof(unsigned int*));
	cost_array = (double**) malloc(amount_of_cities*sizeof(double*));
	for (i = 0; i < amount_of_cities; i++) {
		weight_array[i] = (unsigned int*) malloc(amount_of_cities*sizeof(unsigned int));
		cost_array[i] = (double*) malloc(amount_of_cities*sizeof(double));
		for (j = 0; j < amount_of_cities; j++) {
			if (fscanf(input_file,"%d",&weight_array[i][j]) == EOF) {
				if(comm_rank == 0) printf("Incorrect input in file %s | Usage: tsp input.txt\n", argv[1]);
				fclose(input_file);
				for (j = 0; j <= i; j++) {
					free(weight_array[j]);
					free(cost_array[j]);
				}
				free(weight_array);
				free(cost_array);
				MPI_Finalize();
				return EXIT_FAILURE;
			}
		}
	}
	fclose(input_file);
	
	/* Initialize other arrays */
	city_array = (unsigned int*) malloc(amount_of_cities*sizeof(unsigned int));
	best_route = (unsigned int*) malloc(amount_of_cities*sizeof(unsigned int));
	swap_array = (unsigned int*) malloc(amount_of_cities*sizeof(unsigned int));
	i_array = (unsigned int*) malloc(amount_of_cities*sizeof(unsigned int));
	min_door = (unsigned int*) malloc(amount_of_cities*sizeof(unsigned int));
	tmp_array = (unsigned int*) malloc(amount_of_cities*sizeof(unsigned int));
	degree = (unsigned int*) malloc(amount_of_cities*sizeof(unsigned int));
	penalty = (double*) malloc(amount_of_cities*sizeof(double));
	if(comm_rank == 0) requests_send = (MPI_Request*) malloc((comm_size-1)*sizeof(MPI_Request));

	/* Execute algorithm */
	tsp_init();
	
	/* Free memory */
	for (i = 0; i < amount_of_cities; i++) {
		free(weight_array[i]);
		free(cost_array[i]);
	}
	free(weight_array);
	free(cost_array);
	free(city_array);
	free(best_route);
	free(swap_array);
	free(i_array);
	free(min_door);
	free(tmp_array);
	if(comm_rank == 0) free(requests_send);

	MPI_Finalize();
	return EXIT_SUCCESS;
}
