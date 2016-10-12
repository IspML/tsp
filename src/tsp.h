#ifndef TSP_H
#define TSP_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

typedef enum {RECURSIVE, LOOP} algorithm_t;
typedef enum {SORT_BY_MIN_DOOR, SORT_GREEDY, SORT_BY_HELD_KARP} sort_t;

/* TSP Macros */
#define ALGORITHM RECURSIVE
#define SORT SORT_BY_HELD_KARP
#define TWO_OPT_ENABLED 1
#define THREE_OPT_ENABLED 1

/* MPI Macros */
#define MESSAGE_UPPER_BOUND 0
#define MESSAGE_FINAL_RESULT 1

/* Timer Macro */
#define TIMER_ENABLED 0

/* TSP Variables */
unsigned int amount_of_cities;
unsigned int **weight_array;
unsigned int *city_array;
unsigned int *swap_array;
unsigned int *i_array;
unsigned int *tmp_array;
unsigned int upper_bound;
unsigned int *min_door;
unsigned int *best_route;
unsigned int absolute_min_distance; /* The minimum distance of everything, NOT multiplied by 2 */
unsigned int current_weight;
unsigned int current_weight_guess;
unsigned int min_door_adaptive_depth;
unsigned int order_first; /* Helps keep order: first must always be before second - thus we can ignore half of the paths */
unsigned int order_second;
unsigned int order_first_taken;
unsigned int *degree;
unsigned int held_karp_main;
unsigned int held_karp_current;
unsigned int held_karp_depth;
double *penalty;
double **cost_array;

/* MPI Variables */
unsigned int mpi_enabled;
unsigned int comm_rank;
unsigned int comm_size;
unsigned int split_depth;
MPI_Status status_receive;
MPI_Status status_send;
MPI_Request request_receive;
MPI_Request request_send;
MPI_Request *requests_send;
MPI_Request request_receive_final;
unsigned int upper_bound_received;
unsigned int upper_bound_modified;
unsigned int upper_bound_sending;
unsigned int final_result_received;
unsigned int best_process;
char final_action;
char print_action;

/* Timer Variables */
unsigned long total_time;
clock_t start_time;
clock_t end_time;

/* Main TSP Functions */
void tsp_init();
void search_loop(unsigned int city, unsigned int weight, unsigned int amount_visited);
void search_recursive(unsigned int city, unsigned int weight, unsigned int weight_guess, unsigned int visited_cities);
void tsp_end();
void print_best_tour();

/* TSP Help Functions */
void swap(unsigned int* a, unsigned int* b);
void swap_reverse(unsigned int *tour, unsigned int start, unsigned int end, unsigned int max_tour_length);
unsigned int swap_three_opt(unsigned int start, unsigned int middle, unsigned int end);
unsigned int get_tour_length(unsigned int* tour);
void two_opt();
void three_opt();
void sort_greedy(unsigned int *tour, unsigned int increasing);
void sort_by_min_door(unsigned int *tour, unsigned int increasing);
void sort_by_held_karp(unsigned int *tour, unsigned int increasing);
unsigned int get_min_door(unsigned int amount_visited, unsigned int chosen_index);
unsigned int get_held_karp(unsigned int *tour, unsigned int target, unsigned int edges_chosen);
double get_two_nearest(unsigned int *tour, unsigned int edges_chosen);
double get_mst(unsigned int *tour, unsigned int edges_chosen);

/* MPI Funcions*/
unsigned int calculate_procnumber();
void send_upper_bound();
void receive_upper_bound();
void send_receive_best_tour();

#endif
