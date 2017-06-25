/*!
 * \file thread_lib.h
 *
 * Contiene tutte le funzioni per l'esecuzione del programma tramite
 * uso di thread.
 */

#ifndef THREAD_LIB_H
#define THREAD_LIB_H

#include <pthread.h>
#include "io_lib.h"

/*! 
 * \brief       Rappresenta il ruolo di una thread
 * 
 * Il ruolo è fisso.
 * Può essere:
 *  - T_MULTIPLY: 	La thread si occupa delle operazioni necessarie a 
 *  			  	calcolare una riga della matrice C
 *  - T_SUM:      	La thread si occupa della somma degli elementi della
 *  			  	matrice C
 */
typedef enum {
    T_MULTIPLY,
    T_SUM
} thread_role_t;


/*!
 * \brief       Struct degli argomenti da passare ad ogni thread.
 */
typedef struct {
    int thread_id;
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

/*!
 * \brief      	Crea thread_number thread passando ad ogni thread gli argomenti corretti.
 *
 * \param[out] 	thread_array  	L'array dove vengono salvati gli oggetti di tipo thread_t.
 * \param[in]  	args          	L'array contnente alla posizione i gli argomenti da passare all'array i
 * \param[in]  	thread_number  	Il numero dei thread
 *
 * \return     	0 in caso di successo -1 altrimenti.
 */
int create_threads(pthread_t *thread_array, thread_arg_t *args, int thread_number);


/*!
 * \brief      	Punto di accesso per l'esecuzione del programma con le thread.
 *
 * \param[in] 	A_path  Path della matrice A
 * \param[in]  	B_path  Path della matrice B
 * \param[in]  	C_path  Path della matrice C
 * \param[in]  	N      	Dimensione delle matrici
 *
 * \return     	Stato di uscita del programma.
 */
int use_thread(char *A_path, char *B_path, char *C_path, int N);

/*!
 * \brief      	Salva una matrice su file in formato csv. 
 *
 * \param[in]  	fd      File descriptor
 * \param[in]  	matrix  La matrice da salvare
 * \param[in]  	N       Dimensione della matrice
 */
void matrix_to_csv(int fd, long *matrix, int N);

/**
 * \brief      	Fa il parsing di una matrice salvata su file in formato csv.
 *
 * \param[in]  	fd      File descriptor
 * \param[out] 	matrix  Matrice di long su cui salvarla.
 * \param[in]  	N       Dimensione della matrice
 */
void matrix_from_csv(int fd, long *matrix, int N);

/**
 * \brief      	Funzione eseguita dalle threads.
 *
 * \param[in]  	args  	Argomenti per la thread
 */
void *thread_callback(void *args);

#endif 