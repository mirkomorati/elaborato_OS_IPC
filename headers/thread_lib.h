/*!
 * \file thread_lib.h
 * file che contiene tutte le utility relative al ai thread 
 */

#ifndef THREAD_LIB_H
#define THREAD_LIB_H

#include <pthread.h>
#include "io_lib.h"

typedef enum {
    T_MULTIPLY,
    T_SUM
} thread_role_t;

typedef struct {
	pthread_mutex_t *sum_mutex;
	int *completed_rows;
	thread_role_t role;
	int row;
	int dimension;
	long *matrixA;
	long *matrixB;
	long *matrixC;
	long *sum;
} thread_arg_t;

/**
 * \brief      Crea thread_number thread passando ad ogni thread gli argomenti corretti.
 *
 * \param      thread_array  	L'array dove vengono salvati gli oggetti di tipo thread_t.
 * \param      args          	L'array contnente alla posizione i gli argomenti da passare all'array i
 * \param[in]  thread_number  	Il numero dei thread
 *
 * \return     0 in caso di successo -1 altrimenti.
 */
int create_threads(pthread_t *thread_array, thread_arg_t *args, int thread_number);

int use_thread(char *A_path, char *B_path, char *C_path, int N);

void matrix_to_csv(int fd, long *matrix, int N);

void matrix_from_csv(int fd, long *matrix, int N);

void * thread_callback(void * args);

#endif 