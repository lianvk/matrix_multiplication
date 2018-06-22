#define HAVE_STRUCT_TIMESPEC
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
#include <chrono>
#define  THR_NUM 4
#define  N 2000

int *alloc_matrix(int size) 
{
	int *matrix = (int*)malloc(size * size * sizeof(int));
	return matrix;
}

void del_matrix(int *matrix) 
{
	free(matrix);
}

struct matrix_args 
{
	int *m1;
	int *m2;
	int *ans;
	int size;
	int start;
	int end;
} ARG[100];

void *multiply_matrix(void *pargs) {
	struct matrix_args *args = (struct matrix_args *) pargs;
	int i, j, k, l, m, tmp = 0;;
	int *m1 = args->m1;
	int *m2 = args->m2;
	int *ans = args->ans;
	int size = args->size;
	int start = args->start;
	int end = args->end;
	for (i = start; i < end; i++) {
		m = i * size;
		for (j = 0; j < size; j++) {
			l = 0;
			for (k = 0; k < size; k++) {
				tmp += m1[m + k] * m2[j + l];
				l += size;
			}
			ans[m + j] = tmp;
			tmp = 0;
		}
	}
	pthread_exit(NULL);
	return 0;
}
int main(int argc, char** argv) {
	
	int i, j, size, k, step, pos, res;
	size = N;
	pthread_t threads[THR_NUM];
	pthread_attr_t attr;
	int *m1 = alloc_matrix(size);
	int *m2 = alloc_matrix(size);
	int *ans = alloc_matrix(size);
	srand(time(NULL));
	for (int i = 0; i<size; i++) {
		for (int j = 0; j<size; j++) {
			m1[i + j * size] = rand() ;
			m2[i + j * size] = rand();
		}
	}

	std::cout << "Multiplication started" << std::endl;
	auto startTime = std::chrono::system_clock::now();
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	step = (int)((double)size / (double)THR_NUM);
	pos = 0;
	for (k = 0; k < THR_NUM; k++) {
		ARG[k].m1 = m1;
		ARG[k].m2 = m2;
		ARG[k].ans = ans;
		ARG[k].size = size;
		ARG[k].start = pos;
		pos += step;
		ARG[k].end = (k == THR_NUM - 1) ? size : pos;
		res = pthread_create(&threads[k], &attr, multiply_matrix, (void *)&ARG[k]);
		if (res) {
			fprintf(stderr, "Error creating thread\n");
			exit(1);
		}
	}
	pthread_attr_destroy(&attr);
	for (k = 0; k < THR_NUM; k++)
		pthread_join(threads[k], NULL);
	std::chrono::duration<double> delay = std::chrono::system_clock::now() - startTime;
	std::cout << "Multiplication ended: delay = " << delay.count() << std::endl;

	del_matrix(m1);
	del_matrix(m2);
	del_matrix(ans);
	pthread_exit(NULL);
}