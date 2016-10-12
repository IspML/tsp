#include "tsp.h"
/* This file contains all MPI-related functions used to solve the TSP */

/* Calculate the process number assigned to a branch according to the i-array */
unsigned int calculate_procnumber() {
	unsigned long long int node_number, multiplier;
	unsigned int i;
	if(!mpi_enabled) return 0;
	node_number = 0;
	multiplier = 1;
	for(i = split_depth; i >= 1; i--) {
		node_number += ((unsigned long long int)(i_array[i-1]-i))*multiplier;
		multiplier *= ((unsigned long long int)amount_of_cities-i-1);
	}
	return (unsigned int)(node_number % (unsigned long long int)comm_size);
}

/* Receive the upper bound */
void receive_upper_bound() {
	int flag_receive;
	if(!mpi_enabled) return;
	if(request_receive == MPI_REQUEST_NULL){
		if(comm_rank == 0){
			MPI_Irecv(&upper_bound_received,1,MPI_UNSIGNED,MPI_ANY_SOURCE,MESSAGE_UPPER_BOUND,MPI_COMM_WORLD,&request_receive);
		} else MPI_Irecv(&upper_bound_received,1,MPI_UNSIGNED,0,MESSAGE_UPPER_BOUND,MPI_COMM_WORLD,&request_receive);
	}
	MPI_Test(&request_receive,&flag_receive,&status_receive);
	if(flag_receive) {
		request_receive = MPI_REQUEST_NULL;
		if(upper_bound_received < upper_bound) {
			upper_bound = upper_bound_received;
			if(comm_rank == 0) {
				best_process = status_receive.MPI_SOURCE;
				upper_bound_modified = 1;
				send_upper_bound();
			} else upper_bound_modified = 0;
		}
	}
}

/* Send the upper bound */
void send_upper_bound() {
	int flag_send;
	if(!mpi_enabled) return;
	if(comm_rank != 0) {
		if(request_send == MPI_REQUEST_NULL) {
			if(upper_bound_modified == 1) {
				upper_bound_modified = 0;
				upper_bound_sending = upper_bound;
				MPI_Isend(&upper_bound_sending,1,MPI_UNSIGNED,0,MESSAGE_UPPER_BOUND,MPI_COMM_WORLD,&request_send);
			}
		}
		MPI_Test(&request_send,&flag_send,&status_send);
		if(flag_send) request_send = MPI_REQUEST_NULL;
	} else {
		if(requests_send[comm_size-2] == MPI_REQUEST_NULL) {
			if(upper_bound_modified == 1) {
				unsigned int i;
				upper_bound_modified = 0;
				upper_bound_sending = upper_bound;
				for(i=1;i<comm_size;i++) {
					MPI_Isend(&upper_bound_sending,1,MPI_UNSIGNED,i,MESSAGE_UPPER_BOUND,MPI_COMM_WORLD,&(requests_send[i-1]));
				}
			}
		} else {
			MPI_Testall(comm_size-1,requests_send,&flag_send,MPI_STATUSES_IGNORE);
			if(flag_send) {
				unsigned int i;
				for(i=0; i<comm_size-1; i++) {
					requests_send[i] == MPI_REQUEST_NULL;
				}
			}
		}
	}
}

/* Send and receive the best tour at the end of the algorithm */
void send_receive_best_tour() {
	if(!mpi_enabled) return;
	if(comm_rank != 0) {
		MPI_Send(&upper_bound,1,MPI_UNSIGNED,0,MESSAGE_FINAL_RESULT,MPI_COMM_WORLD);
		MPI_Recv(&final_action,1,MPI_CHAR,0,MESSAGE_FINAL_RESULT,MPI_COMM_WORLD,&status_receive);
		if(final_action == 'p') print_best_tour();
		/* The process ends here */
	} else {
		unsigned int amount_finished, flag_receive, i;
		amount_finished = 1;
		while(amount_finished < comm_size) {
			receive_upper_bound();
			send_upper_bound();
			if(request_receive_final == MPI_REQUEST_NULL){
				MPI_Irecv(&final_result_received,1,MPI_UNSIGNED,MPI_ANY_SOURCE,MESSAGE_FINAL_RESULT,MPI_COMM_WORLD,&request_receive_final);
			}
			MPI_Test(&request_receive_final,&flag_receive,&status_receive);
			if(flag_receive) {
				amount_finished++;
				request_receive_final = MPI_REQUEST_NULL;
				if(final_result_received < upper_bound) {
					upper_bound = upper_bound_received;
					best_process = status_receive.MPI_SOURCE;
					upper_bound_modified = 0;
				}
			}
		}
		if(best_process != 0) {
			MPI_Send(&print_action,1,MPI_CHAR,best_process,MESSAGE_FINAL_RESULT,MPI_COMM_WORLD);
		} else print_best_tour();
		for(i=1; i<comm_size; i++) {
			if(i == best_process) continue;
			MPI_Send(&final_action,1,MPI_CHAR,i,MESSAGE_FINAL_RESULT,MPI_COMM_WORLD);
		}
	}
}
