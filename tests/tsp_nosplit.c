#include "tsp.h"
/* This file contains the main TSP functions */

/* Initialize the TSP algorithm */
void tsp_init() {
	unsigned int i, j, k, weight, current_minimum, second_minimum, min_door1, min_door2;

	/* Initialize variables */
	if(TIMER_ENABLED) start_time = clock(); /* Start timer */
	upper_bound = 0;
	weight = 0;
	absolute_min_distance = UINT_MAX;
	best_process = 0;
	final_action = 'q';
	print_action = 'p';
	if(comm_size > 1) {
		mpi_enabled = 1;
		/* status_receive = MPI_STATUS_NULL;
		status_send = MPI_STATUS_NULL; */
		request_receive = MPI_REQUEST_NULL;
		request_send = MPI_REQUEST_NULL;
		if(comm_rank == 0) {
			request_receive_final = MPI_REQUEST_NULL;
			for(i=0; i< comm_size-1; i++) requests_send[i] = MPI_REQUEST_NULL;
		}
		upper_bound_modified = 0;
	} else mpi_enabled = 0;

	/* Initialize city_array, min_door and double all weights */
	for(i = 0; i < amount_of_cities; i++) {
		min_door1 = UINT_MAX; /* Smallest */
		min_door2 = UINT_MAX; /* Second smallest */
		city_array[i] = i;
		for(j = 0; j < amount_of_cities; j++) {
			if(j==i) continue;
			if(weight_array[i][j] < min_door1) {
				min_door2 = min_door1;
				min_door1 = weight_array[i][j];
			} else if(weight_array[i][j] < min_door2) min_door2 = weight_array[i][j];
			weight_array[i][j] *= 2; /* We do this so we don't have to deal with divisions for min_door */
		}
		weight_array[i][i] = min_door1;
		if(min_door1 < absolute_min_distance) absolute_min_distance = min_door1;
		min_door[i] = min_door1+min_door2;
		weight+=min_door[i];
	}

	/* Now, calculate Held-Karp lower bound */
	held_karp_main = get_held_karp(city_array,upper_bound,0);
	held_karp_current = held_karp_main;

	/* Get upper bound estimate with two-opt */
	upper_bound = get_tour_length(city_array);
	memcpy(best_route, city_array, amount_of_cities*sizeof(unsigned int));
	if(upper_bound == held_karp_main) { tsp_end(); return; } /* Cheap check, better than doing k-opt and then checking. */
	two_opt();
	if(upper_bound == held_karp_main) { tsp_end(); return; }
	three_opt();
	if(upper_bound == held_karp_main) { tsp_end(); return; }
	two_opt();
	if(upper_bound == held_karp_main) { tsp_end(); return; }

	/* Determine the best start node */
	current_minimum = min_door[city_array[0]];
	for(i = 0; i < amount_of_cities; i++) {
		if(min_door[city_array[i]] > current_minimum) {
			current_minimum = min_door[city_array[i]];
			swap(&city_array[i],&city_array[0]);
		}
	}

	/* Sort array (excl. start node) */
	if(SORT == SORT_BY_MIN_DOOR) {
		sort_by_min_door(city_array,0);
	} else if(SORT == SORT_GREEDY) {
		sort_greedy(city_array,0);
	} else if(SORT == SORT_BY_HELD_KARP) {
		sort_by_held_karp(city_array,1);
	}

	if(amount_of_cities >= 60) {
		/* Try 2-opt and 3-opt again for the sorted array */
		/* But first, let's temporarily copy the current best route to i_array which isn't in use yet */
		current_minimum = upper_bound;
		memcpy(i_array, best_route, amount_of_cities*sizeof(unsigned int));
		upper_bound = get_tour_length(city_array);
		memcpy(best_route, city_array, amount_of_cities*sizeof(unsigned int));
		two_opt();
		three_opt();
		two_opt();
		if(upper_bound > current_minimum) {
			/* Sorted array isn't better */
			upper_bound = current_minimum;
			memcpy(best_route, i_array, amount_of_cities*sizeof(unsigned int));
		} else if(upper_bound == held_karp_main) { tsp_end(); return; }

		/* Try random arrays, this not-very-uniform random generator is more than enough */
		/* Keep minimum upper bound in current_minimum and the corresponding best route in i_array  */
		srand(time(NULL)+comm_rank);
		current_minimum = upper_bound;
		memcpy(i_array, best_route, amount_of_cities*sizeof(unsigned int));
		for(i = 0; i < 10+(amount_of_cities/10); i++) {
			for(j = amount_of_cities-1; j > 0; j--) {
				k = rand() % j;
				swap(&best_route[j],&best_route[k]);
			}

			/* Improve this permutation with 2-opt and 3-opt */
			upper_bound = get_tour_length(best_route);
			two_opt();
			three_opt();
			two_opt();

			/* Check if this permutation is better */
			if(upper_bound < current_minimum) {
				current_minimum = upper_bound;
				memcpy(i_array, best_route, amount_of_cities*sizeof(unsigned int));
				upper_bound_modified = 1;
				best_process = comm_rank;
				send_upper_bound();

				/* Check if we've reached the Held-Karp value AFTER we've sent it so other processes will end too */
				if(upper_bound == held_karp_main) { tsp_end(); return; }
			}
		}
		upper_bound = current_minimum;
		memcpy(best_route, i_array, amount_of_cities*sizeof(unsigned int));
	}
	/* Improve lower bound now that we know the start node */
	weight += weight_array[city_array[0]][city_array[0]]*2;
	if(upper_bound == weight) { tsp_end(); return; } /* Weight is the min_door lower bound, and HK is probably better but we should still test */

	/* Determine split depth */
	/* split_depth = amount_of_cities-2; */ /* -2 because we ignore the last node every time */
	/* for(i = 2; i < amount_of_cities; i++) {
		if(split_depth >= (comm_size*comm_size*comm_size*comm_size*2)) break;
		split_depth *= (amount_of_cities-i);
	}
	split_depth = i-1; */

	/* To what depths should the min_door and Held-Karp values be recalculated? */
	min_door_adaptive_depth = amount_of_cities/2;
	held_karp_depth = amount_of_cities-4;

	/* Start the search algorithm with city 0 visited (so amount = 1) */
	if(amount_of_cities < 4) {
		upper_bound = 0;
		for(i=1; i<amount_of_cities;i++) {
			upper_bound += weight_array[i-1][i];
		}
		if(amount_of_cities > 1) upper_bound += weight_array[amount_of_cities-1][0];
		/* Best route is just the normal route. */
	} else {
		/* We separate the first selection step from search_loop/search_recursive to avoid too many if-checks */
		order_first = city_array[1];
		order_second = city_array[amount_of_cities-1];
		order_first_taken = 0;
		weight = 0;
		for(i=1;i<amount_of_cities;i++) {
			/* Keep order so we don't have to check every tour twice */
			if(city_array[i] == order_second) continue;

			/* Split depth? If so, split! */
			if((split_depth == 1) && (((i-1) % comm_size) != comm_rank)) continue;

			current_weight = weight+weight_array[city_array[0]][city_array[i]];
			if(current_weight+get_min_door(1,i) < upper_bound) {
				/* Put the next city you'll try out on position 1 */
				swap(&city_array[i],&city_array[1]);

				/* Calculate Held-Karp value and only continue if it's small enough */
				held_karp_current = current_weight+get_held_karp(city_array,upper_bound,1);
				if(held_karp_current < upper_bound) {
					/* Keep track of which edge you chose */
					i_array[0] = i;

					/* Keep order so we don't have to check every tour twice */
					if(city_array[1] == order_first) order_first_taken = 1;

					/* Choose the right algorithm as defined in tsp.h */
					if(ALGORITHM == LOOP) {
						search_loop(city_array[1],current_weight,2);
					} else if(ALGORITHM == RECURSIVE) search_recursive(city_array[1],current_weight,0,2);

					/* If this is the same as the main Held-Karp value, we are sure to have the optimal tour! */
					if(upper_bound == held_karp_main) break;
				}
				/* Swap back and check the next edge. */
				swap(&city_array[i],&city_array[1]);
 			}
		}
	}
	/* End the TSP algorithm. What, did you expect anything else? */
	tsp_end();
}

/* The loop (non-recursive) version of the branch and bound algorithm */
/* About as fast as the recursive one! It uses goto though. What a shame. */
void search_loop(unsigned int city, unsigned int weight, unsigned int amount_visited) {
	unsigned int i, weight_guess;
	i = 2;
	weight_guess = 0;
start_loop:
	if(amount_visited == amount_of_cities) {
		weight += weight_array[city][city_array[0]];
		if(weight < upper_bound) {
			/* Update upper bound */
			upper_bound=weight;
			memcpy(best_route, city_array, amount_of_cities*sizeof(unsigned int));

			/* Send upper bound */
			upper_bound_modified = 1;
			best_process = comm_rank;
			send_upper_bound();

			/* Try 3-opt and send upper bound again if improved */
			three_opt();
			if(upper_bound < weight) {
				upper_bound_modified = 1;
				send_upper_bound();
			}

			/* If this is the same as the main Held-Karp value, we are sure to have the optimal tour! */
			if(upper_bound == held_karp_main) return;
		}
		/* Backtracking to the previous branch */
		/* No swap necessary here - we automatically chose the last available node in the last step. */
		amount_visited--;
		i = i_array[amount_visited-1];
		city = city_array[amount_visited-1];
		weight -= weight_array[city][city_array[i]]+weight_array[city_array[i]][city_array[0]];
		weight_guess += min_door[city_array[i]]+weight_array[city][city]-weight_array[city_array[i]][city_array[i]];
		i++;
	}
continue_loop:
	/* i has already been initialized, so no need to do so here. */
	for(;i<amount_of_cities;i++) {
		/* Keep order so we don't have to check every tour twice */
		if((city_array[i] == order_second) && (order_first_taken == 0)) continue;

		/* Split depth? If so, split! */
		if(amount_visited == split_depth) {
			i_array[amount_visited-1] = i;
			if(calculate_procnumber()!=comm_rank) continue;
		}

		/* Calculate weight so far + min_door lower bound with edge forced in */
		current_weight = weight+weight_array[city][city_array[i]];
		if(amount_visited > min_door_adaptive_depth) {
			current_weight_guess = weight_guess+weight_array[city_array[i]][city_array[i]]-min_door[city_array[i]]-weight_array[city][city];
		} else current_weight_guess = get_min_door(amount_visited,i);

		if(current_weight+current_weight_guess < upper_bound) {
			/* Do MPI stuff */
			receive_upper_bound();
			if(upper_bound == held_karp_main) return;
			send_upper_bound();

			/* Keep track of which edge you chose where and put that city on city_array[amount_visited] */
			i_array[amount_visited-1] = i;
			swap(&city_array[i],&city_array[amount_visited]);

			/* Calculate Held-Karp value and if it's too large, swap back and continue */
			if(amount_visited <= held_karp_depth) {
				held_karp_current = current_weight+get_held_karp(city_array,upper_bound,amount_visited);
				if(held_karp_current >= upper_bound) {
					swap(&city_array[i],&city_array[amount_visited]);
					continue;
				}
			}

			/* Keep order so we don't have to check every tour twice */
			city = city_array[amount_visited];
			if((order_first_taken == 0) && (city_array[amount_visited] == order_first)) order_first_taken = amount_visited;

			/* Force this edge in and try to find a better optimum. Change all variables necessary. */
			amount_visited++;
			weight = current_weight;
			weight_guess = current_weight_guess;
			i = amount_visited;
			goto start_loop;
		}
	}
	/* Backtracking to the previous branch */
	amount_visited--;
	i = i_array[amount_visited-1];
	city = city_array[amount_visited-1];
	swap(&city_array[i],&city_array[amount_visited]);
	weight -= weight_array[city][city_array[i]];
	if(amount_visited > min_door_adaptive_depth) {
		weight_guess += min_door[city_array[i]]+weight_array[city][city]-weight_array[city_array[i]][city_array[i]];
	}
	i++;

	/* Keep order so we don't have to check every tour twice */
	if(order_first_taken == amount_visited) order_first_taken = 0;

	/* Should the loop go on or should we go back to the first selection step in tsp_init()? */
	if(amount_visited > 1) goto continue_loop;
}

/* The recursive (non-loop) version of the branch and bound algorithm */
void search_recursive(unsigned int city, unsigned int weight, unsigned int weight_guess, unsigned int amount_visited) {
	if(amount_visited == amount_of_cities) {
		weight += weight_array[city][city_array[0]];
		if(weight < upper_bound) {
			/* Update upper bound */
			upper_bound=weight;
			memcpy(best_route, city_array, amount_of_cities*sizeof(unsigned int));

			/* Send upper bound */
			upper_bound_modified = 1;
			best_process = comm_rank;
                        send_upper_bound();

			/* Try 3-opt and send upper bound again if improved */
			three_opt();
			if(upper_bound < weight) {
				upper_bound_modified = 1;
				send_upper_bound();
			}

			/* If this is the same as the main Held-Karp value, we are sure to have the optimal tour! */
                        if(upper_bound == held_karp_main) return;
		}
	} else {
		unsigned int i;
		for(i=amount_visited;i<amount_of_cities;i++) {
			/* Keep order so we don't have to check every tour twice */
			if((city_array[i] == order_second) && (order_first_taken == 0)) continue;

			/* Split depth? If so, split! */
			if(amount_visited == split_depth) {
				i_array[amount_visited-1] = i;
				if(calculate_procnumber()!=comm_rank) continue;
			}

			/* Calculate weight so far + min_door lower bound with edge forced in */
			current_weight = weight+weight_array[city][city_array[i]];
			if(amount_visited > min_door_adaptive_depth) {
				current_weight_guess = weight_guess+weight_array[city_array[i]][city_array[i]]-min_door[city_array[i]]-weight_array[city][city];
			} else current_weight_guess = get_min_door(amount_visited,i);

			if(current_weight+current_weight_guess < upper_bound) {
				/* Do MPI stuff */
				receive_upper_bound();
				if(upper_bound == held_karp_main) return;
				send_upper_bound();

				/* Keep track of which edge you chose where and put that city on city_array[amount_visited] */
				i_array[amount_visited-1] = i;
				swap(&city_array[i],&city_array[amount_visited]);

				/* Calculate Held-Karp value and if it's too large, swap back and continue */
				if(amount_visited <= held_karp_depth) {
					held_karp_current = current_weight+get_held_karp(city_array,upper_bound,amount_visited);
					if(held_karp_current >= upper_bound) {
						swap(&city_array[i],&city_array[amount_visited]);
						continue;
					}
				}

				/* Keep order so we don't have to check every tour twice */
				if((order_first_taken == 0) && (city_array[amount_visited] == order_first)) order_first_taken = amount_visited;

				/* Force this edge in and try to find a better optimum, then swap back */
				search_recursive(city_array[amount_visited],current_weight,current_weight_guess,amount_visited+1);
				if(upper_bound == held_karp_main) return;
				swap(&city_array[i],&city_array[amount_visited]);

				/* Keep order so we don't have to check every tour twice */
				if(order_first_taken == amount_visited) order_first_taken = 0;
			}
		}
	}
}

/* End the TSP algorithm */
void tsp_end() {
	/* MPI - Send Help Offer */
	/* Not implemented yet! */

	/* MPI - Send Best Distance */
	send_receive_best_tour();
	if(!mpi_enabled) print_best_tour();
}

/* Reconstruct the best tour and print it (and end the timer) */
void print_best_tour() {
	unsigned int i;

	/* Divide end result by 2 (we multiplied all weights by 2 before) */
	upper_bound /= 2;

	/* Reconstruct best route starting from node 0 */
	if(best_route[0] != 0) {
		i = 1;
		while(best_route[i] != 0) i++;
		memcpy(city_array,&best_route[i],(amount_of_cities-i)*sizeof(unsigned int));
		memcpy(&city_array[amount_of_cities-i],best_route,i*sizeof(unsigned int));
		memcpy(best_route, city_array, amount_of_cities*sizeof(unsigned int));
	}

	/* Get total time and print it along with the best process */
	if(TIMER_ENABLED) {
		end_time = clock();
		total_time = (unsigned long)((end_time - start_time)/(CLOCKS_PER_SEC/1000));
		printf("Process number: %u | Time: %lu\n",comm_rank,total_time);
	}

	/* Print result */
	printf("%u\n",upper_bound);
	for(i = 0; i < amount_of_cities; i++) printf("%u ",best_route[i]);
	printf("0\n");
}
