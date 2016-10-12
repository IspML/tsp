#include "tsp.h"
/* This file contains the help functions used by the TSP algorithm */

/* Swap two values */
void swap(unsigned int* a, unsigned int* b) {
	register unsigned int temp; /* should be kept in a register */
	temp = *a;
	*a = *b;
	*b = temp;
}

/* Reverse part of an array by swapping */
void swap_reverse(unsigned int *tour, unsigned int start, unsigned int end, unsigned int max_tour_length) {
	unsigned int i, amount_of_swaps;
	amount_of_swaps = (end-start+1)/2;
	for(i=0;i<amount_of_swaps;i++) swap(&tour[(start+i)%max_tour_length],&tour[(end-i)%max_tour_length]);
}

/* Execute one 3-opt move */
unsigned int swap_three_opt(unsigned int start, unsigned int middle, unsigned int end) {
	unsigned int i, length, best_length;
	unsigned int trans[4]; /* 0 = swap start-middle with middle-end; 1 = swap start-middle, 2 = swap middle-end, 3 = swap end-start */
	unsigned int best_trans[4];
	best_length = UINT_MAX;
	memcpy(tmp_array,best_route,amount_of_cities*sizeof(unsigned int));

	for(trans[0] = 0; trans[0] < 2; trans[0]++) {
		if(trans[0]) {
			tmp_array[start] = best_route[(middle)%amount_of_cities];
			tmp_array[(start+end-middle-1)%amount_of_cities] = best_route[(end-1)%amount_of_cities];
			tmp_array[(start+end-middle)%amount_of_cities] = best_route[(start)%amount_of_cities];
			tmp_array[(end-1)%amount_of_cities] = best_route[(middle-1)%amount_of_cities];
			middle = start+end-middle;
		}
		for(trans[1] = 0; trans[1] < 2; trans[1]++) {
			if(trans[1]) swap(&tmp_array[start],&tmp_array[(middle-1)%amount_of_cities]);
			for(trans[2] = 0; trans[2] < 2; trans[2]++) {
				if(trans[2]) swap(&tmp_array[middle%amount_of_cities],&tmp_array[(end-1)%amount_of_cities]);
				for(trans[3] = 0; trans[3] < 2; trans[3]++) {
					if(trans[3]) swap(&tmp_array[end%amount_of_cities],&tmp_array[(start+amount_of_cities-1)%amount_of_cities]);
					length = weight_array[tmp_array[(start+amount_of_cities-1)%amount_of_cities]][tmp_array[start]];
					length += weight_array[tmp_array[(middle-1)%amount_of_cities]][tmp_array[middle%amount_of_cities]];
					length += weight_array[tmp_array[(end-1)%amount_of_cities]][tmp_array[end%amount_of_cities]];
					if(length < best_length) {
						best_length = length;
						memcpy(best_trans,trans,4*sizeof(unsigned int));
					}
					if(trans[3]) swap(&tmp_array[end%amount_of_cities],&tmp_array[(start+amount_of_cities-1)%amount_of_cities]);
				}
				if(trans[2]) swap(&tmp_array[middle%amount_of_cities],&tmp_array[(end-1)%amount_of_cities]);
			}
			if(trans[1]) swap(&tmp_array[start],&tmp_array[(middle-1)%amount_of_cities]);
		}
	}
	middle = start+end-middle;
	memcpy(tmp_array,best_route,amount_of_cities*sizeof(unsigned int));
	if(best_trans[0]) {
		for(i = 0; i < end-middle; i++) tmp_array[(start+i)%amount_of_cities] = best_route[(middle+i)%amount_of_cities];
		for(i = 0; i < middle-start; i++) tmp_array[(start+end-middle+i)%amount_of_cities] = best_route[(start+i)%amount_of_cities];
		middle = start+end-middle;
	}
	memcpy(swap_array,tmp_array,amount_of_cities*sizeof(unsigned int));
	if(best_trans[1]) swap_reverse(swap_array,start,middle-1,amount_of_cities);
	if(best_trans[2]) swap_reverse(swap_array,middle,end-1,amount_of_cities);
	if(best_trans[3]) swap_reverse(swap_array,end,start+amount_of_cities-1,amount_of_cities);
	return get_tour_length(swap_array);
}

/* Get tour length */
unsigned int get_tour_length(unsigned int* tour) {
	unsigned int i, length;
	length = 0;
	for(i = 0; i < amount_of_cities-1; i++) {
		length += weight_array[tour[i]][tour[i+1]];
	}
	return length+weight_array[tour[i]][tour[0]];
}

/* Execute two-opt */
void two_opt() {
	if(TWO_OPT_ENABLED) {
		unsigned int i, j, current_minimum, weight_before, weight_after, city_i, city_before_i, city_j, city_after_j;
start_two_opt:
		for(i = 0; i < amount_of_cities; i++) {
			for(j = i+1; j < amount_of_cities+i-1; j++) {
				city_i = best_route[i];
				city_before_i = best_route[(amount_of_cities+i-1)%amount_of_cities];
				city_j = best_route[j%amount_of_cities];
				city_after_j = best_route[(j+1)%amount_of_cities];
				weight_before = weight_array[city_before_i][city_i]+weight_array[city_j][city_after_j];
				weight_after = weight_array[city_before_i][city_j]+weight_array[city_i][city_after_j];
				if(weight_after < weight_before) {
					upper_bound = upper_bound-weight_before+weight_after;
					swap_reverse(best_route,i,j,amount_of_cities);
					goto start_two_opt;
				}
			}
		}
	}
}

/* Execute three-opt */
void three_opt() {
	if(THREE_OPT_ENABLED) {
		unsigned int i, j, k, current_minimum;
start_three_opt:
		for(i = 0; i < amount_of_cities; i++) {
			for(j = i+1; j < amount_of_cities+i-1; j++) {
				for(k = j+1; k < amount_of_cities+i; k++) {
					current_minimum = swap_three_opt(i,j,k);
					if(current_minimum < upper_bound) {
						upper_bound = current_minimum;
						memcpy(best_route,swap_array,amount_of_cities*sizeof(unsigned int));
						goto start_three_opt;
					}
				}
			}
		}
	}
}

/* Sort (excl. start node) a tour array by weight (greedy - take minimum weight edge first) */
void sort_greedy(unsigned int *tour, unsigned int increasing) {
	unsigned int i, j, current_minimum;
	for(i = 0; i < amount_of_cities-1; i++) {
		current_minimum = weight_array[tour[i]][tour[i+1]];
		for(j = i+2; j < amount_of_cities; j++) {
			if((increasing) && (weight_array[tour[i]][tour[j]] < current_minimum)) {
				current_minimum = weight_array[tour[i]][tour[j]];
				swap(&tour[i+1],&tour[j]);
			} else if((!increasing) && (weight_array[tour[i]][tour[j]] > current_minimum)) {
				current_minimum = weight_array[tour[i]][tour[j]];
				swap(&tour[i+1],&tour[j]);
			}
		}
	}
}

/* Sort (excl. start node) a tour by increasing min_door value */
void sort_by_min_door(unsigned int *tour, unsigned int increasing) {
	unsigned int i, j, current_minimum;
	for(i = 0; i < amount_of_cities-1; i++) {
		current_minimum = min_door[tour[i+1]];
		for(j = i+2; j < amount_of_cities; j++) {
			if((increasing) && (min_door[tour[j]] < current_minimum)) {
				current_minimum = min_door[tour[j]];
				swap(&tour[i+1],&tour[j]);
			} else if((!increasing) && (min_door[tour[j]] > current_minimum)) {
				current_minimum = min_door[tour[j]];
				swap(&tour[i+1],&tour[j]);
			}
		}
	}
}

/* Sort (excl. start node) a tour array by the nodes' Held-Karp values with tour[0]-tour[i] forced in */
void sort_by_held_karp(unsigned int *tour, unsigned int increasing) {
	unsigned int i, j, current_minimum;
	unsigned int hk_array[amount_of_cities];
	for(i = 1; i < amount_of_cities; i++) {
		swap(&tour[i],&tour[1]);
		hk_array[tour[1]] = weight_array[tour[0]][tour[1]]+get_held_karp(tour,upper_bound,1);
	}
	for(i = 1; i < amount_of_cities-1; i++) {
		current_minimum = hk_array[tour[i]];
		for(j = i+1; j < amount_of_cities; j++) {
			if((increasing) && (hk_array[tour[j]] < current_minimum)) {
				current_minimum = hk_array[tour[j]];
				swap(&tour[i],&tour[j]);
			} else if((!increasing) && (hk_array[tour[j]] > current_minimum)) {
				current_minimum = hk_array[tour[j]];
				swap(&tour[i],&tour[j]);
			}
		}
	}
}

/* Calculate (adaptive) min_door value */
unsigned int get_min_door(unsigned int amount_visited, unsigned int chosen_index) {
	unsigned int i, j, min_door1, min_door2, weight_guess;
	weight_guess = 0;
	for(i = amount_visited; i < amount_of_cities; i++) {
		if(i==chosen_index) continue;
		min_door1 = UINT_MAX; /* Smallest */
		min_door2 = UINT_MAX; /* Second smallest */
		for(j = amount_visited; j < amount_of_cities; j++) {
			if(j==i) continue;
			if(weight_array[city_array[i]][city_array[j]] < min_door1) {
				min_door2 = min_door1;
				min_door1 = weight_array[city_array[i]][city_array[j]];
			} else if(weight_array[city_array[i]][city_array[j]] < min_door2) min_door2 = weight_array[city_array[i]][city_array[j]];
		}
		if(weight_array[city_array[i]][city_array[0]] < min_door1) {
			min_door2 = min_door1;
			min_door1 = weight_array[city_array[i]][city_array[0]];
		} else if(weight_array[city_array[i]][city_array[0]] < min_door2) min_door2 = weight_array[city_array[i]][city_array[0]];
		weight_array[city_array[i]][city_array[i]] = (min_door1)/2;
		min_door[city_array[i]] = (min_door1+min_door2)/2;
		weight_guess+=min_door[city_array[i]];
	}
	if(amount_visited != amount_of_cities-1) {
		min_door1 = UINT_MAX;
		for(i = amount_visited; i < amount_of_cities; i++) {
			if(i==chosen_index) continue;
			if(weight_array[city_array[chosen_index]][city_array[i]] < min_door1) min_door1 = weight_array[city_array[chosen_index]][city_array[i]];
		}
		if(weight_array[city_array[chosen_index]][city_array[0]] < min_door1) min_door1 = weight_array[city_array[chosen_index]][city_array[0]];
		weight_array[city_array[chosen_index]][city_array[chosen_index]] = (min_door1)/2;
		weight_guess += (min_door1)/2;
		min_door1 = UINT_MAX;
		for(i = amount_visited; i < amount_of_cities; i++) {
			if(weight_array[city_array[0]][city_array[i]] < min_door1) min_door1 = weight_array[city_array[0]][city_array[i]];
		}
		weight_array[city_array[0]][city_array[0]] = (min_door1)/2;
		weight_guess += (min_door1)/2;
	} else {
		weight_guess += weight_array[city_array[chosen_index]][city_array[0]];
	}
	return weight_guess;
}

/* Calculate Held-Karp value */
unsigned int get_held_karp(unsigned int *tour, unsigned int target, unsigned int cities_chosen) {
	double lambda, denom, tmp, lower_bound, best_bound;
	unsigned int i, j, z, current_iteration, best_start;
	lambda = 2;
	tmp = 1;
	z = 2*amount_of_cities;
	current_iteration = 0;

	lower_bound = DBL_MIN;
	best_bound = DBL_MIN;
	for(i = 0; i < amount_of_cities; i++) {
		penalty[i] = 0;
	}
	memcpy(tmp_array, tour, amount_of_cities*sizeof(unsigned int));
	while((lambda > 0.000002)||(-lambda > 0.000002)) {
		/* Reset the degrees of every vertex and calculate the cost_array */
		for(i = 0; i < amount_of_cities; i++) {
			if((i > 0) && (i < cities_chosen)){
				 degree[tour[i]] = 2;
			} else degree[tour[i]] = 0;
			for(j = 0; j < amount_of_cities; j++) cost_array[i][j] = (weight_array[i][j])/2+penalty[i]+penalty[j];
		}

		/* Get the lower bound */
		lower_bound = get_two_nearest(tmp_array,cities_chosen);
		lower_bound += get_mst(tmp_array,cities_chosen);

		for(i = cities_chosen+1; i < amount_of_cities; i++) lower_bound -= 2*penalty[tour[i]];
		if(lower_bound > best_bound) best_bound = lower_bound;

		denom = 0;
		for(i = cities_chosen+1; i < amount_of_cities; i++) {
			tmp = ((double)degree[tour[i]])-2;
			denom += tmp*tmp;
		}
		if(denom == 0) break;
		tmp = lambda * ((((double)target)/2.0)-lower_bound)/denom;
		current_iteration++;
		if(current_iteration > z) {
			current_iteration = 0;
			lambda = lambda / 2.0;
			if(z >= 10) z = z / 2;
		}
		if(tmp == 0) break;
		for(i = 0; i < amount_of_cities; i++) penalty[i] += tmp*(((double)degree[i])-2);
	}
	if(best_bound-round(best_bound) > 0.05) {
		return ((unsigned int)ceil(best_bound))*2;
	} else return ((unsigned int)round(best_bound))*2;
}

/* Get the weight of the two nearest edges (for 1-tree) and record the vertex degrees */
double get_two_nearest(unsigned int *tour, unsigned int cities_chosen) {
	unsigned int i, j, current_first, current_second;
	double current_minimum, second_minimum;

	if(amount_of_cities < 2) return 0;
	if(amount_of_cities == 2) return cost_array[tour[0]][tour[1]];

	current_minimum = DBL_MAX;
	second_minimum = DBL_MAX;

	if(cities_chosen > 0) {
		for(i = cities_chosen+1; i < amount_of_cities; i++) {
			if(cost_array[tour[0]][tour[i]] < current_minimum) {
				current_minimum = cost_array[tour[0]][tour[i]];
				current_first = tour[i];
			}
		}
		for(i = cities_chosen+1; i < amount_of_cities; i++) {
			if(cost_array[tour[cities_chosen]][tour[i]] < second_minimum) {
				second_minimum = cost_array[tour[cities_chosen]][tour[i]];
				current_second = tour[i];
			}
		}
		degree[tour[cities_chosen]]+=2;
	} else {
		for(i = 1; i < amount_of_cities; i++) {
			if(cost_array[tour[0]][tour[i]] < current_minimum) {
				second_minimum = current_minimum;
				current_second = current_first;
				current_minimum = cost_array[tour[0]][tour[i]];
				current_first = tour[i];
			} else if(cost_array[tour[0]][tour[i]] < second_minimum) {
				second_minimum = cost_array[tour[0]][tour[i]];
				current_second = tour[i];
			}
		}
	}
	degree[current_first]++;
	degree[current_second]++;
	degree[tour[0]]+=2;
	return current_minimum+second_minimum;
}

/* Get the weight of the Minimum Spanning Tree and record the vertex degrees */
double get_mst(unsigned int *tour, unsigned int cities_chosen) {
	unsigned int i, j, k, current_j;
	double weight, current_minimum;
	weight = 0;
	for(i = cities_chosen+2; i < amount_of_cities; i++) {
		current_minimum = DBL_MAX;
		for(j = cities_chosen+1; j < i; j++) {
			for(k = i; k < amount_of_cities; k++) {
				if(cost_array[tour[j]][tour[k]] < current_minimum) {
					current_minimum = cost_array[tour[j]][tour[k]];
					swap(&tour[i],&tour[k]);
					current_j = j;
				}
			}
		}
		degree[tour[current_j]]++;
		degree[tour[i]]++;
		weight += cost_array[tour[current_j]][tour[i]];
	}
	return weight;
}