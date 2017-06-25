/*!
 * \file father.h
 */

#ifndef FATHER_H
#define FATHER_H
#include "std_lib.h"
#include "shm_lib.h"
#include "ending_lib.h"
#include "sem_lib.h"
#include "msg_lib.h"
#include "child.h"
#include "io_lib.h"
#include "thread_lib.h"


/*!
 * \brief      	Inizializza la memoria condivisa ed i semafori
 *
 * \param      	shm_array  	L'array delle memorie condivise da creare
 * \param      	sem_ids    	ID dei semafori da inizializzare
 * \param[in]  	P          	Numero di processi
 *
 * \return     	0 in caso di successo, -1 altrimenti
 */
int init(shm_t **shm_array, lock_t *sem_ids, int P);

/*!
 * \brief      	Crea P figli del padre ed inizializza gli strumenti per
 *             	permettere la comunicazione tra padre e figli.
 *
 * \param[in]  	shm_array 		L'array contenente i puntatori alle zone di memoria
 *                          	condivisa.
 * \param[in]  	sem_ids      	ID dei semafori.
 * \param[in]  	P            	Il numero di figli da creare.
 * \param[in]  	pid_to_pipe 	Array per associare al pid la pipe corrispondente.
 * \param[out] 	queue_id     	In questo indirizzo viene salvato l'identificativo
 *                          	della coda da cui leggere le comunicazioni dei
 *                          	figli.
 * \param[out] 	pipe_fd      	In questo puntatore viene salvato il file descriptor per scrivere sulla pipe.
 *
 * \return     	0 in caso di riuscita, -1 altrimenti
 */
int make_child(shm_t **shm_array, lock_t *sem_ids, int P, int *pid_to_pipe, int *queue_id);

/*!
 * \brief      	Si occupa di tutto quello che avviene nel core del
 *             	programma: effettua un loop in cui continua a chiedere ai figli
 *             	di fare un lavoro e controlla che lo abbiano svolto
 *             	correttamente, se un figli ci riesce allora, il padre legge il
 *             	risultato ed invia un nuovo comando
 *
 * \param[in]  	N            Ordine delle matrici da moltiplicare
 * \param[in]  	P            Il numero di figli sui quali deve ciclare
 * \param[in]  	pid_to_pipe  Array per associare al pid la pipe corrispondente
 * \param[in]  	queue        La coda dalla quale deve leggere
 * \param[in]  	sem_ids      ID dei semafori
 *
 * \return     	Il codice di uscita del programma
 */
int run(int N, int P, int *pid_to_pipe, int queue, lock_t *sem_ids);

/*!
 * \brief      	Crea un array di comandi per la moltiplicazione e uno per la somma.
 *
 * \param[out] 	multiply_array 	L'array per i comandi di moltiplicazione
 * \param[out] 	sum_array       L'array per i comandi di somma
 * \param[in]  	N               Dimensione della matrice. Il numero di comandi totale è N * (N + 1)
 */

void generate_cmd_array(cmd_t *multiply_array, cmd_t *sum_array, int N);

#endif