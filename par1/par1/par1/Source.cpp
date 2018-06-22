#include <stdio.h>
#include <chrono>
#include <mpi.h>
#include <iostream>

#define N 2000 
#define MASTER_TO_SLAVE_TAG 1 
#define SLAVE_TO_MASTER_TAG 4 

void makeAB(); 
int rank; 
int num_proc; //number of process
int i, j, k; 
float a[N][N]; 
float b[N][N]; 
float c[N][N]; 
int low_bound; 
int upper_bound; 
int portion; 
MPI_Status status; 
MPI_Request request; 

int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv); 
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
	MPI_Comm_size(MPI_COMM_WORLD, &num_proc); 	

	if (rank == 0) {
		for (int j1 = 0; j1 < N; j1++)
		{
			for (int j2 = 0; j2 < N; j2++)
			{
				a[j1][j2] = rand();
				b[j1][j2] = rand();
				c[j1][j2] = 0;
			}
		}
		for (i = 1; i < num_proc; i++) {
			portion = (N / (num_proc - 1)); 
			low_bound = (i - 1) * portion;
			if (((i + 1) == num_proc) && ((N % (num_proc - 1)) != 0)) {
				upper_bound = N; 
			}
			else {
				upper_bound = low_bound + portion; 
			}
			
			MPI_Isend(&low_bound, 1, MPI_INT, i, MASTER_TO_SLAVE_TAG, MPI_COMM_WORLD, &request);
			
			MPI_Isend(&upper_bound, 1, MPI_INT, i, MASTER_TO_SLAVE_TAG + 1, MPI_COMM_WORLD, &request);
			
			MPI_Isend(&a[low_bound][0], (upper_bound - low_bound) * N, MPI_DOUBLE, i, MASTER_TO_SLAVE_TAG + 2, MPI_COMM_WORLD, &request);
		}
	}
	MPI_Bcast(&b, N*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	std::cout << "Multiplication started" << std::endl;
	auto startTime = std::chrono::system_clock::now();

	if (rank > 0) {
		
		MPI_Recv(&low_bound, 1, MPI_INT, 0, MASTER_TO_SLAVE_TAG, MPI_COMM_WORLD, &status);
		
		MPI_Recv(&upper_bound, 1, MPI_INT, 0, MASTER_TO_SLAVE_TAG + 1, MPI_COMM_WORLD, &status);
		
		MPI_Recv(&a[low_bound][0], (upper_bound - low_bound) * N, MPI_DOUBLE, 0, MASTER_TO_SLAVE_TAG + 2, MPI_COMM_WORLD, &status);

		for (i = low_bound; i < upper_bound; i++) {
			for (j = 0; j < N; j++) {
				for (k = 0; k < N; k++) {
					c[i][j] += (a[i][k] * b[k][j]);
				}
			}
		}
		
		MPI_Isend(&low_bound, 1, MPI_INT, 0, SLAVE_TO_MASTER_TAG, MPI_COMM_WORLD, &request);
		
		MPI_Isend(&upper_bound, 1, MPI_INT, 0, SLAVE_TO_MASTER_TAG + 1, MPI_COMM_WORLD, &request);
		
		MPI_Isend(&c[low_bound][0], (upper_bound - low_bound) * N, MPI_DOUBLE, 0, SLAVE_TO_MASTER_TAG + 2, MPI_COMM_WORLD, &request);
	}
	
	if (rank == 0) {
		for (i = 1; i < num_proc; i++) {

			MPI_Recv(&low_bound, 1, MPI_INT, i, SLAVE_TO_MASTER_TAG, MPI_COMM_WORLD, &status);
			
			MPI_Recv(&upper_bound, 1, MPI_INT, i, SLAVE_TO_MASTER_TAG + 1, MPI_COMM_WORLD, &status);
			
			MPI_Recv(&c[low_bound][0], (upper_bound - low_bound) * N, MPI_DOUBLE, i, SLAVE_TO_MASTER_TAG + 2, MPI_COMM_WORLD, &status);
		}
	}
	std::chrono::duration<double> delay = std::chrono::system_clock::now() - startTime;
	std::cout << "Multiplication ended: delay = " << delay.count() << std::endl;
	MPI_Finalize(); //finalize MPI operations
	return 0;
}

